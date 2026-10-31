/*
 * Public HMI entry points (hmi_ui.h). All the work is in ui/: shell (chrome, navigation, refresh), registry pages,
 * Wi-Fi flow. docs/mockups/hmi-ui-landscape.html is a browser port of this UI; keep the two in step.
 */
#include "hmi_ui.h"

#include "ui/hmi_shell.h"
#include "ui/hmi_wifi_ui.h"

void hmi_ui_start(bool link_started) { hmi_shell_start(link_started); }
void hmi_ui_select_page(int page_idx, int sub_tab) { hmi_shell_select_page(page_idx, sub_tab); }
void hmi_ui_open_wifi(void) { hmi_wifi_ui_open(); }
#ifdef HMI_SIM
void hmi_ui_wifi_inject(const char *text) { hmi_wifi_ui_inject(text); }
#endif
