/* Stand-ins for the ESP-only settings / display-power modules so the UI renders on the PC. */
#include "hmi_power.h"
#include "hmi_settings.h"
static hmi_settings_t cfg = {.brightness_pct = 100, .sleep_timeout_s = 180, .dim_pct = 15, .language = 0};
esp_err_t hmi_settings_init(void) { return ESP_OK; }
const hmi_settings_t *hmi_settings_get(void) { return &cfg; }
esp_err_t hmi_settings_save(const hmi_settings_t *c) { cfg = *c; return ESP_OK; }
esp_err_t hmi_settings_set_brightness(uint8_t p) { cfg.brightness_pct = p; return ESP_OK; }
esp_err_t hmi_settings_set_sleep_timeout(uint16_t s) { cfg.sleep_timeout_s = s; return ESP_OK; }
esp_err_t hmi_settings_set_dim_pct(uint8_t p) { cfg.dim_pct = p; return ESP_OK; }
esp_err_t hmi_settings_set_language(uint8_t l) { cfg.language = l; return ESP_OK; }
void hmi_power_start(void) {}
bool hmi_power_on_touch(void) { return false; }
void hmi_power_set_brightness(uint8_t p) { cfg.brightness_pct = p; }
void hmi_power_tick(bool i) { (void)i; }
#include <string.h>
#include "hmi_ota.h"
void hmi_ota_arm(void) {}
void hmi_ota_disarm(void) {}
static hmi_ota_state_t sim_ota = HMI_OTA_IDLE;
void sim_ota_state(const char *name)   /* ota=receiving|verifying|success: show the OTA progress overlay */
{
    sim_ota = !strcmp(name, "receiving") ? HMI_OTA_RECEIVING : !strcmp(name, "verifying") ? HMI_OTA_VERIFYING :
              !strcmp(name, "success") ? HMI_OTA_SUCCESS : HMI_OTA_IDLE;
}
void hmi_ota_get_status(hmi_ota_status_t *out)
{
    memset(out, 0, sizeof(*out));
    out->state = sim_ota;
    out->progress_pct = sim_ota == HMI_OTA_RECEIVING ? 42 : sim_ota == HMI_OTA_IDLE ? 0 : 100;
    out->bytes_written = sim_ota == HMI_OTA_RECEIVING ? 430080 : 1024000;
    out->total_bytes = 1024000;
}
