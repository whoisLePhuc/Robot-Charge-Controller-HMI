/**
 * @file rcc_wifi_fail_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "rcc_wifi_fail_gen.h"
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

lv_obj_t * rcc_wifi_fail_create(lv_obj_t * parent, const char * failure_title, const char * failure_text, const char * attempt_text, const char * signal_text, bool show_password_hint, lv_font_t * caption_font, lv_font_t * data_font, lv_font_t * title_font, const void * result_icon)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "rcc_wifi_fail_#");
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

        lv_obj_t * lv_obj_1 = lv_obj_create(lv_obj_0);
        lv_obj_set_x(lv_obj_1, 8);
        lv_obj_set_y(lv_obj_1, 48);
        lv_obj_set_width(lv_obj_1, 464);
        lv_obj_set_height(lv_obj_1, 62);
        lv_obj_set_clickable(lv_obj_1, false);
        lv_obj_set_style_radius(lv_obj_1, 12, 0);
        lv_obj_set_style_border_width(lv_obj_1, 1, 0);
        lv_obj_set_style_bg_opa(lv_obj_1, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(lv_obj_1, lv_color_hex(0x35101b), 0);
        lv_obj_set_style_border_color(lv_obj_1, lv_color_hex(0xf43f5e), 0);
        lv_obj_set_style_pad_top(lv_obj_1, -1, 0);
        lv_obj_set_style_pad_bottom(lv_obj_1, -1, 0);
        lv_obj_set_style_pad_left(lv_obj_1, -1, 0);
        lv_obj_set_style_pad_right(lv_obj_1, -1, 0);
        lv_obj_set_scrollable(lv_obj_1, false);
        lv_obj_set_style_shadow_width(lv_obj_1, 0, 0);
        lv_obj_set_style_outline_width(lv_obj_1, 0, 0);
        lv_obj_t * lv_obj_2 = lv_obj_create(lv_obj_1);
        lv_obj_set_x(lv_obj_2, 14);
        lv_obj_set_y(lv_obj_2, 13);
        lv_obj_set_width(lv_obj_2, 36);
        lv_obj_set_height(lv_obj_2, 36);
        lv_obj_set_clickable(lv_obj_2, false);
        lv_obj_set_style_radius(lv_obj_2, 18, 0);
        lv_obj_set_style_border_width(lv_obj_2, 0, 0);
        lv_obj_set_style_bg_opa(lv_obj_2, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(lv_obj_2, lv_color_hex(0x4a1624), 0);
        lv_obj_set_style_pad_top(lv_obj_2, 0, 0);
        lv_obj_set_style_pad_bottom(lv_obj_2, 0, 0);
        lv_obj_set_style_pad_left(lv_obj_2, 0, 0);
        lv_obj_set_style_pad_right(lv_obj_2, 0, 0);
        lv_obj_set_scrollable(lv_obj_2, false);
        lv_obj_set_style_shadow_width(lv_obj_2, 0, 0);
        lv_obj_set_style_outline_width(lv_obj_2, 0, 0);

        lv_obj_t * lv_image_0 = lv_image_create(lv_obj_1);
        lv_obj_set_x(lv_image_0, 22);
        lv_obj_set_y(lv_image_0, 21);
        lv_obj_set_width(lv_image_0, 20);
        lv_obj_set_height(lv_image_0, 20);
        lv_obj_set_clickable(lv_image_0, false);
        lv_obj_set_scrollable(lv_image_0, false);
        lv_image_set_src(lv_image_0, result_icon);
        lv_obj_set_style_image_recolor(lv_image_0, lv_color_hex(0xf43f5e), 0);
        lv_obj_set_style_image_recolor_opa(lv_image_0, (255 * 100 / 100), 0);

        lv_obj_t * lv_label_0 = lv_label_create(lv_obj_1);
        lv_obj_set_x(lv_label_0, 62);
        lv_obj_set_y(lv_label_0, 9);
        lv_obj_set_width(lv_label_0, 390);
        lv_obj_set_height(lv_label_0, 16);
        lv_obj_set_clickable(lv_label_0, false);
        lv_obj_set_style_text_font(lv_label_0, title_font, 0);
        lv_obj_set_style_text_color(lv_label_0, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_scrollable(lv_label_0, false);
        lv_label_set_text(lv_label_0, failure_title);
        lv_label_set_long_mode(lv_label_0, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * lv_label_1 = lv_label_create(lv_obj_1);
        lv_obj_set_x(lv_label_1, 62);
        lv_obj_set_y(lv_label_1, 28);
        lv_obj_set_width(lv_label_1, 388);
        lv_obj_set_height(lv_label_1, LV_SIZE_CONTENT);
        lv_obj_set_clickable(lv_label_1, false);
        lv_obj_set_style_text_font(lv_label_1, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_1, lv_color_hex(0xcbd5e1), 0);
        lv_obj_set_scrollable(lv_label_1, false);
        lv_label_set_text(lv_label_1, failure_text);
        lv_label_set_long_mode(lv_label_1, LV_LABEL_LONG_MODE_WRAP);
        lv_obj_set_style_text_align(lv_label_1, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * lv_obj_3 = lv_obj_create(lv_obj_0);
        lv_obj_set_x(lv_obj_3, 8);
        lv_obj_set_y(lv_obj_3, 118);
        lv_obj_set_width(lv_obj_3, 229);
        lv_obj_set_height(lv_obj_3, 44);
        lv_obj_set_clickable(lv_obj_3, false);
        lv_obj_set_style_radius(lv_obj_3, 10, 0);
        lv_obj_set_style_border_width(lv_obj_3, 1, 0);
        lv_obj_set_style_bg_opa(lv_obj_3, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(lv_obj_3, lv_color_hex(0x111722), 0);
        lv_obj_set_style_border_color(lv_obj_3, lv_color_hex(0x1e293b), 0);
        lv_obj_set_style_pad_top(lv_obj_3, -1, 0);
        lv_obj_set_style_pad_bottom(lv_obj_3, -1, 0);
        lv_obj_set_style_pad_left(lv_obj_3, -1, 0);
        lv_obj_set_style_pad_right(lv_obj_3, -1, 0);
        lv_obj_set_scrollable(lv_obj_3, false);
        lv_obj_set_style_shadow_width(lv_obj_3, 0, 0);
        lv_obj_set_style_outline_width(lv_obj_3, 0, 0);
        lv_obj_t * lv_label_2 = lv_label_create(lv_obj_3);
        lv_obj_set_x(lv_label_2, 12);
        lv_obj_set_y(lv_label_2, 7);
        lv_obj_set_width(lv_label_2, 56);
        lv_obj_set_height(lv_label_2, 13);
        lv_obj_set_clickable(lv_label_2, false);
        lv_obj_set_style_text_font(lv_label_2, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_2, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_scrollable(lv_label_2, false);
        lv_label_set_text(lv_label_2, "ATTEMPT");
        lv_label_set_long_mode(lv_label_2, LV_LABEL_LONG_MODE_WRAP);
        lv_obj_set_style_text_align(lv_label_2, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_letter_space(lv_label_2, 1, 0);

        lv_obj_t * lv_label_3 = lv_label_create(lv_obj_3);
        lv_obj_set_x(lv_label_3, 12);
        lv_obj_set_y(lv_label_3, 24);
        lv_obj_set_width(lv_label_3, 205);
        lv_obj_set_height(lv_label_3, 13);
        lv_obj_set_clickable(lv_label_3, false);
        lv_obj_set_style_text_font(lv_label_3, data_font, 0);
        lv_obj_set_style_text_color(lv_label_3, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_scrollable(lv_label_3, false);
        lv_label_set_text(lv_label_3, attempt_text);
        lv_label_set_long_mode(lv_label_3, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_3, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * lv_obj_4 = lv_obj_create(lv_obj_0);
        lv_obj_set_x(lv_obj_4, 243);
        lv_obj_set_y(lv_obj_4, 118);
        lv_obj_set_width(lv_obj_4, 229);
        lv_obj_set_height(lv_obj_4, 44);
        lv_obj_set_clickable(lv_obj_4, false);
        lv_obj_set_style_radius(lv_obj_4, 10, 0);
        lv_obj_set_style_border_width(lv_obj_4, 1, 0);
        lv_obj_set_style_bg_opa(lv_obj_4, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(lv_obj_4, lv_color_hex(0x111722), 0);
        lv_obj_set_style_border_color(lv_obj_4, lv_color_hex(0x1e293b), 0);
        lv_obj_set_style_pad_top(lv_obj_4, -1, 0);
        lv_obj_set_style_pad_bottom(lv_obj_4, -1, 0);
        lv_obj_set_style_pad_left(lv_obj_4, -1, 0);
        lv_obj_set_style_pad_right(lv_obj_4, -1, 0);
        lv_obj_set_scrollable(lv_obj_4, false);
        lv_obj_set_style_shadow_width(lv_obj_4, 0, 0);
        lv_obj_set_style_outline_width(lv_obj_4, 0, 0);
        lv_obj_t * lv_label_4 = lv_label_create(lv_obj_4);
        lv_obj_set_x(lv_label_4, 12);
        lv_obj_set_y(lv_label_4, 7);
        lv_obj_set_width(lv_label_4, 44);
        lv_obj_set_height(lv_label_4, 13);
        lv_obj_set_clickable(lv_label_4, false);
        lv_obj_set_style_text_font(lv_label_4, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_4, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_scrollable(lv_label_4, false);
        lv_label_set_text(lv_label_4, "SIGNAL");
        lv_label_set_long_mode(lv_label_4, LV_LABEL_LONG_MODE_WRAP);
        lv_obj_set_style_text_align(lv_label_4, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_letter_space(lv_label_4, 1, 0);

        lv_obj_t * lv_label_5 = lv_label_create(lv_obj_4);
        lv_obj_set_x(lv_label_5, 12);
        lv_obj_set_y(lv_label_5, 24);
        lv_obj_set_width(lv_label_5, 205);
        lv_obj_set_height(lv_label_5, 13);
        lv_obj_set_clickable(lv_label_5, false);
        lv_obj_set_style_text_font(lv_label_5, data_font, 0);
        lv_obj_set_style_text_color(lv_label_5, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_scrollable(lv_label_5, false);
        lv_label_set_text(lv_label_5, signal_text);
        lv_label_set_long_mode(lv_label_5, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_5, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * lv_label_6 = lv_label_create(lv_obj_0);
        lv_obj_set_x(lv_label_6, 12);
        lv_obj_set_y(lv_label_6, 172);
        lv_obj_set_width(lv_label_6, 456);
        lv_obj_set_height(lv_label_6, 13);
        lv_obj_set_clickable(lv_label_6, false);
        lv_obj_set_style_text_font(lv_label_6, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_6, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_scrollable(lv_label_6, false);
        lv_label_set_text(lv_label_6, "Passwords are case-sensitive. WPA2 passwords have 8 to 63 characters.");
        lv_label_set_long_mode(lv_label_6, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_6, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_hidden(lv_label_6, !show_password_hint);

        lv_obj_t * choose_other = lv_button_create(lv_obj_0);
        lv_obj_set_x(choose_other, 8);
        lv_obj_set_y(choose_other, 268);
        lv_obj_set_width(choose_other, 228);
        lv_obj_set_height(choose_other, 40);
        lv_obj_set_clickable(choose_other, true);
        lv_obj_set_style_radius(choose_other, 10, 0);
        lv_obj_set_style_border_width(choose_other, 1, 0);
        lv_obj_set_style_bg_opa(choose_other, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(choose_other, lv_color_hex(0x172030), 0);
        lv_obj_set_style_border_color(choose_other, lv_color_hex(0x1e293b), 0);
        lv_obj_set_style_pad_top(choose_other, -1, 0);
        lv_obj_set_style_pad_bottom(choose_other, -1, 0);
        lv_obj_set_style_pad_left(choose_other, -1, 0);
        lv_obj_set_style_pad_right(choose_other, -1, 0);
        lv_obj_set_scrollable(choose_other, false);
        lv_obj_set_name(choose_other, "choose_other");
        lv_obj_set_style_bg_opa(choose_other, (255 * 70 / 100), LV_STATE_PRESSED);
        lv_obj_set_style_shadow_width(choose_other, 0, 0);
        lv_obj_set_style_outline_width(choose_other, 0, 0);
        lv_obj_t * lv_label_7 = lv_label_create(choose_other);
        lv_obj_set_x(lv_label_7, 0);
        lv_obj_set_y(lv_label_7, 13);
        lv_obj_set_width(lv_label_7, 228);
        lv_obj_set_height(lv_label_7, 13);
        lv_obj_set_clickable(lv_label_7, false);
        lv_obj_set_style_text_font(lv_label_7, data_font, 0);
        lv_obj_set_style_text_color(lv_label_7, lv_color_hex(0xcbd5e1), 0);
        lv_obj_set_scrollable(lv_label_7, false);
        lv_label_set_text(lv_label_7, "CHOOSE OTHER");
        lv_label_set_long_mode(lv_label_7, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_7, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_letter_space(lv_label_7, 1, 0);

        lv_obj_t * retry = lv_button_create(lv_obj_0);
        lv_obj_set_x(retry, 244);
        lv_obj_set_y(retry, 268);
        lv_obj_set_width(retry, 228);
        lv_obj_set_height(retry, 40);
        lv_obj_set_clickable(retry, true);
        lv_obj_set_style_radius(retry, 10, 0);
        lv_obj_set_style_border_width(retry, 0, 0);
        lv_obj_set_style_bg_opa(retry, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(retry, lv_color_hex(0x0ea5e9), 0);
        lv_obj_set_style_pad_top(retry, 0, 0);
        lv_obj_set_style_pad_bottom(retry, 0, 0);
        lv_obj_set_style_pad_left(retry, 0, 0);
        lv_obj_set_style_pad_right(retry, 0, 0);
        lv_obj_set_scrollable(retry, false);
        lv_obj_set_name(retry, "retry");
        lv_obj_set_style_bg_opa(retry, (255 * 70 / 100), LV_STATE_PRESSED);
        lv_obj_set_style_shadow_width(retry, 0, 0);
        lv_obj_set_style_outline_width(retry, 0, 0);
        lv_obj_t * lv_label_8 = lv_label_create(retry);
        lv_obj_set_x(lv_label_8, 0);
        lv_obj_set_y(lv_label_8, 13);
        lv_obj_set_width(lv_label_8, 228);
        lv_obj_set_height(lv_label_8, 13);
        lv_obj_set_clickable(lv_label_8, false);
        lv_obj_set_style_text_font(lv_label_8, data_font, 0);
        lv_obj_set_style_text_color(lv_label_8, lv_color_hex(0xffffff), 0);
        lv_obj_set_scrollable(lv_label_8, false);
        lv_label_set_text(lv_label_8, "RETRY");
        lv_label_set_long_mode(lv_label_8, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_8, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_letter_space(lv_label_8, 1, 0);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

lv_obj_t * rcc_wifi_fail_get_choose_other(lv_obj_t *parent)
{
    return lv_obj_find_by_name(parent, "choose_other");
}

lv_obj_t * rcc_wifi_fail_get_retry(lv_obj_t *parent)
{
    return lv_obj_find_by_name(parent, "retry");
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

