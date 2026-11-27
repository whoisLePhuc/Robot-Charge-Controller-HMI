#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Dim/sleep policy over the BSP backlight driver. Start the activity timer and fade to the stored
 * brightness; call once the UI is up (the BSP brings the backlight up at its floor). */
void hmi_backlight_start(void);

/* Touch hook (bsp_touch_activity_cb_t): records activity; true if the touch only woke the screen. */
bool hmi_backlight_on_touch(void);

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
