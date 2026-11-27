/* LEDC PWM backlight driver. The dim/sleep policy that decides the duty is hmi_backlight (services). */
#include "bsp_elecrow_s3.h"

#include <inttypes.h>
#include "driver/ledc.h"
#include "esp_log.h"
#include "bsp_pins.h"

static const char *TAG = "bsp_backlight";

#define BACKLIGHT_LEDC_SPEED LEDC_LOW_SPEED_MODE
#define BACKLIGHT_LEDC_TIMER LEDC_TIMER_0
#define BACKLIGHT_LEDC_CH    LEDC_CHANNEL_0
#define BACKLIGHT_PWM_FREQ   5000 /* 5 kHz */

esp_err_t bsp_backlight_init(uint8_t initial_pct)
{
    if (initial_pct > 100) initial_pct = 100;
    if (initial_pct < 10) initial_pct = 10;

    const ledc_timer_config_t timer_conf = {
        .speed_mode = BACKLIGHT_LEDC_SPEED,
        .duty_resolution = LEDC_TIMER_8_BIT,
        .timer_num = BACKLIGHT_LEDC_TIMER,
        .freq_hz = BACKLIGHT_PWM_FREQ,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    esp_err_t err = ledc_timer_config(&timer_conf);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "ledc_timer_config failed: %s", esp_err_to_name(err));
        return err;
    }

    const uint32_t duty = ((uint32_t)initial_pct * 255u) / 100u;
    const ledc_channel_config_t ch_conf = {
        .gpio_num = BSP_BACKLIGHT_GPIO,
        .speed_mode = BACKLIGHT_LEDC_SPEED,
        .channel = BACKLIGHT_LEDC_CH,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = BACKLIGHT_LEDC_TIMER,
        .duty = duty,
        .hpoint = 0,
        .flags = {
            .output_invert = 0,
        },
    };
    err = ledc_channel_config(&ch_conf);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "ledc_channel_config failed: %s", esp_err_to_name(err));
        return err;
    }

    /* Install fade interrupt service */
    err = ledc_fade_func_install(0);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        ESP_LOGW(TAG, "ledc_fade_func_install: %s", esp_err_to_name(err));
    }

    ESP_LOGI(TAG, "LEDC PWM Backlight initialized on GPIO %d: %u%% (duty %" PRIu32 "/255, %d Hz)",
             BSP_BACKLIGHT_GPIO, initial_pct, duty, BACKLIGHT_PWM_FREQ);
    return ESP_OK;
}

void bsp_backlight_set_duty(uint8_t pct, bool fade)
{
    if (pct > 100) pct = 100;
    const uint32_t duty = ((uint32_t)pct * 255u) / 100u;

    if (fade) {
        ledc_set_fade_with_time(BACKLIGHT_LEDC_SPEED, BACKLIGHT_LEDC_CH, duty, 200);
        ledc_fade_start(BACKLIGHT_LEDC_SPEED, BACKLIGHT_LEDC_CH, LEDC_FADE_NO_WAIT);
    } else {
        ledc_set_duty(BACKLIGHT_LEDC_SPEED, BACKLIGHT_LEDC_CH, duty);
        ledc_update_duty(BACKLIGHT_LEDC_SPEED, BACKLIGHT_LEDC_CH);
    }
}
