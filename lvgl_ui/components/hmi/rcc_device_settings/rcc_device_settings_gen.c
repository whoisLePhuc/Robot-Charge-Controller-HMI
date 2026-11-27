/**
 * @file rcc_device_settings_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "rcc_device_settings_gen.h"
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

lv_obj_t * rcc_device_settings_create(lv_obj_t * parent, int32_t card_height, lv_font_t * caption_font, lv_font_t * body_font, lv_font_t * value_font, const void * wifi_icon, const void * chevron_icon)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "rcc_device_settings_#");
        lv_obj_set_x(lv_obj_0, 0);
        lv_obj_set_y(lv_obj_0, 68);
        lv_obj_set_width(lv_obj_0, 408);
        lv_obj_set_height(lv_obj_0, card_height);
        lv_obj_set_clickable(lv_obj_0, false);
        lv_obj_set_scrollable(lv_obj_0, false);
        lv_obj_set_style_bg_color(lv_obj_0, lv_color_hex(0x000000), 0);
        lv_obj_set_style_bg_opa(lv_obj_0, (255 * 0 / 100), 0);
        lv_obj_set_style_radius(lv_obj_0, 0, 0);
        lv_obj_set_style_border_width(lv_obj_0, 0, 0);
        lv_obj_set_style_outline_width(lv_obj_0, 0, 0);
        lv_obj_set_style_shadow_width(lv_obj_0, 0, 0);
        lv_obj_set_style_pad_all(lv_obj_0, 0, 0);

        lv_obj_t * lv_obj_1 = lv_obj_create(lv_obj_0);
        lv_obj_set_x(lv_obj_1, 0);
        lv_obj_set_y(lv_obj_1, 0);
        lv_obj_set_width(lv_obj_1, 200);
        lv_obj_set_height(lv_obj_1, card_height);
        lv_obj_set_clickable(lv_obj_1, false);
        lv_obj_set_scrollable(lv_obj_1, false);
        lv_obj_set_style_bg_color(lv_obj_1, lv_color_hex(0x111722), 0);
        lv_obj_set_style_bg_opa(lv_obj_1, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(lv_obj_1, 10, 0);
        lv_obj_set_style_border_width(lv_obj_1, 1, 0);
        lv_obj_set_style_outline_width(lv_obj_1, 0, 0);
        lv_obj_set_style_shadow_width(lv_obj_1, 0, 0);
        lv_obj_set_style_pad_all(lv_obj_1, -1, 0);
        lv_obj_set_style_border_color(lv_obj_1, lv_color_hex(0x1e293b), 0);
        lv_obj_set_style_border_opa(lv_obj_1, (255 * 100 / 100), 0);
        lv_obj_t * lv_label_0 = lv_label_create(lv_obj_1);
        lv_obj_set_x(lv_label_0, 13);
        lv_obj_set_y(lv_label_0, 8);
        lv_label_set_text(lv_label_0, "DISPLAY & POWER");
        lv_obj_set_style_text_font(lv_label_0, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_0, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_clickable(lv_label_0, false);
        lv_obj_set_scrollable(lv_label_0, false);
        lv_obj_set_style_text_letter_space(lv_label_0, 1, 0);

        lv_obj_t * device_backlight = lv_label_create(lv_obj_1);
        lv_obj_set_x(device_backlight, 13);
        lv_obj_set_y(device_backlight, 24);
        lv_label_set_text(device_backlight, "Backlight: 100%");
        lv_obj_set_style_text_font(device_backlight, body_font, 0);
        lv_obj_set_style_text_color(device_backlight, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_clickable(device_backlight, false);
        lv_obj_set_scrollable(device_backlight, false);
        lv_obj_set_width(device_backlight, 174);
        lv_obj_set_height(device_backlight, 15);
        lv_label_set_long_mode(device_backlight, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(device_backlight, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_name(device_backlight, "device_backlight");

        lv_obj_t * device_brightness0 = lv_button_create(lv_obj_1);
        lv_obj_set_x(device_brightness0, 13);
        lv_obj_set_y(device_brightness0, 42);
        lv_obj_set_width(device_brightness0, 40);
        lv_obj_set_height(device_brightness0, 22);
        lv_obj_set_clickable(device_brightness0, true);
        lv_obj_set_scrollable(device_brightness0, false);
        lv_obj_set_style_bg_color(device_brightness0, lv_color_hex(0x172030), 0);
        lv_obj_set_style_bg_opa(device_brightness0, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(device_brightness0, 6, 0);
        lv_obj_set_style_border_width(device_brightness0, 0, 0);
        lv_obj_set_style_outline_width(device_brightness0, 0, 0);
        lv_obj_set_style_shadow_width(device_brightness0, 0, 0);
        lv_obj_set_style_pad_all(device_brightness0, 0, 0);
        lv_obj_set_name(device_brightness0, "device_brightness0");
        lv_obj_set_style_bg_opa(device_brightness0, (255 * 70 / 100), LV_STATE_PRESSED);
        lv_obj_t * lv_label_1 = lv_label_create(device_brightness0);
        lv_obj_set_x(lv_label_1, 0);
        lv_obj_set_y(lv_label_1, 4);
        lv_label_set_text(lv_label_1, "25%");
        lv_obj_set_style_text_font(lv_label_1, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_1, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_clickable(lv_label_1, false);
        lv_obj_set_scrollable(lv_label_1, false);
        lv_obj_set_width(lv_label_1, 40);
        lv_obj_set_height(lv_label_1, 13);
        lv_label_set_long_mode(lv_label_1, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_1, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * device_brightness1 = lv_button_create(lv_obj_1);
        lv_obj_set_x(device_brightness1, 57);
        lv_obj_set_y(device_brightness1, 42);
        lv_obj_set_width(device_brightness1, 40);
        lv_obj_set_height(device_brightness1, 22);
        lv_obj_set_clickable(device_brightness1, true);
        lv_obj_set_scrollable(device_brightness1, false);
        lv_obj_set_style_bg_color(device_brightness1, lv_color_hex(0x172030), 0);
        lv_obj_set_style_bg_opa(device_brightness1, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(device_brightness1, 6, 0);
        lv_obj_set_style_border_width(device_brightness1, 0, 0);
        lv_obj_set_style_outline_width(device_brightness1, 0, 0);
        lv_obj_set_style_shadow_width(device_brightness1, 0, 0);
        lv_obj_set_style_pad_all(device_brightness1, 0, 0);
        lv_obj_set_name(device_brightness1, "device_brightness1");
        lv_obj_set_style_bg_opa(device_brightness1, (255 * 70 / 100), LV_STATE_PRESSED);
        lv_obj_t * lv_label_2 = lv_label_create(device_brightness1);
        lv_obj_set_x(lv_label_2, 0);
        lv_obj_set_y(lv_label_2, 4);
        lv_label_set_text(lv_label_2, "50%");
        lv_obj_set_style_text_font(lv_label_2, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_2, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_clickable(lv_label_2, false);
        lv_obj_set_scrollable(lv_label_2, false);
        lv_obj_set_width(lv_label_2, 40);
        lv_obj_set_height(lv_label_2, 13);
        lv_label_set_long_mode(lv_label_2, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_2, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * device_brightness2 = lv_button_create(lv_obj_1);
        lv_obj_set_x(device_brightness2, 101);
        lv_obj_set_y(device_brightness2, 42);
        lv_obj_set_width(device_brightness2, 40);
        lv_obj_set_height(device_brightness2, 22);
        lv_obj_set_clickable(device_brightness2, true);
        lv_obj_set_scrollable(device_brightness2, false);
        lv_obj_set_style_bg_color(device_brightness2, lv_color_hex(0x172030), 0);
        lv_obj_set_style_bg_opa(device_brightness2, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(device_brightness2, 6, 0);
        lv_obj_set_style_border_width(device_brightness2, 0, 0);
        lv_obj_set_style_outline_width(device_brightness2, 0, 0);
        lv_obj_set_style_shadow_width(device_brightness2, 0, 0);
        lv_obj_set_style_pad_all(device_brightness2, 0, 0);
        lv_obj_set_name(device_brightness2, "device_brightness2");
        lv_obj_set_style_bg_opa(device_brightness2, (255 * 70 / 100), LV_STATE_PRESSED);
        lv_obj_t * lv_label_3 = lv_label_create(device_brightness2);
        lv_obj_set_x(lv_label_3, 0);
        lv_obj_set_y(lv_label_3, 4);
        lv_label_set_text(lv_label_3, "75%");
        lv_obj_set_style_text_font(lv_label_3, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_3, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_clickable(lv_label_3, false);
        lv_obj_set_scrollable(lv_label_3, false);
        lv_obj_set_width(lv_label_3, 40);
        lv_obj_set_height(lv_label_3, 13);
        lv_label_set_long_mode(lv_label_3, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_3, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * device_brightness3 = lv_button_create(lv_obj_1);
        lv_obj_set_x(device_brightness3, 145);
        lv_obj_set_y(device_brightness3, 42);
        lv_obj_set_width(device_brightness3, 40);
        lv_obj_set_height(device_brightness3, 22);
        lv_obj_set_clickable(device_brightness3, true);
        lv_obj_set_scrollable(device_brightness3, false);
        lv_obj_set_style_bg_color(device_brightness3, lv_color_hex(0x0c2538), 0);
        lv_obj_set_style_bg_opa(device_brightness3, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(device_brightness3, 6, 0);
        lv_obj_set_style_border_width(device_brightness3, 1, 0);
        lv_obj_set_style_outline_width(device_brightness3, 0, 0);
        lv_obj_set_style_shadow_width(device_brightness3, 0, 0);
        lv_obj_set_style_pad_all(device_brightness3, 0, 0);
        lv_obj_set_name(device_brightness3, "device_brightness3");
        lv_obj_set_style_bg_opa(device_brightness3, (255 * 70 / 100), LV_STATE_PRESSED);
        lv_obj_set_style_border_color(device_brightness3, lv_color_hex(0x38bdf8), 0);
        lv_obj_t * lv_label_4 = lv_label_create(device_brightness3);
        lv_obj_set_x(lv_label_4, 0);
        lv_obj_set_y(lv_label_4, 4);
        lv_label_set_text(lv_label_4, "100%");
        lv_obj_set_style_text_font(lv_label_4, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_4, lv_color_hex(0x38bdf8), 0);
        lv_obj_set_clickable(lv_label_4, false);
        lv_obj_set_scrollable(lv_label_4, false);
        lv_obj_set_width(lv_label_4, 40);
        lv_obj_set_height(lv_label_4, 13);
        lv_label_set_long_mode(lv_label_4, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_4, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * device_sleep = lv_label_create(lv_obj_1);
        lv_obj_set_x(device_sleep, 13);
        lv_obj_set_y(device_sleep, 70);
        lv_label_set_text(device_sleep, "Sleep timeout: 3 min");
        lv_obj_set_style_text_font(device_sleep, body_font, 0);
        lv_obj_set_style_text_color(device_sleep, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_clickable(device_sleep, false);
        lv_obj_set_scrollable(device_sleep, false);
        lv_obj_set_width(device_sleep, 174);
        lv_obj_set_height(device_sleep, 15);
        lv_label_set_long_mode(device_sleep, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(device_sleep, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_name(device_sleep, "device_sleep");

        lv_obj_t * device_timeout0 = lv_button_create(lv_obj_1);
        lv_obj_set_x(device_timeout0, 13);
        lv_obj_set_y(device_timeout0, 88);
        lv_obj_set_width(device_timeout0, 40);
        lv_obj_set_height(device_timeout0, 22);
        lv_obj_set_clickable(device_timeout0, true);
        lv_obj_set_scrollable(device_timeout0, false);
        lv_obj_set_style_bg_color(device_timeout0, lv_color_hex(0x172030), 0);
        lv_obj_set_style_bg_opa(device_timeout0, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(device_timeout0, 6, 0);
        lv_obj_set_style_border_width(device_timeout0, 0, 0);
        lv_obj_set_style_outline_width(device_timeout0, 0, 0);
        lv_obj_set_style_shadow_width(device_timeout0, 0, 0);
        lv_obj_set_style_pad_all(device_timeout0, 0, 0);
        lv_obj_set_name(device_timeout0, "device_timeout0");
        lv_obj_set_style_bg_opa(device_timeout0, (255 * 70 / 100), LV_STATE_PRESSED);
        lv_obj_t * lv_label_5 = lv_label_create(device_timeout0);
        lv_obj_set_x(lv_label_5, 0);
        lv_obj_set_y(lv_label_5, 4);
        lv_label_set_text(lv_label_5, "OFF");
        lv_obj_set_style_text_font(lv_label_5, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_5, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_clickable(lv_label_5, false);
        lv_obj_set_scrollable(lv_label_5, false);
        lv_obj_set_width(lv_label_5, 40);
        lv_obj_set_height(lv_label_5, 13);
        lv_label_set_long_mode(lv_label_5, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_5, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * device_timeout1 = lv_button_create(lv_obj_1);
        lv_obj_set_x(device_timeout1, 57);
        lv_obj_set_y(device_timeout1, 88);
        lv_obj_set_width(device_timeout1, 40);
        lv_obj_set_height(device_timeout1, 22);
        lv_obj_set_clickable(device_timeout1, true);
        lv_obj_set_scrollable(device_timeout1, false);
        lv_obj_set_style_bg_color(device_timeout1, lv_color_hex(0x172030), 0);
        lv_obj_set_style_bg_opa(device_timeout1, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(device_timeout1, 6, 0);
        lv_obj_set_style_border_width(device_timeout1, 0, 0);
        lv_obj_set_style_outline_width(device_timeout1, 0, 0);
        lv_obj_set_style_shadow_width(device_timeout1, 0, 0);
        lv_obj_set_style_pad_all(device_timeout1, 0, 0);
        lv_obj_set_name(device_timeout1, "device_timeout1");
        lv_obj_set_style_bg_opa(device_timeout1, (255 * 70 / 100), LV_STATE_PRESSED);
        lv_obj_t * lv_label_6 = lv_label_create(device_timeout1);
        lv_obj_set_x(lv_label_6, 0);
        lv_obj_set_y(lv_label_6, 4);
        lv_label_set_text(lv_label_6, "1m");
        lv_obj_set_style_text_font(lv_label_6, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_6, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_clickable(lv_label_6, false);
        lv_obj_set_scrollable(lv_label_6, false);
        lv_obj_set_width(lv_label_6, 40);
        lv_obj_set_height(lv_label_6, 13);
        lv_label_set_long_mode(lv_label_6, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_6, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * device_timeout2 = lv_button_create(lv_obj_1);
        lv_obj_set_x(device_timeout2, 101);
        lv_obj_set_y(device_timeout2, 88);
        lv_obj_set_width(device_timeout2, 40);
        lv_obj_set_height(device_timeout2, 22);
        lv_obj_set_clickable(device_timeout2, true);
        lv_obj_set_scrollable(device_timeout2, false);
        lv_obj_set_style_bg_color(device_timeout2, lv_color_hex(0x0c2538), 0);
        lv_obj_set_style_bg_opa(device_timeout2, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(device_timeout2, 6, 0);
        lv_obj_set_style_border_width(device_timeout2, 1, 0);
        lv_obj_set_style_outline_width(device_timeout2, 0, 0);
        lv_obj_set_style_shadow_width(device_timeout2, 0, 0);
        lv_obj_set_style_pad_all(device_timeout2, 0, 0);
        lv_obj_set_name(device_timeout2, "device_timeout2");
        lv_obj_set_style_bg_opa(device_timeout2, (255 * 70 / 100), LV_STATE_PRESSED);
        lv_obj_set_style_border_color(device_timeout2, lv_color_hex(0x38bdf8), 0);
        lv_obj_t * lv_label_7 = lv_label_create(device_timeout2);
        lv_obj_set_x(lv_label_7, 0);
        lv_obj_set_y(lv_label_7, 4);
        lv_label_set_text(lv_label_7, "3m");
        lv_obj_set_style_text_font(lv_label_7, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_7, lv_color_hex(0x38bdf8), 0);
        lv_obj_set_clickable(lv_label_7, false);
        lv_obj_set_scrollable(lv_label_7, false);
        lv_obj_set_width(lv_label_7, 40);
        lv_obj_set_height(lv_label_7, 13);
        lv_label_set_long_mode(lv_label_7, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_7, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * device_timeout3 = lv_button_create(lv_obj_1);
        lv_obj_set_x(device_timeout3, 145);
        lv_obj_set_y(device_timeout3, 88);
        lv_obj_set_width(device_timeout3, 40);
        lv_obj_set_height(device_timeout3, 22);
        lv_obj_set_clickable(device_timeout3, true);
        lv_obj_set_scrollable(device_timeout3, false);
        lv_obj_set_style_bg_color(device_timeout3, lv_color_hex(0x172030), 0);
        lv_obj_set_style_bg_opa(device_timeout3, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(device_timeout3, 6, 0);
        lv_obj_set_style_border_width(device_timeout3, 0, 0);
        lv_obj_set_style_outline_width(device_timeout3, 0, 0);
        lv_obj_set_style_shadow_width(device_timeout3, 0, 0);
        lv_obj_set_style_pad_all(device_timeout3, 0, 0);
        lv_obj_set_name(device_timeout3, "device_timeout3");
        lv_obj_set_style_bg_opa(device_timeout3, (255 * 70 / 100), LV_STATE_PRESSED);
        lv_obj_t * lv_label_8 = lv_label_create(device_timeout3);
        lv_obj_set_x(lv_label_8, 0);
        lv_obj_set_y(lv_label_8, 4);
        lv_label_set_text(lv_label_8, "5m");
        lv_obj_set_style_text_font(lv_label_8, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_8, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_clickable(lv_label_8, false);
        lv_obj_set_scrollable(lv_label_8, false);
        lv_obj_set_width(lv_label_8, 40);
        lv_obj_set_height(lv_label_8, 13);
        lv_label_set_long_mode(lv_label_8, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_8, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * lv_label_9 = lv_label_create(lv_obj_1);
        lv_obj_set_x(lv_label_9, 13);
        lv_obj_set_y(lv_label_9, 119);
        lv_label_set_text(lv_label_9, "Idle dim level");
        lv_obj_set_style_text_font(lv_label_9, body_font, 0);
        lv_obj_set_style_text_color(lv_label_9, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_clickable(lv_label_9, false);
        lv_obj_set_scrollable(lv_label_9, false);
        lv_obj_set_width(lv_label_9, 174);
        lv_obj_set_height(lv_label_9, 15);
        lv_label_set_long_mode(lv_label_9, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_9, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * device_dim = lv_label_create(lv_obj_1);
        lv_obj_set_x(device_dim, 16);
        lv_obj_set_y(device_dim, 120);
        lv_label_set_text(device_dim, "15%");
        lv_obj_set_style_text_font(device_dim, value_font, 0);
        lv_obj_set_style_text_color(device_dim, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_clickable(device_dim, false);
        lv_obj_set_scrollable(device_dim, false);
        lv_obj_set_width(device_dim, 172);
        lv_obj_set_height(device_dim, 13);
        lv_label_set_long_mode(device_dim, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(device_dim, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_set_name(device_dim, "device_dim");

        lv_obj_t * lv_label_10 = lv_label_create(lv_obj_1);
        lv_obj_set_x(lv_label_10, 13);
        lv_obj_set_y(lv_label_10, 137);
        lv_label_set_text(lv_label_10, "Wake method");
        lv_obj_set_style_text_font(lv_label_10, body_font, 0);
        lv_obj_set_style_text_color(lv_label_10, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_clickable(lv_label_10, false);
        lv_obj_set_scrollable(lv_label_10, false);
        lv_obj_set_width(lv_label_10, 174);
        lv_obj_set_height(lv_label_10, 15);
        lv_label_set_long_mode(lv_label_10, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_10, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * device_wake = lv_label_create(lv_obj_1);
        lv_obj_set_x(device_wake, 16);
        lv_obj_set_y(device_wake, 138);
        lv_label_set_text(device_wake, "Touch");
        lv_obj_set_style_text_font(device_wake, value_font, 0);
        lv_obj_set_style_text_color(device_wake, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_clickable(device_wake, false);
        lv_obj_set_scrollable(device_wake, false);
        lv_obj_set_width(device_wake, 172);
        lv_obj_set_height(device_wake, 13);
        lv_label_set_long_mode(device_wake, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(device_wake, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_set_name(device_wake, "device_wake");

        lv_obj_t * lv_obj_2 = lv_obj_create(lv_obj_0);
        lv_obj_set_x(lv_obj_2, 208);
        lv_obj_set_y(lv_obj_2, 0);
        lv_obj_set_width(lv_obj_2, 200);
        lv_obj_set_height(lv_obj_2, card_height);
        lv_obj_set_clickable(lv_obj_2, false);
        lv_obj_set_scrollable(lv_obj_2, false);
        lv_obj_set_style_bg_color(lv_obj_2, lv_color_hex(0x111722), 0);
        lv_obj_set_style_bg_opa(lv_obj_2, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(lv_obj_2, 10, 0);
        lv_obj_set_style_border_width(lv_obj_2, 1, 0);
        lv_obj_set_style_outline_width(lv_obj_2, 0, 0);
        lv_obj_set_style_shadow_width(lv_obj_2, 0, 0);
        lv_obj_set_style_pad_all(lv_obj_2, -1, 0);
        lv_obj_set_style_border_color(lv_obj_2, lv_color_hex(0x1e293b), 0);
        lv_obj_set_style_border_opa(lv_obj_2, (255 * 100 / 100), 0);
        lv_obj_t * lv_label_11 = lv_label_create(lv_obj_2);
        lv_obj_set_x(lv_label_11, 13);
        lv_obj_set_y(lv_label_11, 8);
        lv_label_set_text(lv_label_11, "SYSTEM");
        lv_obj_set_style_text_font(lv_label_11, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_11, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_clickable(lv_label_11, false);
        lv_obj_set_scrollable(lv_label_11, false);
        lv_obj_set_style_text_letter_space(lv_label_11, 1, 0);

        lv_obj_t * device_system_box = lv_obj_create(lv_obj_2);
        lv_obj_set_x(device_system_box, 130);
        lv_obj_set_y(device_system_box, 7);
        lv_obj_set_width(device_system_box, 57);
        lv_obj_set_height(device_system_box, 17);
        lv_obj_set_clickable(device_system_box, false);
        lv_obj_set_scrollable(device_system_box, false);
        lv_obj_set_style_bg_color(device_system_box, lv_color_hex(0x0a2617), 0);
        lv_obj_set_style_bg_opa(device_system_box, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(device_system_box, 6, 0);
        lv_obj_set_style_border_width(device_system_box, 0, 0);
        lv_obj_set_style_outline_width(device_system_box, 0, 0);
        lv_obj_set_style_shadow_width(device_system_box, 0, 0);
        lv_obj_set_style_pad_all(device_system_box, 0, 0);
        lv_obj_set_name(device_system_box, "device_system_box");

        lv_obj_t * device_system_text = lv_label_create(lv_obj_2);
        lv_obj_set_x(device_system_text, 137);
        lv_obj_set_y(device_system_text, 9);
        lv_label_set_text(device_system_text, "ACTIVE");
        lv_obj_set_style_text_font(device_system_text, caption_font, 0);
        lv_obj_set_style_text_color(device_system_text, lv_color_hex(0x22c55e), 0);
        lv_obj_set_clickable(device_system_text, false);
        lv_obj_set_scrollable(device_system_text, false);
        lv_obj_set_name(device_system_text, "device_system_text");

        lv_obj_t * lv_label_12 = lv_label_create(lv_obj_2);
        lv_obj_set_x(lv_label_12, 13);
        lv_obj_set_y(lv_label_12, 24);
        lv_label_set_text(lv_label_12, "Language");
        lv_obj_set_style_text_font(lv_label_12, body_font, 0);
        lv_obj_set_style_text_color(lv_label_12, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_clickable(lv_label_12, false);
        lv_obj_set_scrollable(lv_label_12, false);
        lv_obj_set_width(lv_label_12, 174);
        lv_obj_set_height(lv_label_12, 15);
        lv_label_set_long_mode(lv_label_12, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_12, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * device_language0 = lv_button_create(lv_obj_2);
        lv_obj_set_x(device_language0, 13);
        lv_obj_set_y(device_language0, 42);
        lv_obj_set_width(device_language0, 84);
        lv_obj_set_height(device_language0, 22);
        lv_obj_set_clickable(device_language0, true);
        lv_obj_set_scrollable(device_language0, false);
        lv_obj_set_style_bg_color(device_language0, lv_color_hex(0x0c2538), 0);
        lv_obj_set_style_bg_opa(device_language0, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(device_language0, 6, 0);
        lv_obj_set_style_border_width(device_language0, 1, 0);
        lv_obj_set_style_outline_width(device_language0, 0, 0);
        lv_obj_set_style_shadow_width(device_language0, 0, 0);
        lv_obj_set_style_pad_all(device_language0, 0, 0);
        lv_obj_set_name(device_language0, "device_language0");
        lv_obj_set_style_bg_opa(device_language0, (255 * 70 / 100), LV_STATE_PRESSED);
        lv_obj_set_style_border_color(device_language0, lv_color_hex(0x38bdf8), 0);
        lv_obj_t * lv_label_13 = lv_label_create(device_language0);
        lv_obj_set_x(lv_label_13, 0);
        lv_obj_set_y(lv_label_13, 4);
        lv_label_set_text(lv_label_13, "English");
        lv_obj_set_style_text_font(lv_label_13, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_13, lv_color_hex(0x38bdf8), 0);
        lv_obj_set_clickable(lv_label_13, false);
        lv_obj_set_scrollable(lv_label_13, false);
        lv_obj_set_width(lv_label_13, 84);
        lv_obj_set_height(lv_label_13, 13);
        lv_label_set_long_mode(lv_label_13, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_13, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * device_language1 = lv_button_create(lv_obj_2);
        lv_obj_set_x(device_language1, 103);
        lv_obj_set_y(device_language1, 42);
        lv_obj_set_width(device_language1, 84);
        lv_obj_set_height(device_language1, 22);
        lv_obj_set_clickable(device_language1, true);
        lv_obj_set_scrollable(device_language1, false);
        lv_obj_set_style_bg_color(device_language1, lv_color_hex(0x172030), 0);
        lv_obj_set_style_bg_opa(device_language1, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(device_language1, 6, 0);
        lv_obj_set_style_border_width(device_language1, 0, 0);
        lv_obj_set_style_outline_width(device_language1, 0, 0);
        lv_obj_set_style_shadow_width(device_language1, 0, 0);
        lv_obj_set_style_pad_all(device_language1, 0, 0);
        lv_obj_set_name(device_language1, "device_language1");
        lv_obj_set_style_bg_opa(device_language1, (255 * 70 / 100), LV_STATE_PRESSED);
        lv_obj_t * lv_label_14 = lv_label_create(device_language1);
        lv_obj_set_x(lv_label_14, 0);
        lv_obj_set_y(lv_label_14, 4);
        lv_label_set_text(lv_label_14, "Tiếng Việt");
        lv_obj_set_style_text_font(lv_label_14, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_14, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_clickable(lv_label_14, false);
        lv_obj_set_scrollable(lv_label_14, false);
        lv_obj_set_width(lv_label_14, 84);
        lv_obj_set_height(lv_label_14, 13);
        lv_label_set_long_mode(lv_label_14, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_14, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * lv_label_15 = lv_label_create(lv_obj_2);
        lv_obj_set_x(lv_label_15, 13);
        lv_obj_set_y(lv_label_15, 75);
        lv_label_set_text(lv_label_15, "Task watchdog");
        lv_obj_set_style_text_font(lv_label_15, body_font, 0);
        lv_obj_set_style_text_color(lv_label_15, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_clickable(lv_label_15, false);
        lv_obj_set_scrollable(lv_label_15, false);
        lv_obj_set_width(lv_label_15, 174);
        lv_obj_set_height(lv_label_15, 15);
        lv_label_set_long_mode(lv_label_15, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_15, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * device_watchdog = lv_label_create(lv_obj_2);
        lv_obj_set_x(device_watchdog, 16);
        lv_obj_set_y(device_watchdog, 76);
        lv_label_set_text(device_watchdog, "5.0 s");
        lv_obj_set_style_text_font(device_watchdog, value_font, 0);
        lv_obj_set_style_text_color(device_watchdog, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_clickable(device_watchdog, false);
        lv_obj_set_scrollable(device_watchdog, false);
        lv_obj_set_width(device_watchdog, 172);
        lv_obj_set_height(device_watchdog, 13);
        lv_label_set_long_mode(device_watchdog, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(device_watchdog, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_set_name(device_watchdog, "device_watchdog");

        lv_obj_t * lv_label_16 = lv_label_create(lv_obj_2);
        lv_obj_set_x(lv_label_16, 13);
        lv_obj_set_y(lv_label_16, 93);
        lv_label_set_text(lv_label_16, "LCD driver");
        lv_obj_set_style_text_font(lv_label_16, body_font, 0);
        lv_obj_set_style_text_color(lv_label_16, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_clickable(lv_label_16, false);
        lv_obj_set_scrollable(lv_label_16, false);
        lv_obj_set_width(lv_label_16, 174);
        lv_obj_set_height(lv_label_16, 15);
        lv_label_set_long_mode(lv_label_16, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_16, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * device_lcd = lv_label_create(lv_obj_2);
        lv_obj_set_x(device_lcd, 16);
        lv_obj_set_y(device_lcd, 94);
        lv_label_set_text(device_lcd, "ST77922 QSPI");
        lv_obj_set_style_text_font(device_lcd, value_font, 0);
        lv_obj_set_style_text_color(device_lcd, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_clickable(device_lcd, false);
        lv_obj_set_scrollable(device_lcd, false);
        lv_obj_set_width(device_lcd, 172);
        lv_obj_set_height(device_lcd, 13);
        lv_label_set_long_mode(device_lcd, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(device_lcd, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_set_name(device_lcd, "device_lcd");

        lv_obj_t * lv_label_17 = lv_label_create(lv_obj_2);
        lv_obj_set_x(lv_label_17, 13);
        lv_obj_set_y(lv_label_17, 111);
        lv_label_set_text(lv_label_17, "Firmware");
        lv_obj_set_style_text_font(lv_label_17, body_font, 0);
        lv_obj_set_style_text_color(lv_label_17, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_clickable(lv_label_17, false);
        lv_obj_set_scrollable(lv_label_17, false);
        lv_obj_set_width(lv_label_17, 174);
        lv_obj_set_height(lv_label_17, 15);
        lv_label_set_long_mode(lv_label_17, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_17, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * device_version = lv_label_create(lv_obj_2);
        lv_obj_set_x(device_version, 16);
        lv_obj_set_y(device_version, 112);
        lv_label_set_text(device_version, "unknown");
        lv_obj_set_style_text_font(device_version, value_font, 0);
        lv_obj_set_style_text_color(device_version, lv_color_hex(0x22c55e), 0);
        lv_obj_set_clickable(device_version, false);
        lv_obj_set_scrollable(device_version, false);
        lv_obj_set_width(device_version, 172);
        lv_obj_set_height(device_version, 13);
        lv_label_set_long_mode(device_version, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(device_version, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_set_name(device_version, "device_version");

        lv_obj_t * device_wifi_button = lv_button_create(lv_obj_2);
        lv_obj_set_x(device_wifi_button, 13);
        lv_obj_set_y(device_wifi_button, 132);
        lv_obj_set_width(device_wifi_button, 174);
        lv_obj_set_height(device_wifi_button, 22);
        lv_obj_set_clickable(device_wifi_button, true);
        lv_obj_set_scrollable(device_wifi_button, false);
        lv_obj_set_style_bg_color(device_wifi_button, lv_color_hex(0x172030), 0);
        lv_obj_set_style_bg_opa(device_wifi_button, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(device_wifi_button, 6, 0);
        lv_obj_set_style_border_width(device_wifi_button, 0, 0);
        lv_obj_set_style_outline_width(device_wifi_button, 0, 0);
        lv_obj_set_style_shadow_width(device_wifi_button, 0, 0);
        lv_obj_set_style_pad_all(device_wifi_button, 0, 0);
        lv_obj_set_name(device_wifi_button, "device_wifi_button");
        lv_obj_set_style_bg_opa(device_wifi_button, (255 * 70 / 100), LV_STATE_PRESSED);
        lv_obj_t * lv_image_0 = lv_image_create(device_wifi_button);
        lv_obj_set_x(lv_image_0, 8);
        lv_obj_set_y(lv_image_0, 4);
        lv_image_set_src(lv_image_0, wifi_icon);
        lv_obj_set_style_image_recolor(lv_image_0, lv_color_hex(0x38bdf8), 0);
        lv_obj_set_style_image_recolor_opa(lv_image_0, (255 * 100 / 100), 0);
        lv_obj_set_clickable(lv_image_0, false);
        lv_obj_set_scrollable(lv_image_0, false);

        lv_obj_t * lv_label_18 = lv_label_create(device_wifi_button);
        lv_obj_set_x(lv_label_18, 28);
        lv_obj_set_y(lv_label_18, 4);
        lv_label_set_text(lv_label_18, "Wi-Fi");
        lv_obj_set_style_text_font(lv_label_18, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_18, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_clickable(lv_label_18, false);
        lv_obj_set_scrollable(lv_label_18, false);
        lv_obj_set_width(lv_label_18, 40);
        lv_obj_set_height(lv_label_18, 13);
        lv_label_set_long_mode(lv_label_18, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_18, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * device_wifi_status = lv_label_create(device_wifi_button);
        lv_obj_set_x(device_wifi_status, 66);
        lv_obj_set_y(device_wifi_status, 4);
        lv_label_set_text(device_wifi_status, "MSVN-Lab-5G");
        lv_obj_set_style_text_font(device_wifi_status, caption_font, 0);
        lv_obj_set_style_text_color(device_wifi_status, lv_color_hex(0x22c55e), 0);
        lv_obj_set_clickable(device_wifi_status, false);
        lv_obj_set_scrollable(device_wifi_status, false);
        lv_obj_set_width(device_wifi_status, 84);
        lv_obj_set_height(device_wifi_status, 13);
        lv_label_set_long_mode(device_wifi_status, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(device_wifi_status, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_set_name(device_wifi_status, "device_wifi_status");

        lv_obj_t * lv_image_1 = lv_image_create(device_wifi_button);
        lv_obj_set_x(lv_image_1, 156);
        lv_obj_set_y(lv_image_1, 6);
        lv_image_set_src(lv_image_1, chevron_icon);
        lv_obj_set_style_image_recolor(lv_image_1, lv_color_hex(0x64748b), 0);
        lv_obj_set_style_image_recolor_opa(lv_image_1, (255 * 100 / 100), 0);
        lv_obj_set_clickable(lv_image_1, false);
        lv_obj_set_scrollable(lv_image_1, false);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

lv_obj_t * rcc_device_settings_get_device_wifi_button(lv_obj_t *parent)
{
    return lv_obj_find_by_name(parent, "device_wifi_button");
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

