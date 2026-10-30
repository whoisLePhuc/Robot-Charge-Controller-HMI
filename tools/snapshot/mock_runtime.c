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

/* Illustrative controller data for the README previews (not a recorded session). */
static rcc_snapshot_t s_rcc;

static void add_event(uint16_t code, uint32_t since_boot_s, uint32_t a0, uint32_t a1, uint32_t a2)
{
    const uint8_t i = s_rcc.event_count++;
    s_rcc.event_codes[i] = code;
    s_rcc.event_sequences[i] = i + 1u;
    s_rcc.event_us[i] = (uint64_t)since_boot_s * 1000000u;
    s_rcc.event_args[i][0] = a0;
    s_rcc.event_args[i][1] = a1;
    s_rcc.event_args[i][2] = a2;
}
static uint32_t state_pair(uint8_t t0, uint8_t o0, uint8_t t1, uint8_t o1)
{
    return (uint32_t)t0 | ((uint32_t)o0 << 8) | ((uint32_t)t1 << 16) | ((uint32_t)o1 << 24);
}

static void scenario_base(void)
{
    memset(&s_rcc, 0, sizeof(s_rcc));
    s_rcc.online = s_rcc.device_valid = s_rcc.status_valid = s_rcc.measurement_valid = s_rcc.faults_valid = true;
    s_rcc.reply_age_ms = s_rcc.status_age_ms = s_rcc.measurement_age_ms = 400;
    s_rcc.faults_age_ms = 800;
    s_rcc.command_age_ms = UINT32_MAX;
    s_rcc.product_id = 1;
    s_rcc.protocol_version = 1;
    s_rcc.hardware_revision = 0x01;
    s_rcc.firmware_revision = 0x07;
    memcpy(s_rcc.image_id, "\x9F\x3A\x11\x22\x33\x44\xE8\x15", 8);
    s_rcc.boot_id = 0x1234E4B2u;
    s_rcc.build_flags = 7;        /* provisional, relay enable, operational node */
    s_rcc.running_links = 0xF;    /* CAN, RS485, UART, SVC */
    s_rcc.vout_status = s_rcc.iout_status = 0x3F;
    s_rcc.measurement_sample_age_ms = 12;
    s_rcc.timeouts = 2;
}

/* 0: charging session #17. 1: bench INHIBITED with ADC faults (Faults preview). */
void mock_set_scenario(int scenario)
{
    scenario_base();
    if (scenario == 1) {
        s_rcc.top_state = 5;
        s_rcc.inhibit_mask = 4;              /* RECOVERY */
        s_rcc.primary_fault = 0x0302;        /* ADC_STALE */
        s_rcc.active_fault_mask = 0x2A0;
        s_rcc.vout_mv = 3400;
        s_rcc.vout_status = 0x7F;
        s_rcc.iout_status = 0x0D;
        s_rcc.measurement_sequence = 1032;
        return;
    }
    s_rcc.top_state = 4;
    s_rcc.operational_state = 6;
    s_rcc.charging_established = true;
    s_rcc.session_id = 17;
    s_rcc.relay_command = 1;
    s_rcc.vout_mv = 48200;
    s_rcc.iout_ma = 8000;
    s_rcc.vout_filtered_mv = 48100;
    s_rcc.iout_filtered_ma = 8000;
    s_rcc.measurement_sequence = 48213;
    s_rcc.last_command = 0x20;
    s_rcc.command_state = RCC_COMMAND_COMPLETED;
    s_rcc.command_age_ms = 860000;
    s_rcc.event_missed = 3;
    add_event(0x0001, 2, 1, 0, 0);                         /* BOOT_COMPLETED, POWER_ON */
    add_event(0x0011, 1305, 1, 0, 0);                      /* INHIBIT_CHANGED REMOTE -> none */
    add_event(0x0013, 1306, 17, 0, 0);                     /* SESSION_ARMED */
    add_event(0x0010, 1311, state_pair(4, 4, 4, 5), 0, 0); /* RELAY_CLOSING -> CHARGE_VERIFY */
    add_event(0x0014, 1316, 17, 48200, 8000);              /* CHARGE_ESTABLISHED */
    add_event(0x0010, 1317, state_pair(4, 5, 4, 6), 0, 0); /* CHARGE_VERIFY -> CHARGING */
}

void rcc_link_snapshot(rcc_snapshot_t *out)
{
    if (out) *out = s_rcc;
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
void hmi_wifi_scan(void) {}   /* keep the connected list for the preview */
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
    .running_version = "snapshot",
    .version = 1,
};

esp_err_t hmi_ota_init(void) { return ESP_OK; }
void hmi_ota_arm(void) { s_mock_ota.armed = true; snprintf(s_mock_ota.pin, sizeof(s_mock_ota.pin), "482913"); s_mock_ota.arm_seq++; s_mock_ota.version++; }
void hmi_ota_disarm(void) { s_mock_ota.armed = false; s_mock_ota.pin[0] = '\0'; s_mock_ota.arm_seq++; s_mock_ota.version++; }
esp_err_t hmi_ota_start_server(void) { return ESP_OK; }
void hmi_ota_stop_server(void) {}
bool hmi_ota_server_is_running(void) { return true; }
void hmi_ota_get_status(hmi_ota_status_t *out) { if (out) *out = s_mock_ota; }
esp_err_t hmi_ota_mark_valid(void) { return ESP_OK; }


