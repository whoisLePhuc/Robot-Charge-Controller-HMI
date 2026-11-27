/**
 * @file rcc_wifi_info_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "rcc_wifi_info_gen.h"
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

lv_obj_t * rcc_wifi_info_create(lv_obj_t * parent, const char * connected_ssid, const char * ip_address, const char * gateway, const char * signal_text, const char * mac_address, const char * ota_title, const char * ota_value, const char * slot_text, const char * ota_button_text, bool ota_armed, lv_font_t * caption_font, lv_font_t * data_font, lv_font_t * title_font, const void * result_icon)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "rcc_wifi_info_#");
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
        lv_obj_set_style_bg_color(lv_obj_1, lv_color_hex(0x0a2617), 0);
        lv_obj_set_style_border_color(lv_obj_1, lv_color_hex(0x22c55e), 0);
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
        lv_obj_set_style_bg_color(lv_obj_2, lv_color_hex(0x0f3a22), 0);
        lv_obj_set_style_pad_top(lv_obj_2, 0, 0);
        lv_obj_set_style_pad_bottom(lv_obj_2, 0, 0);
        lv_obj_set_style_pad_left(lv_obj_2, 0, 0);
        lv_obj_set_style_pad_right(lv_obj_2, 0, 0);
        lv_obj_set_scrollable(lv_obj_2, false);
        lv_obj_set_style_shadow_width(lv_obj_2, 0, 0);
        lv_obj_set_style_outline_width(lv_obj_2, 0, 0);

        lv_obj_t * lv_image_0 = lv_image_create(lv_obj_1);
        lv_obj_set_x(lv_image_0, 21);
        lv_obj_set_y(lv_image_0, 20);
        lv_obj_set_width(lv_image_0, 22);
        lv_obj_set_height(lv_image_0, 22);
        lv_obj_set_clickable(lv_image_0, false);
        lv_obj_set_scrollable(lv_image_0, false);
        lv_image_set_src(lv_image_0, result_icon);
        lv_obj_set_style_image_recolor(lv_image_0, lv_color_hex(0x22c55e), 0);
        lv_obj_set_style_image_recolor_opa(lv_image_0, (255 * 100 / 100), 0);

        lv_obj_t * lv_label_0 = lv_label_create(lv_obj_1);
        lv_obj_set_x(lv_label_0, 62);
        lv_obj_set_y(lv_label_0, 15);
        lv_obj_set_width(lv_label_0, 290);
        lv_obj_set_height(lv_label_0, 16);
        lv_obj_set_clickable(lv_label_0, false);
        lv_obj_set_style_text_font(lv_label_0, title_font, 0);
        lv_obj_set_style_text_color(lv_label_0, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_scrollable(lv_label_0, false);
        lv_label_set_text(lv_label_0, connected_ssid);
        lv_label_set_long_mode(lv_label_0, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * lv_label_1 = lv_label_create(lv_obj_1);
        lv_obj_set_x(lv_label_1, 62);
        lv_obj_set_y(lv_label_1, 34);
        lv_obj_set_width(lv_label_1, 290);
        lv_obj_set_height(lv_label_1, 13);
        lv_obj_set_clickable(lv_label_1, false);
        lv_obj_set_style_text_font(lv_label_1, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_1, lv_color_hex(0xcbd5e1), 0);
        lv_obj_set_scrollable(lv_label_1, false);
        lv_label_set_text(lv_label_1, "Station connected");
        lv_label_set_long_mode(lv_label_1, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_1, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * lv_obj_3 = lv_obj_create(lv_obj_1);
        lv_obj_set_x(lv_obj_3, 396);
        lv_obj_set_y(lv_obj_3, 22);
        lv_obj_set_width(lv_obj_3, 54);
        lv_obj_set_height(lv_obj_3, 17);
        lv_obj_set_clickable(lv_obj_3, false);
        lv_obj_set_style_radius(lv_obj_3, 6, 0);
        lv_obj_set_style_border_width(lv_obj_3, 0, 0);
        lv_obj_set_style_bg_opa(lv_obj_3, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(lv_obj_3, lv_color_hex(0x0f3a22), 0);
        lv_obj_set_style_pad_top(lv_obj_3, 0, 0);
        lv_obj_set_style_pad_bottom(lv_obj_3, 0, 0);
        lv_obj_set_style_pad_left(lv_obj_3, 0, 0);
        lv_obj_set_style_pad_right(lv_obj_3, 0, 0);
        lv_obj_set_scrollable(lv_obj_3, false);
        lv_obj_set_style_shadow_width(lv_obj_3, 0, 0);
        lv_obj_set_style_outline_width(lv_obj_3, 0, 0);

        lv_obj_t * lv_label_2 = lv_label_create(lv_obj_1);
        lv_obj_set_x(lv_label_2, 403);
        lv_obj_set_y(lv_label_2, 24);
        lv_obj_set_width(lv_label_2, 40);
        lv_obj_set_height(lv_label_2, 13);
        lv_obj_set_clickable(lv_label_2, false);
        lv_obj_set_style_text_font(lv_label_2, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_2, lv_color_hex(0x22c55e), 0);
        lv_obj_set_scrollable(lv_label_2, false);
        lv_label_set_text(lv_label_2, "ONLINE");
        lv_label_set_long_mode(lv_label_2, LV_LABEL_LONG_MODE_WRAP);
        lv_obj_set_style_text_align(lv_label_2, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * lv_obj_4 = lv_obj_create(lv_obj_0);
        lv_obj_set_x(lv_obj_4, 8);
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
        lv_obj_t * lv_label_3 = lv_label_create(lv_obj_4);
        lv_obj_set_x(lv_label_3, 12);
        lv_obj_set_y(lv_label_3, 7);
        lv_obj_set_width(lv_label_3, 72);
        lv_obj_set_height(lv_label_3, 13);
        lv_obj_set_clickable(lv_label_3, false);
        lv_obj_set_style_text_font(lv_label_3, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_3, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_scrollable(lv_label_3, false);
        lv_label_set_text(lv_label_3, "IP ADDRESS");
        lv_label_set_long_mode(lv_label_3, LV_LABEL_LONG_MODE_WRAP);
        lv_obj_set_style_text_align(lv_label_3, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_letter_space(lv_label_3, 1, 0);

        lv_obj_t * lv_label_4 = lv_label_create(lv_obj_4);
        lv_obj_set_x(lv_label_4, 12);
        lv_obj_set_y(lv_label_4, 24);
        lv_obj_set_width(lv_label_4, 205);
        lv_obj_set_height(lv_label_4, 13);
        lv_obj_set_clickable(lv_label_4, false);
        lv_obj_set_style_text_font(lv_label_4, data_font, 0);
        lv_obj_set_style_text_color(lv_label_4, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_scrollable(lv_label_4, false);
        lv_label_set_text(lv_label_4, ip_address);
        lv_label_set_long_mode(lv_label_4, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_4, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * lv_obj_5 = lv_obj_create(lv_obj_0);
        lv_obj_set_x(lv_obj_5, 243);
        lv_obj_set_y(lv_obj_5, 118);
        lv_obj_set_width(lv_obj_5, 229);
        lv_obj_set_height(lv_obj_5, 44);
        lv_obj_set_clickable(lv_obj_5, false);
        lv_obj_set_style_radius(lv_obj_5, 10, 0);
        lv_obj_set_style_border_width(lv_obj_5, 1, 0);
        lv_obj_set_style_bg_opa(lv_obj_5, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(lv_obj_5, lv_color_hex(0x111722), 0);
        lv_obj_set_style_border_color(lv_obj_5, lv_color_hex(0x1e293b), 0);
        lv_obj_set_style_pad_top(lv_obj_5, -1, 0);
        lv_obj_set_style_pad_bottom(lv_obj_5, -1, 0);
        lv_obj_set_style_pad_left(lv_obj_5, -1, 0);
        lv_obj_set_style_pad_right(lv_obj_5, -1, 0);
        lv_obj_set_scrollable(lv_obj_5, false);
        lv_obj_set_style_shadow_width(lv_obj_5, 0, 0);
        lv_obj_set_style_outline_width(lv_obj_5, 0, 0);
        lv_obj_t * lv_label_5 = lv_label_create(lv_obj_5);
        lv_obj_set_x(lv_label_5, 12);
        lv_obj_set_y(lv_label_5, 7);
        lv_obj_set_width(lv_label_5, 61);
        lv_obj_set_height(lv_label_5, 13);
        lv_obj_set_clickable(lv_label_5, false);
        lv_obj_set_style_text_font(lv_label_5, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_5, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_scrollable(lv_label_5, false);
        lv_label_set_text(lv_label_5, "GATEWAY");
        lv_label_set_long_mode(lv_label_5, LV_LABEL_LONG_MODE_WRAP);
        lv_obj_set_style_text_align(lv_label_5, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_letter_space(lv_label_5, 1, 0);

        lv_obj_t * lv_label_6 = lv_label_create(lv_obj_5);
        lv_obj_set_x(lv_label_6, 12);
        lv_obj_set_y(lv_label_6, 24);
        lv_obj_set_width(lv_label_6, 205);
        lv_obj_set_height(lv_label_6, 13);
        lv_obj_set_clickable(lv_label_6, false);
        lv_obj_set_style_text_font(lv_label_6, data_font, 0);
        lv_obj_set_style_text_color(lv_label_6, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_scrollable(lv_label_6, false);
        lv_label_set_text(lv_label_6, gateway);
        lv_label_set_long_mode(lv_label_6, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_6, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * lv_obj_6 = lv_obj_create(lv_obj_0);
        lv_obj_set_x(lv_obj_6, 8);
        lv_obj_set_y(lv_obj_6, 168);
        lv_obj_set_width(lv_obj_6, 229);
        lv_obj_set_height(lv_obj_6, 44);
        lv_obj_set_clickable(lv_obj_6, false);
        lv_obj_set_style_radius(lv_obj_6, 10, 0);
        lv_obj_set_style_border_width(lv_obj_6, 1, 0);
        lv_obj_set_style_bg_opa(lv_obj_6, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(lv_obj_6, lv_color_hex(0x111722), 0);
        lv_obj_set_style_border_color(lv_obj_6, lv_color_hex(0x1e293b), 0);
        lv_obj_set_style_pad_top(lv_obj_6, -1, 0);
        lv_obj_set_style_pad_bottom(lv_obj_6, -1, 0);
        lv_obj_set_style_pad_left(lv_obj_6, -1, 0);
        lv_obj_set_style_pad_right(lv_obj_6, -1, 0);
        lv_obj_set_scrollable(lv_obj_6, false);
        lv_obj_set_style_shadow_width(lv_obj_6, 0, 0);
        lv_obj_set_style_outline_width(lv_obj_6, 0, 0);
        lv_obj_t * lv_label_7 = lv_label_create(lv_obj_6);
        lv_obj_set_x(lv_label_7, 12);
        lv_obj_set_y(lv_label_7, 7);
        lv_obj_set_width(lv_label_7, 44);
        lv_obj_set_height(lv_label_7, 13);
        lv_obj_set_clickable(lv_label_7, false);
        lv_obj_set_style_text_font(lv_label_7, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_7, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_scrollable(lv_label_7, false);
        lv_label_set_text(lv_label_7, "SIGNAL");
        lv_label_set_long_mode(lv_label_7, LV_LABEL_LONG_MODE_WRAP);
        lv_obj_set_style_text_align(lv_label_7, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_letter_space(lv_label_7, 1, 0);

        lv_obj_t * lv_label_8 = lv_label_create(lv_obj_6);
        lv_obj_set_x(lv_label_8, 12);
        lv_obj_set_y(lv_label_8, 24);
        lv_obj_set_width(lv_label_8, 205);
        lv_obj_set_height(lv_label_8, 13);
        lv_obj_set_clickable(lv_label_8, false);
        lv_obj_set_style_text_font(lv_label_8, data_font, 0);
        lv_obj_set_style_text_color(lv_label_8, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_scrollable(lv_label_8, false);
        lv_label_set_text(lv_label_8, signal_text);
        lv_label_set_long_mode(lv_label_8, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_8, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * lv_obj_7 = lv_obj_create(lv_obj_0);
        lv_obj_set_x(lv_obj_7, 243);
        lv_obj_set_y(lv_obj_7, 168);
        lv_obj_set_width(lv_obj_7, 229);
        lv_obj_set_height(lv_obj_7, 44);
        lv_obj_set_clickable(lv_obj_7, false);
        lv_obj_set_style_radius(lv_obj_7, 10, 0);
        lv_obj_set_style_border_width(lv_obj_7, 1, 0);
        lv_obj_set_style_bg_opa(lv_obj_7, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(lv_obj_7, lv_color_hex(0x111722), 0);
        lv_obj_set_style_border_color(lv_obj_7, lv_color_hex(0x1e293b), 0);
        lv_obj_set_style_pad_top(lv_obj_7, -1, 0);
        lv_obj_set_style_pad_bottom(lv_obj_7, -1, 0);
        lv_obj_set_style_pad_left(lv_obj_7, -1, 0);
        lv_obj_set_style_pad_right(lv_obj_7, -1, 0);
        lv_obj_set_scrollable(lv_obj_7, false);
        lv_obj_set_style_shadow_width(lv_obj_7, 0, 0);
        lv_obj_set_style_outline_width(lv_obj_7, 0, 0);
        lv_obj_t * lv_label_9 = lv_label_create(lv_obj_7);
        lv_obj_set_x(lv_label_9, 12);
        lv_obj_set_y(lv_label_9, 7);
        lv_obj_set_width(lv_label_9, 27);
        lv_obj_set_height(lv_label_9, 13);
        lv_obj_set_clickable(lv_label_9, false);
        lv_obj_set_style_text_font(lv_label_9, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_9, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_scrollable(lv_label_9, false);
        lv_label_set_text(lv_label_9, "MAC");
        lv_label_set_long_mode(lv_label_9, LV_LABEL_LONG_MODE_WRAP);
        lv_obj_set_style_text_align(lv_label_9, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_letter_space(lv_label_9, 1, 0);

        lv_obj_t * lv_label_10 = lv_label_create(lv_obj_7);
        lv_obj_set_x(lv_label_10, 12);
        lv_obj_set_y(lv_label_10, 24);
        lv_obj_set_width(lv_label_10, 205);
        lv_obj_set_height(lv_label_10, 13);
        lv_obj_set_clickable(lv_label_10, false);
        lv_obj_set_style_text_font(lv_label_10, data_font, 0);
        lv_obj_set_style_text_color(lv_label_10, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_scrollable(lv_label_10, false);
        lv_label_set_text(lv_label_10, mac_address);
        lv_label_set_long_mode(lv_label_10, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_10, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * lv_obj_8 = lv_obj_create(lv_obj_0);
        lv_obj_set_x(lv_obj_8, 8);
        lv_obj_set_y(lv_obj_8, 218);
        lv_obj_set_width(lv_obj_8, 229);
        lv_obj_set_height(lv_obj_8, 44);
        lv_obj_set_clickable(lv_obj_8, false);
        lv_obj_set_style_radius(lv_obj_8, 10, 0);
        lv_obj_set_style_border_width(lv_obj_8, 1, 0);
        lv_obj_set_style_bg_opa(lv_obj_8, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(lv_obj_8, lv_color_hex(0x111722), 0);
        lv_obj_set_style_border_color(lv_obj_8, lv_color_hex(0x1e293b), 0);
        lv_obj_set_style_pad_top(lv_obj_8, -1, 0);
        lv_obj_set_style_pad_bottom(lv_obj_8, -1, 0);
        lv_obj_set_style_pad_left(lv_obj_8, -1, 0);
        lv_obj_set_style_pad_right(lv_obj_8, -1, 0);
        lv_obj_set_scrollable(lv_obj_8, false);
        lv_obj_set_style_shadow_width(lv_obj_8, 0, 0);
        lv_obj_set_style_outline_width(lv_obj_8, 0, 0);
        lv_obj_t * lv_label_11 = lv_label_create(lv_obj_8);
        lv_obj_set_x(lv_label_11, 12);
        lv_obj_set_y(lv_label_11, 7);
        lv_obj_set_width(lv_label_11, 57);
        lv_obj_set_height(lv_label_11, 13);
        lv_obj_set_clickable(lv_label_11, false);
        lv_obj_set_style_text_font(lv_label_11, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_11, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_scrollable(lv_label_11, false);
        lv_label_set_text(lv_label_11, ota_title);
        lv_label_set_long_mode(lv_label_11, LV_LABEL_LONG_MODE_WRAP);
        lv_obj_set_style_text_align(lv_label_11, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_letter_space(lv_label_11, 1, 0);

        lv_obj_t * lv_label_12 = lv_label_create(lv_obj_8);
        lv_obj_set_x(lv_label_12, 12);
        lv_obj_set_y(lv_label_12, 24);
        lv_obj_set_width(lv_label_12, 205);
        lv_obj_set_height(lv_label_12, 13);
        lv_obj_set_clickable(lv_label_12, false);
        lv_obj_set_style_text_font(lv_label_12, data_font, 0);
        lv_obj_set_style_text_color(lv_label_12, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_scrollable(lv_label_12, false);
        lv_label_set_text(lv_label_12, ota_value);
        lv_label_set_long_mode(lv_label_12, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_12, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * lv_obj_9 = lv_obj_create(lv_obj_0);
        lv_obj_set_x(lv_obj_9, 243);
        lv_obj_set_y(lv_obj_9, 218);
        lv_obj_set_width(lv_obj_9, 229);
        lv_obj_set_height(lv_obj_9, 44);
        lv_obj_set_clickable(lv_obj_9, false);
        lv_obj_set_style_radius(lv_obj_9, 10, 0);
        lv_obj_set_style_border_width(lv_obj_9, 1, 0);
        lv_obj_set_style_bg_opa(lv_obj_9, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(lv_obj_9, lv_color_hex(0x111722), 0);
        lv_obj_set_style_border_color(lv_obj_9, lv_color_hex(0x1e293b), 0);
        lv_obj_set_style_pad_top(lv_obj_9, -1, 0);
        lv_obj_set_style_pad_bottom(lv_obj_9, -1, 0);
        lv_obj_set_style_pad_left(lv_obj_9, -1, 0);
        lv_obj_set_style_pad_right(lv_obj_9, -1, 0);
        lv_obj_set_scrollable(lv_obj_9, false);
        lv_obj_set_style_shadow_width(lv_obj_9, 0, 0);
        lv_obj_set_style_outline_width(lv_obj_9, 0, 0);
        lv_obj_t * lv_label_13 = lv_label_create(lv_obj_9);
        lv_obj_set_x(lv_label_13, 12);
        lv_obj_set_y(lv_label_13, 7);
        lv_obj_set_width(lv_label_13, 97);
        lv_obj_set_height(lv_label_13, 13);
        lv_obj_set_clickable(lv_label_13, false);
        lv_obj_set_style_text_font(lv_label_13, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_13, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_scrollable(lv_label_13, false);
        lv_label_set_text(lv_label_13, "DUAL-OTA SLOT");
        lv_label_set_long_mode(lv_label_13, LV_LABEL_LONG_MODE_WRAP);
        lv_obj_set_style_text_align(lv_label_13, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_letter_space(lv_label_13, 1, 0);

        lv_obj_t * lv_label_14 = lv_label_create(lv_obj_9);
        lv_obj_set_x(lv_label_14, 12);
        lv_obj_set_y(lv_label_14, 24);
        lv_obj_set_width(lv_label_14, 205);
        lv_obj_set_height(lv_label_14, 13);
        lv_obj_set_clickable(lv_label_14, false);
        lv_obj_set_style_text_font(lv_label_14, data_font, 0);
        lv_obj_set_style_text_color(lv_label_14, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_scrollable(lv_label_14, false);
        lv_label_set_text(lv_label_14, slot_text);
        lv_label_set_long_mode(lv_label_14, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_14, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * forget = lv_button_create(lv_obj_0);
        lv_obj_set_x(forget, 8);
        lv_obj_set_y(forget, 268);
        lv_obj_set_width(forget, 148);
        lv_obj_set_height(forget, 40);
        lv_obj_set_clickable(forget, true);
        lv_obj_set_style_radius(forget, 10, 0);
        lv_obj_set_style_border_width(forget, 1, 0);
        lv_obj_set_style_bg_opa(forget, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(forget, lv_color_hex(0x172030), 0);
        lv_obj_set_style_border_color(forget, lv_color_hex(0x1e293b), 0);
        lv_obj_set_style_pad_top(forget, -1, 0);
        lv_obj_set_style_pad_bottom(forget, -1, 0);
        lv_obj_set_style_pad_left(forget, -1, 0);
        lv_obj_set_style_pad_right(forget, -1, 0);
        lv_obj_set_scrollable(forget, false);
        lv_obj_set_name(forget, "forget");
        lv_obj_set_style_bg_opa(forget, (255 * 70 / 100), LV_STATE_PRESSED);
        lv_obj_set_style_shadow_width(forget, 0, 0);
        lv_obj_set_style_outline_width(forget, 0, 0);
        lv_obj_t * lv_label_15 = lv_label_create(forget);
        lv_obj_set_x(lv_label_15, 0);
        lv_obj_set_y(lv_label_15, 13);
        lv_obj_set_width(lv_label_15, 148);
        lv_obj_set_height(lv_label_15, 13);
        lv_obj_set_clickable(lv_label_15, false);
        lv_obj_set_style_text_font(lv_label_15, data_font, 0);
        lv_obj_set_style_text_color(lv_label_15, lv_color_hex(0xcbd5e1), 0);
        lv_obj_set_scrollable(lv_label_15, false);
        lv_label_set_text(lv_label_15, "FORGET");
        lv_label_set_long_mode(lv_label_15, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_15, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_letter_space(lv_label_15, 1, 0);

        lv_obj_t * ota = lv_button_create(lv_obj_0);
        lv_obj_set_x(ota, 166);
        lv_obj_set_y(ota, 268);
        lv_obj_set_width(ota, 148);
        lv_obj_set_height(ota, 40);
        lv_obj_set_clickable(ota, true);
        lv_obj_set_style_radius(ota, 10, 0);
        lv_obj_set_style_border_width(ota, 1, 0);
        lv_obj_set_style_bg_opa(ota, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(ota, ota_armed ? lv_color_hex(0x2b2309) : lv_color_hex(0x172030), 0);
        lv_obj_set_style_border_color(ota, lv_color_hex(0x1e293b), 0);
        lv_obj_set_style_pad_top(ota, -1, 0);
        lv_obj_set_style_pad_bottom(ota, -1, 0);
        lv_obj_set_style_pad_left(ota, -1, 0);
        lv_obj_set_style_pad_right(ota, -1, 0);
        lv_obj_set_scrollable(ota, false);
        lv_obj_set_name(ota, "ota");
        lv_obj_set_style_bg_opa(ota, (255 * 70 / 100), LV_STATE_PRESSED);
        lv_obj_set_style_shadow_width(ota, 0, 0);
        lv_obj_set_style_outline_width(ota, 0, 0);
        lv_obj_t * lv_label_16 = lv_label_create(ota);
        lv_obj_set_x(lv_label_16, 0);
        lv_obj_set_y(lv_label_16, 13);
        lv_obj_set_width(lv_label_16, 148);
        lv_obj_set_height(lv_label_16, 13);
        lv_obj_set_clickable(lv_label_16, false);
        lv_obj_set_style_text_font(lv_label_16, data_font, 0);
        lv_obj_set_style_text_color(lv_label_16, ota_armed ? lv_color_hex(0xfacc15) : lv_color_hex(0xcbd5e1), 0);
        lv_obj_set_scrollable(lv_label_16, false);
        lv_label_set_text(lv_label_16, ota_button_text);
        lv_label_set_long_mode(lv_label_16, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_16, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_letter_space(lv_label_16, 1, 0);

        lv_obj_t * done = lv_button_create(lv_obj_0);
        lv_obj_set_x(done, 324);
        lv_obj_set_y(done, 268);
        lv_obj_set_width(done, 148);
        lv_obj_set_height(done, 40);
        lv_obj_set_clickable(done, true);
        lv_obj_set_style_radius(done, 10, 0);
        lv_obj_set_style_border_width(done, 0, 0);
        lv_obj_set_style_bg_opa(done, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(done, lv_color_hex(0x0ea5e9), 0);
        lv_obj_set_style_pad_top(done, 0, 0);
        lv_obj_set_style_pad_bottom(done, 0, 0);
        lv_obj_set_style_pad_left(done, 0, 0);
        lv_obj_set_style_pad_right(done, 0, 0);
        lv_obj_set_scrollable(done, false);
        lv_obj_set_name(done, "done");
        lv_obj_set_style_bg_opa(done, (255 * 70 / 100), LV_STATE_PRESSED);
        lv_obj_set_style_shadow_width(done, 0, 0);
        lv_obj_set_style_outline_width(done, 0, 0);
        lv_obj_t * lv_label_17 = lv_label_create(done);
        lv_obj_set_x(lv_label_17, 0);
        lv_obj_set_y(lv_label_17, 13);
        lv_obj_set_width(lv_label_17, 148);
        lv_obj_set_height(lv_label_17, 13);
        lv_obj_set_clickable(lv_label_17, false);
        lv_obj_set_style_text_font(lv_label_17, data_font, 0);
        lv_obj_set_style_text_color(lv_label_17, lv_color_hex(0xffffff), 0);
        lv_obj_set_scrollable(lv_label_17, false);
        lv_label_set_text(lv_label_17, "DONE");
        lv_label_set_long_mode(lv_label_17, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_17, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_letter_space(lv_label_17, 1, 0);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

lv_obj_t * rcc_wifi_info_get_done(lv_obj_t *parent)
{
    return lv_obj_find_by_name(parent, "done");
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

