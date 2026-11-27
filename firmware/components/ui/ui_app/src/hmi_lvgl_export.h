#pragma once

#include "hmi_lvgl_headers.h"
#include <stdbool.h>
#include <string.h>

#include "lvgl.h"

/* The Editor exports all design previews in lvgl_ui.h. Firmware components
 * use only the generated layout and receive the production assets as inputs. */
#define LVGL_PRO_LVGL_UI_H
#define LVGL_UI_CHECK_COMPILE_TARGET(target) 1
#define lvgl_ui_check_target(target) true

/* Automatic instance names serve the Editor. Keep only explicit binding IDs on device. */
static inline void hmi_layout_set_name(lv_obj_t *obj, const char *name)
{
    if (name != NULL && strchr(name, '#') == NULL)
        lv_obj_set_name_static(obj, name);
}

#define lv_obj_set_name(obj, name) hmi_layout_set_name((obj), (name))
#define lv_obj_set_name_static(obj, name) hmi_layout_set_name((obj), (name))
