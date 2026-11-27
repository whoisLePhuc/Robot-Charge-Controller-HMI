/**
 * @file rcc_wifi_row_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "rcc_wifi_row_gen.h"
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

lv_obj_t * rcc_wifi_row_create(lv_obj_t * parent, int32_t row_index, const char * ssid, const char * details, const char * signal, bool secured, int32_t title_width, lv_color_t background, lv_color_t border, lv_color_t details_color, lv_color_t bar_short, lv_color_t bar_low, lv_color_t bar_high, lv_color_t bar_full, lv_font_t * caption_font, lv_font_t * data_font, const void * lock_icon)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_button_0 = lv_button_create(parent);
        lv_obj_set_name_static(lv_button_0, "rcc_wifi_row_#");
        lv_obj_set_x(lv_button_0, 0);
        lv_obj_set_y(lv_button_0, row_index * 44);
        lv_obj_set_width(lv_button_0, 464);
        lv_obj_set_height(lv_button_0, 40);
        lv_obj_set_style_bg_color(lv_button_0, background, 0);
        lv_obj_set_style_bg_opa(lv_button_0, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(lv_button_0, 10, 0);
        lv_obj_set_style_border_width(lv_button_0, 1, 0);
        lv_obj_set_style_pad_all(lv_button_0, -1, 0);
        lv_obj_set_style_border_color(lv_button_0, border, 0);
        lv_obj_set_style_outline_width(lv_button_0, 0, 0);
        lv_obj_set_style_shadow_width(lv_button_0, 0, 0);
        lv_obj_set_scrollable(lv_button_0, false);
        lv_obj_set_style_bg_opa(lv_button_0, (255 * 70 / 100), LV_STATE_PRESSED);

        lv_obj_t * lv_label_0 = lv_label_create(lv_button_0);
        lv_obj_set_x(lv_label_0, 12);
        lv_obj_set_y(lv_label_0, 7);
        lv_obj_set_width(lv_label_0, title_width);
        lv_obj_set_height(lv_label_0, 13);
        lv_obj_set_clickable(lv_label_0, false);
        lv_obj_set_style_text_font(lv_label_0, data_font, 0);
        lv_obj_set_style_text_color(lv_label_0, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_scrollable(lv_label_0, false);
        lv_label_set_text(lv_label_0, ssid);
        lv_label_set_long_mode(lv_label_0, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * lv_label_1 = lv_label_create(lv_button_0);
        lv_obj_set_x(lv_label_1, 12);
        lv_obj_set_y(lv_label_1, 21);
        lv_obj_set_width(lv_label_1, 330);
        lv_obj_set_height(lv_label_1, 13);
        lv_obj_set_clickable(lv_label_1, false);
        lv_obj_set_style_text_font(lv_label_1, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_1, details_color, 0);
        lv_obj_set_scrollable(lv_label_1, false);
        lv_label_set_text(lv_label_1, details);
        lv_label_set_long_mode(lv_label_1, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_1, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * lv_obj_0 = lv_obj_create(lv_button_0);
        lv_obj_set_x(lv_obj_0, 434);
        lv_obj_set_y(lv_obj_0, 23);
        lv_obj_set_width(lv_obj_0, 3);
        lv_obj_set_height(lv_obj_0, 4);
        lv_obj_set_clickable(lv_obj_0, false);
        lv_obj_set_style_radius(lv_obj_0, 1, 0);
        lv_obj_set_style_border_width(lv_obj_0, 0, 0);
        lv_obj_set_style_bg_opa(lv_obj_0, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(lv_obj_0, bar_short, 0);
        lv_obj_set_style_pad_top(lv_obj_0, 0, 0);
        lv_obj_set_style_pad_bottom(lv_obj_0, 0, 0);
        lv_obj_set_style_pad_left(lv_obj_0, 0, 0);
        lv_obj_set_style_pad_right(lv_obj_0, 0, 0);
        lv_obj_set_scrollable(lv_obj_0, false);
        lv_obj_set_style_shadow_width(lv_obj_0, 0, 0);
        lv_obj_set_style_outline_width(lv_obj_0, 0, 0);

        lv_obj_t * lv_obj_1 = lv_obj_create(lv_button_0);
        lv_obj_set_x(lv_obj_1, 439);
        lv_obj_set_y(lv_obj_1, 20);
        lv_obj_set_width(lv_obj_1, 3);
        lv_obj_set_height(lv_obj_1, 7);
        lv_obj_set_clickable(lv_obj_1, false);
        lv_obj_set_style_radius(lv_obj_1, 1, 0);
        lv_obj_set_style_border_width(lv_obj_1, 0, 0);
        lv_obj_set_style_bg_opa(lv_obj_1, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(lv_obj_1, bar_low, 0);
        lv_obj_set_style_pad_top(lv_obj_1, 0, 0);
        lv_obj_set_style_pad_bottom(lv_obj_1, 0, 0);
        lv_obj_set_style_pad_left(lv_obj_1, 0, 0);
        lv_obj_set_style_pad_right(lv_obj_1, 0, 0);
        lv_obj_set_scrollable(lv_obj_1, false);
        lv_obj_set_style_shadow_width(lv_obj_1, 0, 0);
        lv_obj_set_style_outline_width(lv_obj_1, 0, 0);

        lv_obj_t * lv_obj_2 = lv_obj_create(lv_button_0);
        lv_obj_set_x(lv_obj_2, 444);
        lv_obj_set_y(lv_obj_2, 17);
        lv_obj_set_width(lv_obj_2, 3);
        lv_obj_set_height(lv_obj_2, 10);
        lv_obj_set_clickable(lv_obj_2, false);
        lv_obj_set_style_radius(lv_obj_2, 1, 0);
        lv_obj_set_style_border_width(lv_obj_2, 0, 0);
        lv_obj_set_style_bg_opa(lv_obj_2, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(lv_obj_2, bar_high, 0);
        lv_obj_set_style_pad_top(lv_obj_2, 0, 0);
        lv_obj_set_style_pad_bottom(lv_obj_2, 0, 0);
        lv_obj_set_style_pad_left(lv_obj_2, 0, 0);
        lv_obj_set_style_pad_right(lv_obj_2, 0, 0);
        lv_obj_set_scrollable(lv_obj_2, false);
        lv_obj_set_style_shadow_width(lv_obj_2, 0, 0);
        lv_obj_set_style_outline_width(lv_obj_2, 0, 0);

        lv_obj_t * lv_obj_3 = lv_obj_create(lv_button_0);
        lv_obj_set_x(lv_obj_3, 449);
        lv_obj_set_y(lv_obj_3, 14);
        lv_obj_set_width(lv_obj_3, 3);
        lv_obj_set_height(lv_obj_3, 13);
        lv_obj_set_clickable(lv_obj_3, false);
        lv_obj_set_style_radius(lv_obj_3, 1, 0);
        lv_obj_set_style_border_width(lv_obj_3, 0, 0);
        lv_obj_set_style_bg_opa(lv_obj_3, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(lv_obj_3, bar_full, 0);
        lv_obj_set_style_pad_top(lv_obj_3, 0, 0);
        lv_obj_set_style_pad_bottom(lv_obj_3, 0, 0);
        lv_obj_set_style_pad_left(lv_obj_3, 0, 0);
        lv_obj_set_style_pad_right(lv_obj_3, 0, 0);
        lv_obj_set_scrollable(lv_obj_3, false);
        lv_obj_set_style_shadow_width(lv_obj_3, 0, 0);
        lv_obj_set_style_outline_width(lv_obj_3, 0, 0);

        lv_obj_t * lv_label_2 = lv_label_create(lv_button_0);
        lv_obj_set_x(lv_label_2, 374);
        lv_obj_set_y(lv_label_2, 13);
        lv_obj_set_width(lv_label_2, 50);
        lv_obj_set_height(lv_label_2, 13);
        lv_obj_set_clickable(lv_label_2, false);
        lv_obj_set_style_text_font(lv_label_2, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_2, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_scrollable(lv_label_2, false);
        lv_label_set_text(lv_label_2, signal);
        lv_label_set_long_mode(lv_label_2, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_2, LV_TEXT_ALIGN_RIGHT, 0);

        lv_obj_t * lv_image_0 = lv_image_create(lv_button_0);
        lv_obj_set_x(lv_image_0, 353);
        lv_obj_set_y(lv_image_0, 14);
        lv_obj_set_width(lv_image_0, 11);
        lv_obj_set_height(lv_image_0, 11);
        lv_obj_set_clickable(lv_image_0, false);
        lv_obj_set_scrollable(lv_image_0, false);
        lv_image_set_src(lv_image_0, lock_icon);
        lv_obj_set_style_image_recolor(lv_image_0, lv_color_hex(0x64748b), 0);
        lv_obj_set_style_image_recolor_opa(lv_image_0, (255 * 100 / 100), 0);
        lv_obj_set_hidden(lv_image_0, !secured);

        the_root = lv_button_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

