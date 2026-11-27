/**
 * @file rcc_shell_frame_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "rcc_shell_frame_gen.h"
#include "../../../lvgl_ui.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/***********************
 *  STATIC VARIABLES
 **********************/

/***********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * rcc_shell_frame_create(lv_obj_t * parent)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "rcc_shell_frame_#");
        lv_obj_set_x(lv_obj_0, 0);
        lv_obj_set_y(lv_obj_0, 0);
        lv_obj_set_width(lv_obj_0, 480);
        lv_obj_set_height(lv_obj_0, 320);
        lv_obj_set_scrollable(lv_obj_0, false);
        lv_obj_set_clickable(lv_obj_0, false);
        lv_obj_set_style_pad_all(lv_obj_0, 0, 0);
        lv_obj_set_style_bg_color(lv_obj_0, lv_color_hex(0x090d14), 0);
        lv_obj_set_style_bg_opa(lv_obj_0, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(lv_obj_0, 0, 0);
        lv_obj_set_style_border_width(lv_obj_0, 0, 0);
        lv_obj_set_style_outline_width(lv_obj_0, 0, 0);
        lv_obj_set_style_shadow_width(lv_obj_0, 0, 0);

        lv_obj_t * page_host = lv_obj_create(lv_obj_0);
        lv_obj_set_x(page_host, 0);
        lv_obj_set_y(page_host, 0);
        lv_obj_set_width(page_host, 480);
        lv_obj_set_height(page_host, 320);
        lv_obj_set_scrollable(page_host, false);
        lv_obj_set_clickable(page_host, false);
        lv_obj_set_style_pad_all(page_host, 0, 0);
        lv_obj_set_style_bg_color(page_host, lv_color_hex(0x090d14), 0);
        lv_obj_set_style_bg_opa(page_host, (255 * 0 / 100), 0);
        lv_obj_set_style_radius(page_host, 0, 0);
        lv_obj_set_style_border_width(page_host, 0, 0);
        lv_obj_set_style_outline_width(page_host, 0, 0);
        lv_obj_set_style_shadow_width(page_host, 0, 0);
        lv_obj_set_name(page_host, "page_host");

        lv_obj_t * action_host = lv_obj_create(lv_obj_0);
        lv_obj_set_x(action_host, 64);
        lv_obj_set_y(action_host, 270);
        lv_obj_set_width(action_host, 408);
        lv_obj_set_height(action_host, 42);
        lv_obj_set_scrollable(action_host, false);
        lv_obj_set_clickable(action_host, false);
        lv_obj_set_style_pad_all(action_host, 0, 0);
        lv_obj_set_style_bg_color(action_host, lv_color_hex(0x090d14), 0);
        lv_obj_set_style_bg_opa(action_host, (255 * 0 / 100), 0);
        lv_obj_set_style_radius(action_host, 0, 0);
        lv_obj_set_style_border_width(action_host, 0, 0);
        lv_obj_set_style_outline_width(action_host, 0, 0);
        lv_obj_set_style_shadow_width(action_host, 0, 0);
        lv_obj_set_name(action_host, "action_host");

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

