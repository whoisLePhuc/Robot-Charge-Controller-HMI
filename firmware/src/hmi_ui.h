#pragma once

#include <stdbool.h>

/* Call under the LVGL port lock, after board initialization. */
void hmi_ui_start(bool link_started);
