#pragma once

#include <stdbool.h>

/* Call under the LVGL port lock, after board initialization. */
void hmi_ui_start(bool link_started);

/* Select page (0:OVERVIEW, 1:MEASURE, 2:FAULTS, 3:SESSION, 4:DEVICE) and sub-tab */
void hmi_ui_select_page(int page_idx, int sub_tab);

/* Open the full-screen Wi-Fi flow (also reachable from Device > Settings > Wi-Fi). */
void hmi_ui_open_wifi(void);
#ifdef HMI_SIM
void hmi_ui_wifi_inject(const char *text); /* simulator: append characters to the password field */
#endif
