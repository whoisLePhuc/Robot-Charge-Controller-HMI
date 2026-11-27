/*
 * Display power policy: user brightness, dim after the sleep timeout while the controller is idle,
 * wake on touch. Drives the BSP backlight; brightness, timeout and dim level come from hmi_settings.
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Start the idle timer and fade to the stored brightness. Call once the UI is up
 * (the BSP brings the backlight up at its floor). */
void hmi_power_start(void);

/* Touch hook (bsp_touch_activity_cb_t): records activity and wakes a dimmed screen.
 * Returns true if the touch only woke the screen and must not reach the UI. */
bool hmi_power_on_touch(void);

/* User-chosen brightness: persist it in the settings and apply it now. */
void hmi_power_set_brightness(uint8_t pct);

/* Periodic tick (every 500-1000 ms) that applies the sleep timeout while the controller is idle. */
void hmi_power_tick(bool is_idle);

#ifdef __cplusplus
}
#endif
