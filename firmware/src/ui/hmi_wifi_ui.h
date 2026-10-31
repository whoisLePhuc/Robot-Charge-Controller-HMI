/*
 * Wi-Fi flow overlay (network list, password keyboard, connecting, info, failure). LVGL task only.
 */
#pragma once

#include <stdbool.h>

/* Open the full-screen overlay (no-op while already open). */
void hmi_wifi_ui_open(void);
#ifdef HMI_SIM
void hmi_wifi_ui_inject(const char *text);   /* append characters to the password field */
#endif
