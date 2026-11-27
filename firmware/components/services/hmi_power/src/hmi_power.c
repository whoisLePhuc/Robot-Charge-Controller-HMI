/* Display power policy over the BSP backlight driver (bsp_backlight_set_duty). */
#include "hmi_power.h"

#include "bsp_elecrow_s3.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "hmi_settings.h"

static const char *TAG = "hmi_power";

static int64_t s_last_activity_us = 0;
static bool s_is_dimmed = false;

static void notify_activity(void);

void hmi_power_start(void)
{
    s_last_activity_us = esp_timer_get_time();
    s_is_dimmed = false;
    bsp_backlight_set_duty(hmi_settings_get()->brightness_pct, true);
}

bool hmi_power_on_touch(void)
{
    const bool was_dimmed = s_is_dimmed;
    notify_activity();
    return was_dimmed;
}

void hmi_power_set_brightness(uint8_t pct)
{
    (void)hmi_settings_set_brightness(pct);
    bsp_backlight_set_duty(pct, true);
}

static void notify_activity(void)
{
    s_last_activity_us = esp_timer_get_time();
    if (s_is_dimmed) {
        s_is_dimmed = false;
        const hmi_settings_t *cfg = hmi_settings_get();
        bsp_backlight_set_duty(cfg->brightness_pct, true);
        ESP_LOGI(TAG, "Waking up backlight to %u%%", cfg->brightness_pct);
    }
}

void hmi_power_tick(bool is_idle)
{
    const hmi_settings_t *cfg = hmi_settings_get();
    if (cfg->sleep_timeout_s == 0) {
        if (s_is_dimmed) {
            notify_activity();
        }
        return;
    }

    if (!is_idle) {
        /* Not idle (active charging or alert); ensure display is bright */
        if (s_is_dimmed) {
            notify_activity();
        }
        return;
    }

    if (!s_is_dimmed) {
        const int64_t now = esp_timer_get_time();
        const int64_t timeout_us = (int64_t)cfg->sleep_timeout_s * 1000000LL;
        if (now - s_last_activity_us >= timeout_us) {
            s_is_dimmed = true;
            bsp_backlight_set_duty(cfg->dim_pct, true);
            ESP_LOGI(TAG, "Sleep timeout reached (%u s): backlight dimmed to %u%%",
                     cfg->sleep_timeout_s, cfg->dim_pct);
        }
    }
}
