/**
 * @file rcc_shell_ota_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "rcc_shell_ota_gen.h"
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

lv_obj_t * rcc_shell_ota_create(lv_obj_t * parent, lv_font_t * title_font, lv_font_t * body_font, lv_font_t * warning_font, lv_font_t * progress_font)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "rcc_shell_ota_#");
        lv_obj_set_x(lv_obj_0, 30);
        lv_obj_set_y(lv_obj_0, 50);
        lv_obj_set_width(lv_obj_0, 420);
        lv_obj_set_height(lv_obj_0, 220);
        lv_obj_set_scrollable(lv_obj_0, false);
        lv_obj_set_clickable(lv_obj_0, false);
        lv_obj_set_style_pad_all(lv_obj_0, 0, 0);
        lv_obj_set_style_bg_color(lv_obj_0, lv_color_hex(0x070d19), 0);
        lv_obj_set_style_bg_opa(lv_obj_0, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(lv_obj_0, 14, 0);
        lv_obj_set_style_border_width(lv_obj_0, 2, 0);
        lv_obj_set_style_outline_width(lv_obj_0, 0, 0);
        lv_obj_set_style_shadow_width(lv_obj_0, 0, 0);
        lv_obj_set_style_border_color(lv_obj_0, lv_color_hex(0x0284c7), 0);

        lv_obj_t * lv_label_0 = lv_label_create(lv_obj_0);
        lv_obj_set_x(lv_label_0, 20);
        lv_obj_set_y(lv_label_0, 22);
        lv_label_set_text(lv_label_0, "FIRMWARE UPDATE IN PROGRESS");
        lv_obj_set_scrollable(lv_label_0, false);
        lv_obj_set_clickable(lv_label_0, false);
        lv_obj_set_style_text_font(lv_label_0, title_font, 0);
        lv_obj_set_style_text_color(lv_label_0, lv_color_hex(0x38bdf8), 0);
        lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_letter_space(lv_label_0, 0, 0);
        lv_obj_set_width(lv_label_0, 380);
        lv_obj_set_height(lv_label_0, 17);
        lv_label_set_long_mode(lv_label_0, LV_LABEL_LONG_MODE_DOTS);

        lv_obj_t * lv_label_1 = lv_label_create(lv_obj_0);
        lv_obj_set_x(lv_label_1, 20);
        lv_obj_set_y(lv_label_1, 50);
        lv_label_set_text(lv_label_1, "Target slot: writing to flash partition");
        lv_obj_set_scrollable(lv_label_1, false);
        lv_obj_set_clickable(lv_label_1, false);
        lv_obj_set_style_text_font(lv_label_1, body_font, 0);
        lv_obj_set_style_text_color(lv_label_1, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_style_text_align(lv_label_1, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_letter_space(lv_label_1, 0, 0);
        lv_obj_set_width(lv_label_1, 380);
        lv_obj_set_height(lv_label_1, 15);
        lv_label_set_long_mode(lv_label_1, LV_LABEL_LONG_MODE_DOTS);

        lv_obj_t * lv_label_2 = lv_label_create(lv_obj_0);
        lv_obj_set_x(lv_label_2, 20);
        lv_obj_set_y(lv_label_2, 77);
        lv_label_set_text(lv_label_2, "DO NOT DISCONNECT POWER");
        lv_obj_set_scrollable(lv_label_2, false);
        lv_obj_set_clickable(lv_label_2, false);
        lv_obj_set_style_text_font(lv_label_2, warning_font, 0);
        lv_obj_set_style_text_color(lv_label_2, lv_color_hex(0xf59e0b), 0);
        lv_obj_set_style_text_align(lv_label_2, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_letter_space(lv_label_2, 0, 0);
        lv_obj_set_width(lv_label_2, 380);
        lv_obj_set_height(lv_label_2, 13);
        lv_label_set_long_mode(lv_label_2, LV_LABEL_LONG_MODE_DOTS);

        lv_obj_t * ota_track = lv_obj_create(lv_obj_0);
        lv_obj_set_x(ota_track, 30);
        lv_obj_set_y(ota_track, 115);
        lv_obj_set_width(ota_track, 360);
        lv_obj_set_height(ota_track, 20);
        lv_obj_set_scrollable(ota_track, false);
        lv_obj_set_clickable(ota_track, false);
        lv_obj_set_style_pad_all(ota_track, 0, 0);
        lv_obj_set_style_bg_color(ota_track, lv_color_hex(0x1e293b), 0);
        lv_obj_set_style_bg_opa(ota_track, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(ota_track, 8, 0);
        lv_obj_set_style_border_width(ota_track, 0, 0);
        lv_obj_set_style_outline_width(ota_track, 0, 0);
        lv_obj_set_style_shadow_width(ota_track, 0, 0);
        lv_obj_set_name(ota_track, "ota_track");
        lv_obj_t * ota_fill = lv_obj_create(ota_track);
        lv_obj_set_x(ota_fill, 0);
        lv_obj_set_y(ota_fill, 0);
        lv_obj_set_width(ota_fill, 4);
        lv_obj_set_height(ota_fill, 20);
        lv_obj_set_scrollable(ota_fill, false);
        lv_obj_set_clickable(ota_fill, false);
        lv_obj_set_style_pad_all(ota_fill, 0, 0);
        lv_obj_set_style_bg_color(ota_fill, lv_color_hex(0x38bdf8), 0);
        lv_obj_set_style_bg_opa(ota_fill, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(ota_fill, 8, 0);
        lv_obj_set_style_border_width(ota_fill, 0, 0);
        lv_obj_set_style_outline_width(ota_fill, 0, 0);
        lv_obj_set_style_shadow_width(ota_fill, 0, 0);
        lv_obj_set_name(ota_fill, "ota_fill");

        lv_obj_t * ota_status = lv_label_create(lv_obj_0);
        lv_obj_set_x(ota_status, 20);
        lv_obj_set_y(ota_status, 152);
        lv_label_set_text(ota_status, "Writing: 0%");
        lv_obj_set_scrollable(ota_status, false);
        lv_obj_set_clickable(ota_status, false);
        lv_obj_set_style_text_font(ota_status, progress_font, 0);
        lv_obj_set_style_text_color(ota_status, lv_color_hex(0xffffff), 0);
        lv_obj_set_style_text_align(ota_status, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_letter_space(ota_status, 0, 0);
        lv_obj_set_width(ota_status, 380);
        lv_obj_set_height(ota_status, 16);
        lv_label_set_long_mode(ota_status, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_name(ota_status, "ota_status");

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

