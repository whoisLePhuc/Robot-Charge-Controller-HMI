#pragma once

#include "esp_err.h"
#include "lvgl.h"

/* Call with the LVGL port lock held. Native coordinates are rotated by LVGL. */
esp_err_t elecrow_s3_touch_start(lv_display_t *display);
