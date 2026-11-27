/**
 * @file rcc_device_banner_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "rcc_device_banner_gen.h"
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

lv_obj_t * rcc_device_banner_create(lv_obj_t * parent, lv_font_t * caption_font, const void * warning_icon)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "rcc_device_banner_#");
        lv_obj_set_x(lv_obj_0, 0);
        lv_obj_set_y(lv_obj_0, 0);
        lv_obj_set_width(lv_obj_0, 408);
        lv_obj_set_height(lv_obj_0, 24);
        lv_obj_set_clickable(lv_obj_0, false);
        lv_obj_set_scrollable(lv_obj_0, false);
        lv_obj_set_style_bg_color(lv_obj_0, lv_color_hex(0x2b2309), 0);
        lv_obj_set_style_bg_opa(lv_obj_0, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(lv_obj_0, 8, 0);
        lv_obj_set_style_border_width(lv_obj_0, 1, 0);
        lv_obj_set_style_outline_width(lv_obj_0, 0, 0);
        lv_obj_set_style_shadow_width(lv_obj_0, 0, 0);
        lv_obj_set_style_pad_all(lv_obj_0, -1, 0);
        lv_obj_set_style_border_color(lv_obj_0, lv_color_hex(0xfacc15), 0);
        lv_obj_set_style_border_opa(lv_obj_0, (255 * 100 / 100), 0);

        lv_obj_t * device_banner_icon = lv_image_create(lv_obj_0);
        lv_obj_set_x(device_banner_icon, 11);
        lv_obj_set_y(device_banner_icon, 5);
        lv_image_set_src(device_banner_icon, warning_icon);
        lv_obj_set_style_image_recolor(device_banner_icon, lv_color_hex(0xfacc15), 0);
        lv_obj_set_style_image_recolor_opa(device_banner_icon, (255 * 100 / 100), 0);
        lv_obj_set_clickable(device_banner_icon, false);
        lv_obj_set_scrollable(device_banner_icon, false);
        lv_obj_set_name(device_banner_icon, "device_banner_icon");

        lv_obj_t * device_banner_title = lv_label_create(lv_obj_0);
        lv_obj_set_x(device_banner_title, 32);
        lv_obj_set_y(device_banner_title, 5);
        lv_label_set_text(device_banner_title, "BENCH IMAGE — NOT A PRODUCT BUILD");
        lv_obj_set_style_text_font(device_banner_title, caption_font, 0);
        lv_obj_set_style_text_color(device_banner_title, lv_color_hex(0xfacc15), 0);
        lv_obj_set_clickable(device_banner_title, false);
        lv_obj_set_scrollable(device_banner_title, false);
        lv_obj_set_width(device_banner_title, 250);
        lv_obj_set_height(device_banner_title, 13);
        lv_label_set_long_mode(device_banner_title, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(device_banner_title, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_name(device_banner_title, "device_banner_title");

        lv_obj_t * device_banner_status = lv_label_create(lv_obj_0);
        lv_obj_set_x(device_banner_status, 280);
        lv_obj_set_y(device_banner_status, 5);
        lv_label_set_text(device_banner_status, "");
        lv_obj_set_style_text_font(device_banner_status, caption_font, 0);
        lv_obj_set_style_text_color(device_banner_status, lv_color_hex(0xcbd5e1), 0);
        lv_obj_set_clickable(device_banner_status, false);
        lv_obj_set_scrollable(device_banner_status, false);
        lv_obj_set_width(device_banner_status, 115);
        lv_obj_set_height(device_banner_status, 13);
        lv_label_set_long_mode(device_banner_status, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(device_banner_status, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_set_name(device_banner_status, "device_banner_status");

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

