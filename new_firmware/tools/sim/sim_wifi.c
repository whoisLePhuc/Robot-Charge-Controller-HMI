/* Scripted Wi-Fi backend for the PC simulator. Password for every secured network: "msvn1234".
 * "Warehouse-AP" is out of range (NO_AP), "RCC-Service" never gets an IP (NO_IP). */
#include <stdio.h>
#include <string.h>
#include "lvgl.h"
#include "hmi_wifi.h"

#define STEP_MS 700u
static hmi_wifi_status_t st;
static bool inited;
static uint32_t t0;
static char pw[64];

static void add(const char *s, int8_t r, uint8_t sec)
{
    hmi_wifi_ap_t *a = &st.ap[st.ap_count++];
    snprintf(a->ssid, sizeof(a->ssid), "%s", s); a->rssi = r; a->secure = sec;
}
static void init(void)
{
    if (inited) return;
    inited = true;
    st.state = HMI_WIFI_CONNECTED;
    snprintf(st.ssid, sizeof(st.ssid), "MSVN-Lab-5G");
    snprintf(st.ip, sizeof(st.ip), "192.168.1.42"); snprintf(st.gateway, sizeof(st.gateway), "192.168.1.1");
    snprintf(st.mac, sizeof(st.mac), "A4:CF:12:9B:3E:70");
    st.rssi = -48; st.version = 1;
    add("MSVN-Lab-5G", -48, 1); add("MSVN-Office", -61, 1); add("Factory-IoT", -72, 1);
    add("Guest", -83, 0); add("RCC-Service", -86, 1); add("Warehouse-AP", -89, 1);
}
static void advance(void)
{
    if (st.state != HMI_WIFI_CONNECTING) return;
    const uint32_t el = lv_tick_elaps(t0);
    uint8_t step = (uint8_t)(el / STEP_MS);
    if (step > 3) step = 3;
    if (step < 3) { if (st.step != step) { st.step = step; ++st.version; } return; }
    const bool open = !strcmp(st.ssid, "Guest");
    ++st.version;
    if (!strcmp(st.ssid, "Warehouse-AP")) { st.state = HMI_WIFI_FAILED; st.fail_reason = HMI_WIFI_FAIL_NO_AP; return; }
    if (!open && strcmp(pw, "msvn1234")) { st.state = HMI_WIFI_FAILED; st.fail_reason = HMI_WIFI_FAIL_PASSWORD; return; }
    if (!strcmp(st.ssid, "RCC-Service")) { st.state = HMI_WIFI_FAILED; st.fail_reason = HMI_WIFI_FAIL_NO_IP; return; }
    st.state = HMI_WIFI_CONNECTED; st.fail_reason = 0;
    for (int i = 0; i < st.ap_count; ++i) if (!strcmp(st.ap[i].ssid, st.ssid)) st.rssi = st.ap[i].rssi;
}
bool hmi_wifi_init(void) { return true; }
void hmi_wifi_get(hmi_wifi_status_t *out) { init(); advance(); *out = st; }
void hmi_wifi_scan(void) { init(); ++st.version; }
void hmi_wifi_connect(const char *ssid, const char *password)
{
    init();
    snprintf(st.ssid, sizeof(st.ssid), "%s", ssid); snprintf(pw, sizeof(pw), "%s", password);
    st.state = HMI_WIFI_CONNECTING; st.step = 0; ++st.attempt; st.fail_reason = 0; ++st.version;
    t0 = lv_tick_get();
}
void hmi_wifi_cancel(void) { init(); st.state = HMI_WIFI_IDLE; ++st.version; }
void hmi_wifi_disconnect(void) { init(); st.state = HMI_WIFI_IDLE; st.attempt = 0; ++st.version; }
void hmi_wifi_forget(void) { hmi_wifi_disconnect(); }
