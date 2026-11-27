/**
 * @file rcc_wifi_busy_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "rcc_wifi_busy_gen.h"
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

lv_obj_t * rcc_wifi_busy_create(lv_obj_t * parent, lv_font_t * title_font, lv_font_t * caption_font, lv_font_t * data_font, lv_font_t * active_font, const void * spinner_large, const void * spinner_small, const void * check_icon)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "rcc_wifi_busy_#");
        lv_obj_set_width(lv_obj_0, 480);
        lv_obj_set_height(lv_obj_0, 320);
        lv_obj_set_clickable(lv_obj_0, false);
        lv_obj_set_scrollable(lv_obj_0, false);
        lv_obj_set_style_bg_opa(lv_obj_0, (255 * 0 / 100), 0);
        lv_obj_set_style_pad_all(lv_obj_0, 0, 0);
        lv_obj_set_style_border_width(lv_obj_0, 0, 0);
        lv_obj_set_style_outline_width(lv_obj_0, 0, 0);
        lv_obj_set_style_shadow_width(lv_obj_0, 0, 0);
        lv_obj_set_style_radius(lv_obj_0, 0, 0);

        lv_obj_t * spinner = lv_image_create(lv_obj_0);
        lv_obj_set_name(spinner, "spinner");
        lv_image_set_src(spinner, spinner_large);
        lv_obj_set_clickable(spinner, false);
        lv_obj_set_scrollable(spinner, false);
        lv_obj_set_x(spinner, 218);
        lv_obj_set_y(spinner, 50);
        lv_obj_set_style_image_recolor(spinner, lv_color_hex(0x38bdf8), 0);
        lv_obj_set_style_image_recolor_opa(spinner, (255 * 100 / 100), 0);

        lv_obj_t * lv_label_0 = lv_label_create(lv_obj_0);
        lv_label_set_text(lv_label_0, "Joining network...");
        lv_obj_set_clickable(lv_label_0, false);
        lv_obj_set_scrollable(lv_label_0, false);
        lv_obj_set_x(lv_label_0, 0);
        lv_obj_set_y(lv_label_0, 104);
        lv_obj_set_width(lv_label_0, 480);
        lv_obj_set_height(lv_label_0, LV_SIZE_CONTENT);
        lv_obj_set_style_text_font(lv_label_0, title_font, 0);
        lv_obj_set_style_text_color(lv_label_0, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * lv_label_1 = lv_label_create(lv_obj_0);
        lv_label_set_text(lv_label_1, "Please wait, usually under 10 s");
        lv_obj_set_clickable(lv_label_1, false);
        lv_obj_set_scrollable(lv_label_1, false);
        lv_obj_set_x(lv_label_1, 0);
        lv_obj_set_y(lv_label_1, 127);
        lv_obj_set_width(lv_label_1, 480);
        lv_obj_set_height(lv_label_1, LV_SIZE_CONTENT);
        lv_obj_set_style_text_font(lv_label_1, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_1, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_style_text_align(lv_label_1, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * step_marker_0 = lv_obj_create(lv_obj_0);
        lv_obj_set_x(step_marker_0, 90);
        lv_obj_set_y(step_marker_0, 168);
        lv_obj_set_width(step_marker_0, 16);
        lv_obj_set_height(step_marker_0, 16);
        lv_obj_set_clickable(step_marker_0, false);
        lv_obj_set_scrollable(step_marker_0, false);
        lv_obj_set_style_pad_all(step_marker_0, 0, 0);
        lv_obj_set_style_outline_width(step_marker_0, 0, 0);
        lv_obj_set_style_shadow_width(step_marker_0, 0, 0);
        lv_obj_set_hidden(step_marker_0, false);
        lv_obj_set_style_bg_color(step_marker_0, lv_color_hex(0x0a2617), 0);
        lv_obj_set_style_bg_opa(step_marker_0, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(step_marker_0, 8, 0);
        lv_obj_set_style_border_width(step_marker_0, 0, 0);
        lv_obj_set_style_border_color(step_marker_0, lv_color_hex(0x2b3648), 0);
        lv_obj_set_name(step_marker_0, "step_marker_0");

        lv_obj_t * step_check_0 = lv_image_create(lv_obj_0);
        lv_image_set_src(step_check_0, check_icon);
        lv_obj_set_clickable(step_check_0, false);
        lv_obj_set_scrollable(step_check_0, false);
        lv_obj_set_x(step_check_0, 93);
        lv_obj_set_y(step_check_0, 171);
        lv_obj_set_hidden(step_check_0, false);
        lv_obj_set_style_image_recolor(step_check_0, lv_color_hex(0x22c55e), 0);
        lv_obj_set_style_image_recolor_opa(step_check_0, (255 * 100 / 100), 0);
        lv_obj_set_name(step_check_0, "step_check_0");

        lv_obj_t * step_spinner_0 = lv_image_create(lv_obj_0);
        lv_obj_set_name(step_spinner_0, "step_spinner_0");
        lv_image_set_src(step_spinner_0, spinner_small);
        lv_obj_set_clickable(step_spinner_0, false);
        lv_obj_set_scrollable(step_spinner_0, false);
        lv_obj_set_x(step_spinner_0, 90);
        lv_obj_set_y(step_spinner_0, 168);
        lv_obj_set_hidden(step_spinner_0, true);
        lv_obj_set_style_image_recolor(step_spinner_0, lv_color_hex(0x38bdf8), 0);
        lv_obj_set_style_image_recolor_opa(step_spinner_0, (255 * 100 / 100), 0);

        lv_obj_t * step_0 = lv_label_create(lv_obj_0);
        lv_obj_set_name(step_0, "step_0");
        lv_label_set_text(step_0, "Associating");
        lv_obj_set_clickable(step_0, false);
        lv_obj_set_scrollable(step_0, false);
        lv_obj_set_x(step_0, 116);
        lv_obj_set_y(step_0, 168);
        lv_obj_set_width(step_0, 270);
        lv_obj_set_height(step_0, LV_SIZE_CONTENT);
        lv_label_set_long_mode(step_0, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_font(step_0, data_font, 0);
        lv_obj_set_style_text_color(step_0, lv_color_hex(0xcbd5e1), 0);

        lv_obj_t * step_marker_1 = lv_obj_create(lv_obj_0);
        lv_obj_set_x(step_marker_1, 90);
        lv_obj_set_y(step_marker_1, 190);
        lv_obj_set_width(step_marker_1, 16);
        lv_obj_set_height(step_marker_1, 16);
        lv_obj_set_clickable(step_marker_1, false);
        lv_obj_set_scrollable(step_marker_1, false);
        lv_obj_set_style_pad_all(step_marker_1, 0, 0);
        lv_obj_set_style_outline_width(step_marker_1, 0, 0);
        lv_obj_set_style_shadow_width(step_marker_1, 0, 0);
        lv_obj_set_hidden(step_marker_1, false);
        lv_obj_set_style_bg_color(step_marker_1, lv_color_hex(0x0a2617), 0);
        lv_obj_set_style_bg_opa(step_marker_1, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(step_marker_1, 8, 0);
        lv_obj_set_style_border_width(step_marker_1, 0, 0);
        lv_obj_set_style_border_color(step_marker_1, lv_color_hex(0x2b3648), 0);
        lv_obj_set_name(step_marker_1, "step_marker_1");

        lv_obj_t * step_check_1 = lv_image_create(lv_obj_0);
        lv_image_set_src(step_check_1, check_icon);
        lv_obj_set_clickable(step_check_1, false);
        lv_obj_set_scrollable(step_check_1, false);
        lv_obj_set_x(step_check_1, 93);
        lv_obj_set_y(step_check_1, 193);
        lv_obj_set_hidden(step_check_1, false);
        lv_obj_set_style_image_recolor(step_check_1, lv_color_hex(0x22c55e), 0);
        lv_obj_set_style_image_recolor_opa(step_check_1, (255 * 100 / 100), 0);
        lv_obj_set_name(step_check_1, "step_check_1");

        lv_obj_t * step_spinner_1 = lv_image_create(lv_obj_0);
        lv_obj_set_name(step_spinner_1, "step_spinner_1");
        lv_image_set_src(step_spinner_1, spinner_small);
        lv_obj_set_clickable(step_spinner_1, false);
        lv_obj_set_scrollable(step_spinner_1, false);
        lv_obj_set_x(step_spinner_1, 90);
        lv_obj_set_y(step_spinner_1, 190);
        lv_obj_set_hidden(step_spinner_1, true);
        lv_obj_set_style_image_recolor(step_spinner_1, lv_color_hex(0x38bdf8), 0);
        lv_obj_set_style_image_recolor_opa(step_spinner_1, (255 * 100 / 100), 0);

        lv_obj_t * step_1 = lv_label_create(lv_obj_0);
        lv_obj_set_name(step_1, "step_1");
        lv_label_set_text(step_1, "Authenticating");
        lv_obj_set_clickable(step_1, false);
        lv_obj_set_scrollable(step_1, false);
        lv_obj_set_x(step_1, 116);
        lv_obj_set_y(step_1, 190);
        lv_obj_set_width(step_1, 270);
        lv_obj_set_height(step_1, LV_SIZE_CONTENT);
        lv_label_set_long_mode(step_1, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_font(step_1, data_font, 0);
        lv_obj_set_style_text_color(step_1, lv_color_hex(0xcbd5e1), 0);

        lv_obj_t * step_marker_2 = lv_obj_create(lv_obj_0);
        lv_obj_set_x(step_marker_2, 90);
        lv_obj_set_y(step_marker_2, 212);
        lv_obj_set_width(step_marker_2, 16);
        lv_obj_set_height(step_marker_2, 16);
        lv_obj_set_clickable(step_marker_2, false);
        lv_obj_set_scrollable(step_marker_2, false);
        lv_obj_set_style_pad_all(step_marker_2, 0, 0);
        lv_obj_set_style_outline_width(step_marker_2, 0, 0);
        lv_obj_set_style_shadow_width(step_marker_2, 0, 0);
        lv_obj_set_hidden(step_marker_2, true);
        lv_obj_set_style_bg_color(step_marker_2, lv_color_hex(0x0a2617), 0);
        lv_obj_set_style_bg_opa(step_marker_2, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(step_marker_2, 8, 0);
        lv_obj_set_style_border_width(step_marker_2, 0, 0);
        lv_obj_set_style_border_color(step_marker_2, lv_color_hex(0x2b3648), 0);
        lv_obj_set_name(step_marker_2, "step_marker_2");

        lv_obj_t * step_check_2 = lv_image_create(lv_obj_0);
        lv_image_set_src(step_check_2, check_icon);
        lv_obj_set_clickable(step_check_2, false);
        lv_obj_set_scrollable(step_check_2, false);
        lv_obj_set_x(step_check_2, 93);
        lv_obj_set_y(step_check_2, 215);
        lv_obj_set_hidden(step_check_2, true);
        lv_obj_set_style_image_recolor(step_check_2, lv_color_hex(0x22c55e), 0);
        lv_obj_set_style_image_recolor_opa(step_check_2, (255 * 100 / 100), 0);
        lv_obj_set_name(step_check_2, "step_check_2");

        lv_obj_t * step_spinner_2 = lv_image_create(lv_obj_0);
        lv_obj_set_name(step_spinner_2, "step_spinner_2");
        lv_image_set_src(step_spinner_2, spinner_small);
        lv_obj_set_clickable(step_spinner_2, false);
        lv_obj_set_scrollable(step_spinner_2, false);
        lv_obj_set_x(step_spinner_2, 90);
        lv_obj_set_y(step_spinner_2, 212);
        lv_obj_set_hidden(step_spinner_2, false);
        lv_obj_set_style_image_recolor(step_spinner_2, lv_color_hex(0x38bdf8), 0);
        lv_obj_set_style_image_recolor_opa(step_spinner_2, (255 * 100 / 100), 0);

        lv_obj_t * step_2 = lv_label_create(lv_obj_0);
        lv_obj_set_name(step_2, "step_2");
        lv_label_set_text(step_2, "Obtaining IP address");
        lv_obj_set_clickable(step_2, false);
        lv_obj_set_scrollable(step_2, false);
        lv_obj_set_x(step_2, 116);
        lv_obj_set_y(step_2, 212);
        lv_obj_set_width(step_2, 270);
        lv_obj_set_height(step_2, LV_SIZE_CONTENT);
        lv_label_set_long_mode(step_2, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_font(step_2, active_font, 0);
        lv_obj_set_style_text_color(step_2, lv_color_hex(0xf8fafc), 0);

        lv_obj_t * cancel = lv_button_create(lv_obj_0);
        lv_obj_set_x(cancel, 150);
        lv_obj_set_y(cancel, 268);
        lv_obj_set_width(cancel, 180);
        lv_obj_set_height(cancel, 40);
        lv_obj_set_clickable(cancel, true);
        lv_obj_set_style_radius(cancel, 10, 0);
        lv_obj_set_style_border_width(cancel, 1, 0);
        lv_obj_set_style_bg_opa(cancel, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(cancel, lv_color_hex(0x172030), 0);
        lv_obj_set_style_border_color(cancel, lv_color_hex(0x1e293b), 0);
        lv_obj_set_style_pad_top(cancel, -1, 0);
        lv_obj_set_style_pad_bottom(cancel, -1, 0);
        lv_obj_set_style_pad_left(cancel, -1, 0);
        lv_obj_set_style_pad_right(cancel, -1, 0);
        lv_obj_set_scrollable(cancel, false);
        lv_obj_set_style_shadow_width(cancel, 0, 0);
        lv_obj_set_style_outline_width(cancel, 0, 0);
        lv_obj_set_name(cancel, "cancel");
        lv_obj_set_style_bg_opa(cancel, (255 * 70 / 100), LV_STATE_PRESSED);
        lv_obj_t * lv_label_2 = lv_label_create(cancel);
        lv_obj_set_x(lv_label_2, 0);
        lv_obj_set_y(lv_label_2, 13);
        lv_obj_set_width(lv_label_2, 180);
        lv_obj_set_height(lv_label_2, 13);
        lv_obj_set_clickable(lv_label_2, false);
        lv_obj_set_style_text_font(lv_label_2, active_font, 0);
        lv_obj_set_style_text_color(lv_label_2, lv_color_hex(0xcbd5e1), 0);
        lv_obj_set_scrollable(lv_label_2, false);
        lv_label_set_text(lv_label_2, "CANCEL");
        lv_label_set_long_mode(lv_label_2, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_2, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_letter_space(lv_label_2, 1, 0);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

