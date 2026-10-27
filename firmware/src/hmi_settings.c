#include "hmi_settings.h"

#include <string.h>
#include "esp_check.h"
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"

static const char *TAG = "hmi_settings";
#define HMI_NVS_NAMESPACE "hmi_cfg"
#define HMI_NVS_KEY       "settings"

static hmi_settings_t s_settings = {
    .brightness_pct = 100,
    .sleep_timeout_s = 180,
    .dim_pct = 15,
    .language = HMI_LANG_EN,
};

static void sanitize_settings(hmi_settings_t *s)
{
    if (s->brightness_pct < 10 || s->brightness_pct > 100) {
        s->brightness_pct = 100;
    }
    if (s->dim_pct > 50) {
        s->dim_pct = 15;
    }
    if (s->sleep_timeout_s > 3600) {
        s->sleep_timeout_s = 180;
    }
    if (s->language > HMI_LANG_VI) {
        s->language = HMI_LANG_EN;
    }
}

esp_err_t hmi_settings_init(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS flash partition truncated/version mismatch, erasing...");
        ESP_RETURN_ON_ERROR(nvs_flash_erase(), TAG, "nvs_flash_erase failed");
        err = nvs_flash_init();
    }
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "nvs_flash_init failed: %s", esp_err_to_name(err));
        return err;
    }

    nvs_handle_t handle;
    err = nvs_open(HMI_NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "nvs_open failed: %s", esp_err_to_name(err));
        return err;
    }

    size_t size = sizeof(s_settings);
    hmi_settings_t loaded;
    err = nvs_get_blob(handle, HMI_NVS_KEY, &loaded, &size);
    if (err == ESP_OK && size == sizeof(s_settings)) {
        sanitize_settings(&loaded);
        s_settings = loaded;
        ESP_LOGI(TAG, "Loaded settings: brightness=%u%%, sleep=%us, dim=%u%%, lang=%u",
                 s_settings.brightness_pct, s_settings.sleep_timeout_s,
                 s_settings.dim_pct, s_settings.language);
    } else {
        ESP_LOGI(TAG, "No valid settings in NVS, writing defaults...");
        err = nvs_set_blob(handle, HMI_NVS_KEY, &s_settings, sizeof(s_settings));
        if (err == ESP_OK) {
            err = nvs_commit(handle);
        }
    }

    nvs_close(handle);
    return ESP_OK;
}

const hmi_settings_t *hmi_settings_get(void)
{
    return &s_settings;
}

esp_err_t hmi_settings_save(const hmi_settings_t *cfg)
{
    if (cfg == NULL) return ESP_ERR_INVALID_ARG;
    hmi_settings_t temp = *cfg;
    sanitize_settings(&temp);
    s_settings = temp;

    nvs_handle_t handle;
    esp_err_t err = nvs_open(HMI_NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) return err;

    err = nvs_set_blob(handle, HMI_NVS_KEY, &s_settings, sizeof(s_settings));
    if (err == ESP_OK) {
        err = nvs_commit(handle);
    }
    nvs_close(handle);
    ESP_LOGI(TAG, "Saved settings: brightness=%u%%, sleep=%us, dim=%u%%, lang=%u (result %s)",
             s_settings.brightness_pct, s_settings.sleep_timeout_s,
             s_settings.dim_pct, s_settings.language, esp_err_to_name(err));
    return err;
}

esp_err_t hmi_settings_set_brightness(uint8_t pct)
{
    hmi_settings_t s = s_settings;
    s.brightness_pct = pct;
    return hmi_settings_save(&s);
}

esp_err_t hmi_settings_set_sleep_timeout(uint16_t seconds)
{
    hmi_settings_t s = s_settings;
    s.sleep_timeout_s = seconds;
    return hmi_settings_save(&s);
}

esp_err_t hmi_settings_set_dim_pct(uint8_t pct)
{
    hmi_settings_t s = s_settings;
    s.dim_pct = pct;
    return hmi_settings_save(&s);
}

esp_err_t hmi_settings_set_language(uint8_t lang)
{
    hmi_settings_t s = s_settings;
    s.language = lang;
    return hmi_settings_save(&s);
}
