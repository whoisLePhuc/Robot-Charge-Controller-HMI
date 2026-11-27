#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HMI_BACKLIGHT_GPIO 41

/* Initialize LEDC PWM channel and fade service for backlight on GPIO 41 */
esp_err_t hmi_backlight_init(uint8_t initial_pct);

/* Set backlight target duty percentage (0..100) with optional smooth fading */
void hmi_backlight_set_duty(uint8_t pct, bool fade);

/* Notify user interaction (touch) to reset sleep timer and wake screen if dimmed */
void hmi_backlight_notify_activity(void);

/* User-chosen brightness: persist it in the settings and apply it now. */
void hmi_backlight_set_brightness(uint8_t pct);

/* Check if screen is currently dimmed due to inactivity */
bool hmi_backlight_is_dimmed(void);

/* Periodic tick to evaluate sleep timeout when controller is idle. Call every 500-1000ms */
void hmi_backlight_tick(bool is_idle);

#ifdef __cplusplus
}
#endif
