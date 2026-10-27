/* Wi-Fi station backend (ESP-IDF esp_wifi) for the HMI Wi-Fi screens. See hmi_wifi.h for the contract.
 *
 * Threading: hmi_wifi_* commands run on the LVGL task, the event handler on the default event loop
 * task; every access to `s_st` is under `s_lock`. Credentials of the last successful connection are
 * kept in NVS (namespace "hmi_wifi", plain text) and used to auto-connect after boot. */
#include "hmi_wifi.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_timer.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "nvs.h"
#include "hmi_ota.h"

static const char *TAG = "hmi_wifi";

#define NVS_NS              "hmi_wifi"
#define CONNECT_TIMEOUT_MS  20000
#define MAX_RETRIES         2

static hmi_wifi_status_t s_st;
static SemaphoreHandle_t s_lock;
static esp_netif_t *s_netif;
static bool s_inited, s_scanning, s_user_started, s_skip_disc, s_auto;
static int64_t s_t_start_ms;
static uint8_t s_retries;
static char s_pass[HMI_WIFI_PASS_MAX + 1];

static int64_t now_ms(void) { return esp_timer_get_time() / 1000; }
static void lock(void) { xSemaphoreTake(s_lock, portMAX_DELAY); }
static void unlock(void) { xSemaphoreGive(s_lock); }
static void touch(void) { ++s_st.version; }

/* ---- credentials */
static bool cred_load(char *ssid, size_t ssid_n, char *pass, size_t pass_n)
{
    nvs_handle_t h;
    if (nvs_open(NVS_NS, NVS_READONLY, &h) != ESP_OK) return false;
    size_t a = ssid_n, b = pass_n;
    const bool ok = nvs_get_str(h, "ssid", ssid, &a) == ESP_OK && nvs_get_str(h, "pass", pass, &b) == ESP_OK && ssid[0];
    nvs_close(h);
    return ok;
}
static void cred_save(const char *ssid, const char *pass)
{
    nvs_handle_t h;
    if (nvs_open(NVS_NS, NVS_READWRITE, &h) != ESP_OK) return;
    if (nvs_set_str(h, "ssid", ssid) == ESP_OK && nvs_set_str(h, "pass", pass) == ESP_OK) (void)nvs_commit(h);
    nvs_close(h);
}
static void cred_erase(void)
{
    nvs_handle_t h;
    if (nvs_open(NVS_NS, NVS_READWRITE, &h) != ESP_OK) return;
    (void)nvs_erase_all(h);
    (void)nvs_commit(h);
    nvs_close(h);
}

/* ---- connection control (caller holds the lock) */
static void begin_connect(const char *ssid, const char *pass)
{
    wifi_config_t cfg;
    memset(&cfg, 0, sizeof(cfg));
    const size_t sl = strlen(ssid) < sizeof(cfg.sta.ssid) ? strlen(ssid) : sizeof(cfg.sta.ssid);
    memcpy(cfg.sta.ssid, ssid, sl);
    const size_t pl = strlen(pass) < sizeof(cfg.sta.password) - 1 ? strlen(pass) : sizeof(cfg.sta.password) - 1;
    memcpy(cfg.sta.password, pass, pl);
    cfg.sta.threshold.authmode = pass[0] ? WIFI_AUTH_WPA2_PSK : WIFI_AUTH_OPEN;
    cfg.sta.pmf_cfg.capable = true;
    cfg.sta.pmf_cfg.required = false;
    cfg.sta.sae_pwe_h2e = WPA3_SAE_PWE_BOTH;

    snprintf(s_st.ssid, sizeof(s_st.ssid), "%s", ssid);
    snprintf(s_pass, sizeof(s_pass), "%s", pass);
    s_st.state = HMI_WIFI_CONNECTING;
    s_st.step = 0;
    s_st.fail_reason = HMI_WIFI_FAIL_NONE;
    s_t_start_ms = now_ms();
    touch();
    if (esp_wifi_set_config(WIFI_IF_STA, &cfg) != ESP_OK || esp_wifi_connect() != ESP_OK) {
        ESP_LOGW(TAG, "esp_wifi_connect failed");
        s_st.state = s_user_started ? HMI_WIFI_FAILED : HMI_WIFI_IDLE;
        s_st.fail_reason = HMI_WIFI_FAIL_OTHER;
    }
}
static void drop_link(void)
{
    if (s_st.state == HMI_WIFI_CONNECTED || s_st.state == HMI_WIFI_CONNECTING) {
        s_skip_disc = true;           /* the DISCONNECTED event that follows is ours, not a failure */
        (void)esp_wifi_disconnect();
    }
}
static void fail(hmi_wifi_fail_t why)
{
    s_st.state = s_user_started ? HMI_WIFI_FAILED : HMI_WIFI_IDLE;
    s_st.fail_reason = (uint8_t)why;
    touch();
}

/* ---- events */
static hmi_wifi_fail_t map_reason(uint8_t r)
{
    switch (r) {
    case WIFI_REASON_NO_AP_FOUND: return HMI_WIFI_FAIL_NO_AP;
    case WIFI_REASON_AUTH_FAIL:
    case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT:
    case WIFI_REASON_HANDSHAKE_TIMEOUT:
    case WIFI_REASON_MIC_FAILURE:
    case WIFI_REASON_AUTH_EXPIRE: return HMI_WIFI_FAIL_PASSWORD;
    default: return HMI_WIFI_FAIL_OTHER;
    }
}
static void on_scan_done(void)
{
    /* wifi_ap_record_t is ~110 bytes: 24 of them on the default event-loop task stack (2.3 KB) overflow it
     * and reset the chip when a scan finishes, so the records live on the heap. */
    const uint16_t cap = HMI_WIFI_MAX_AP * 2;
    wifi_ap_record_t *rec = malloc(sizeof(*rec) * cap);
    uint16_t n = cap;
    if (!rec || esp_wifi_scan_get_ap_records(&n, rec) != ESP_OK) n = 0;
    s_scanning = false;
    s_st.ap_count = 0;
    for (uint16_t i = 0; i < n && s_st.ap_count < HMI_WIFI_MAX_AP; ++i) {
        const char *name = (const char *)rec[i].ssid;
        if (!name[0]) continue;
        bool dup = false;
        for (int k = 0; k < s_st.ap_count; ++k) if (!strcmp(s_st.ap[k].ssid, name)) { dup = true; break; }
        if (dup) continue;               /* records are sorted by RSSI: the first one is the strongest */
        hmi_wifi_ap_t *a = &s_st.ap[s_st.ap_count++];
        snprintf(a->ssid, sizeof(a->ssid), "%s", name);
        a->rssi = rec[i].rssi;
        a->secure = rec[i].authmode != WIFI_AUTH_OPEN;
    }
    free(rec);
    ESP_LOGI(TAG, "Wi-Fi scan done! Retrieved %u AP records, saved %u APs to UI (free heap: %zu internal)",
             n, s_st.ap_count, heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
    for (int i = 0; i < s_st.ap_count; ++i) {
        ESP_LOGI(TAG, "  [%d] SSID: '%s', RSSI: %d dBm, Sec: %d",
                 i, s_st.ap[i].ssid, s_st.ap[i].rssi, s_st.ap[i].secure);
    }
    touch();
}
static void on_wifi_event(void *arg, esp_event_base_t base, int32_t id, void *data)
{
    (void)arg; (void)base;
    lock();
    switch (id) {
    case WIFI_EVENT_STA_START: {
        char ssid[33], pass[HMI_WIFI_PASS_MAX + 1];
        if (s_auto && cred_load(ssid, sizeof(ssid), pass, sizeof(pass))) { s_st.attempt = 1; begin_connect(ssid, pass); }
        break; }
    case WIFI_EVENT_SCAN_DONE:
        on_scan_done();
        break;
    case WIFI_EVENT_STA_CONNECTED:
        if (s_st.state == HMI_WIFI_CONNECTING) { s_st.step = 2; touch(); }
        break;
    case WIFI_EVENT_STA_DISCONNECTED: {
        const wifi_event_sta_disconnected_t *d = (const wifi_event_sta_disconnected_t *)data;
        if (s_skip_disc) { s_skip_disc = false; break; }
        if (s_st.state == HMI_WIFI_CONNECTING) {
            const hmi_wifi_fail_t why = map_reason(d->reason);
            if (why == HMI_WIFI_FAIL_OTHER && s_retries < MAX_RETRIES) { ++s_retries; (void)esp_wifi_connect(); }
            else fail(why);
        } else if (s_st.state == HMI_WIFI_CONNECTED) {
            s_st.state = HMI_WIFI_IDLE;
            touch();
            (void)hmi_ota_stop_server();
            if (s_auto) { s_retries = 0; ++s_st.attempt; begin_connect(s_st.ssid, s_pass); }   /* link dropped: reconnect */
        }
        break; }
    default: break;
    }
    unlock();
}
static void on_ip_event(void *arg, esp_event_base_t base, int32_t id, void *data)
{
    (void)arg; (void)base; (void)id;
    const ip_event_got_ip_t *e = (const ip_event_got_ip_t *)data;
    lock();
    snprintf(s_st.ip, sizeof(s_st.ip), IPSTR, IP2STR(&e->ip_info.ip));
    snprintf(s_st.gateway, sizeof(s_st.gateway), IPSTR, IP2STR(&e->ip_info.gw));
    uint8_t mac[6] = {0};
    (void)esp_wifi_get_mac(WIFI_IF_STA, mac);
    snprintf(s_st.mac, sizeof(s_st.mac), "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    wifi_ap_record_t ap;
    s_st.rssi = esp_wifi_sta_get_ap_info(&ap) == ESP_OK ? ap.rssi : 0;
    s_st.state = HMI_WIFI_CONNECTED;
    s_st.fail_reason = HMI_WIFI_FAIL_NONE;
    s_auto = true;
    cred_save(s_st.ssid, s_pass);
    touch();
    unlock();
    (void)hmi_ota_start_server();
}

/* ---- public API */
bool hmi_wifi_init(void)
{
    if (s_inited) return true;
    s_lock = xSemaphoreCreateMutex();
    if (!s_lock) return false;
    esp_err_t err = esp_netif_init();
    if (err == ESP_OK) err = esp_event_loop_create_default();
    if (err == ESP_ERR_INVALID_STATE) err = ESP_OK;        /* loop already exists */
    if (err != ESP_OK) { ESP_LOGE(TAG, "netif/event loop: %s", esp_err_to_name(err)); return false; }
    s_netif = esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    err = esp_wifi_init(&cfg);
    if (err != ESP_OK) { ESP_LOGE(TAG, "esp_wifi_init: %s", esp_err_to_name(err)); return false; }
    (void)esp_wifi_set_storage(WIFI_STORAGE_RAM);          /* we keep our own credentials */
    (void)esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, on_wifi_event, NULL);
    (void)esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, on_ip_event, NULL);
    s_auto = true;
    s_st.state = HMI_WIFI_IDLE;
    (void)esp_wifi_set_mode(WIFI_MODE_STA);
    err = esp_wifi_start();
    if (err != ESP_OK) { ESP_LOGE(TAG, "esp_wifi_start: %s", esp_err_to_name(err)); return false; }
    s_inited = true;
    ESP_LOGI(TAG, "station started");
    return true;
}

void hmi_wifi_get(hmi_wifi_status_t *out)
{
    if (!s_inited) { memset(out, 0, sizeof(*out)); out->state = HMI_WIFI_UNAVAILABLE; return; }
    lock();
    if (s_st.state == HMI_WIFI_CONNECTING && now_ms() - s_t_start_ms > CONNECT_TIMEOUT_MS) {
        drop_link();
        fail(s_st.step >= 2 ? HMI_WIFI_FAIL_NO_IP : HMI_WIFI_FAIL_OTHER);
    }
    if (s_st.state == HMI_WIFI_CONNECTED) {
        wifi_ap_record_t ap;
        if (esp_wifi_sta_get_ap_info(&ap) == ESP_OK) s_st.rssi = ap.rssi;
    }
    *out = s_st;
    if (out->state == HMI_WIFI_IDLE && s_scanning) out->state = HMI_WIFI_SCANNING;
    unlock();
}

void hmi_wifi_scan(void)
{
    if (!s_inited) return;
    lock();
    if (s_st.state != HMI_WIFI_CONNECTING && !s_scanning) {
        ESP_LOGI(TAG, "Wi-Fi scan initiated (free internal heap: %zu bytes)...",
                 heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
        const wifi_scan_config_t sc = { .show_hidden = false, .scan_type = WIFI_SCAN_TYPE_ACTIVE };
        if (esp_wifi_scan_start(&sc, false) == ESP_OK) { s_scanning = true; touch(); }
        else { ESP_LOGW(TAG, "esp_wifi_scan_start returned error"); }
    }
    unlock();
}

void hmi_wifi_connect(const char *ssid, const char *password)
{
    if (!s_inited || !ssid || !ssid[0]) return;
    lock();
    s_user_started = true;
    s_auto = true;
    s_retries = 0;
    if (!password) password = "";
    drop_link();
    ++s_st.attempt;
    begin_connect(ssid, password);
    unlock();
}

void hmi_wifi_cancel(void)
{
    if (!s_inited) return;
    lock();
    if (s_st.state == HMI_WIFI_CONNECTING) {
        drop_link();
        s_st.state = HMI_WIFI_IDLE;
        s_st.attempt = 0;
        touch();
    }
    unlock();
}

void hmi_wifi_disconnect(void)
{
    if (!s_inited) return;
    lock();
    s_auto = false;                   /* stay offline until the user connects again */
    drop_link();
    s_st.state = HMI_WIFI_IDLE;
    s_st.attempt = 0;
    touch();
    unlock();
}

void hmi_wifi_forget(void)
{
    hmi_wifi_disconnect();
    if (s_inited) cred_erase();
}
