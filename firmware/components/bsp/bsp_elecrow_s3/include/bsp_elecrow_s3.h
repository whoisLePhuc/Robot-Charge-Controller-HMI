/*
 * Board support for the Elecrow DLE06235B / ES3C35P ESP32-S3 display: ST77922 QSPI panel, LVGL port,
 * capacitive touch and the LEDC backlight. Drivers only: no settings, no application policy.
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Backlight on at its floor (bsp_backlight_init(0)), ST77922 panel, LVGL port, display rotated to
 * 480x320 landscape. *out_display receives the LVGL display. Touch is started separately. */
esp_err_t bsp_display_start(lv_display_t **out_display);

/* Called from the LVGL task on every valid touch, before the point is reported to LVGL.
 * Return true to swallow the touch (for example, it only woke a dimmed screen). */
typedef bool (*bsp_touch_activity_cb_t)(void);

/* Touch controller (integrated ST77922 at 0x55 or FT6x36-compatible at 0x38) as an LVGL pointer.
 * Call with the LVGL port lock held. on_activity may be NULL. */
esp_err_t bsp_touch_start(lv_display_t *display, bsp_touch_activity_cb_t on_activity);

/* LEDC PWM backlight (5 kHz, 8 bit). initial_pct is clamped to 10..100. */
esp_err_t bsp_backlight_init(uint8_t initial_pct);

/* Backlight duty 0..100 %, optionally with a 200 ms fade. */
void bsp_backlight_set_duty(uint8_t pct, bool fade);

#ifdef __cplusplus
}
#endif
