#include <string.h>
#include <stdio.h>
#include "esp_compat.h"
#include "../../firmware/src/rcc_link.h"
#include "../../firmware/src/hmi_settings.h"
#include "../../firmware/src/hmi_backlight.h"
#include "../../firmware/src/hmi_wifi.h"
#include "../../firmware/src/hmi_ota.h"

static hmi_settings_t s_settings = {
    .brightness_pct = 100,
    .sleep_timeout_s = 180,
    .dim_pct = 15,
    .language = HMI_LANG_EN,
};

const hmi_settings_t *hmi_settings_get(void) { return &s_settings; }
esp_err_t hmi_settings_save(const hmi_settings_t *cfg) { if (cfg) s_settings = *cfg; return ESP_OK; }
esp_err_t hmi_settings_set_brightness(uint8_t pct) { s_settings.brightness_pct = pct; return ESP_OK; }
esp_err_t hmi_settings_set_sleep_timeout(uint16_t s) { s_settings.sleep_timeout_s = s; return ESP_OK; }
esp_err_t hmi_settings_set_dim_pct(uint8_t pct) { s_settings.dim_pct = pct; return ESP_OK; }
esp_err_t hmi_settings_set_language(uint8_t lang) { s_settings.language = lang; return ESP_OK; }

void hmi_backlight_set_duty(uint8_t pct, bool fade) { (void)pct; (void)fade; }
void hmi_backlight_notify_activity(void) {}
bool hmi_backlight_is_dimmed(void) { return false; }
void hmi_backlight_tick(bool is_idle) { (void)is_idle; }

esp_err_t rcc_link_start(void) { return ESP_OK; }
bool rcc_link_request_start(void) { return true; }
bool rcc_link_request_stop(void) { return true; }

void rcc_link_snapshot(rcc_snapshot_t *out) {
    if (!out) return;
    memset(out, 0, sizeof(*out));
    out->online = true;
    out->device_valid = true;
    out->status_valid = true;
    out->measurement_valid = true;
    out->faults_valid = true;
    out->reply_age_ms = 15;
    out->status_age_ms = 15;
    out->measurement_age_ms = 15;
    out->top_state = 4;        // OPERATIONAL
    out->operational_state = 1; // IDLE
    out->product_id = 1;
    out->protocol_version = 1;
    out->hardware_revision = 0x01;
    out->firmware_revision = 0x01;
    out->vout_mv = 48500;
    out->iout_ma = 0;
    out->vout_filtered_mv = 48500;
    out->iout_filtered_ma = 0;
    out->build_flags = 3;      // provisional + relay enable
    out->running_links = 4;    // UART active
}

static hmi_wifi_status_t s_wifi = {
    .state = HMI_WIFI_CONNECTED,
    .ssid = "RCC_Workshop_5G",
    .ip = "192.168.1.150",
    .gateway = "192.168.1.1",
    .mac = "B8:1F:3F:C4:EC:74",
    .rssi = -58,
    .ap_count = 2,
    .ap = {
        {"RCC_Workshop_5G", -58, 1},
        {"Station_Lab_WiFi", -72, 1}
    },
    .version = 1,
};

bool hmi_wifi_init(void) { return true; }
void hmi_wifi_get(hmi_wifi_status_t *out) { if (out) *out = s_wifi; }
void hmi_wifi_scan(void) { s_wifi.state = HMI_WIFI_SCANNING; s_wifi.version++; }
void hmi_wifi_connect(const char *ssid, const char *pass) {
    (void)pass;
    if (ssid) snprintf(s_wifi.ssid, sizeof(s_wifi.ssid), "%s", ssid);
    s_wifi.state = HMI_WIFI_CONNECTED;
    s_wifi.version++;
}
void hmi_wifi_cancel(void) { s_wifi.state = HMI_WIFI_IDLE; s_wifi.version++; }
void hmi_wifi_disconnect(void) { s_wifi.state = HMI_WIFI_IDLE; s_wifi.version++; }
void hmi_wifi_forget(void) { s_wifi.ssid[0] = '\0'; s_wifi.state = HMI_WIFI_IDLE; s_wifi.version++; }

static hmi_ota_status_t s_mock_ota = {
    .state = HMI_OTA_IDLE,
    .progress_pct = 0,
    .running_slot = "ota_0",
    .next_slot = "ota_1",
    .running_version = "v3.2 Bento",
    .version = 1,
};

esp_err_t hmi_ota_start_server(void) { return ESP_OK; }
void hmi_ota_stop_server(void) {}
bool hmi_ota_server_is_running(void) { return true; }
void hmi_ota_get_status(hmi_ota_status_t *out) { if (out) *out = s_mock_ota; }
esp_err_t hmi_ota_mark_valid(void) { return ESP_OK; }


