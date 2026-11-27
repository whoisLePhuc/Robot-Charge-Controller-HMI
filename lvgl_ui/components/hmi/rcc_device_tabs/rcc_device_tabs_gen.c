/**
 * @file rcc_device_tabs_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "rcc_device_tabs_gen.h"
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

lv_obj_t * rcc_device_tabs_create(lv_obj_t * parent, lv_font_t * caption_font)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "rcc_device_tabs_#");
        lv_obj_set_x(lv_obj_0, 0);
        lv_obj_set_y(lv_obj_0, 30);
        lv_obj_set_width(lv_obj_0, 408);
        lv_obj_set_height(lv_obj_0, 32);
        lv_obj_set_clickable(lv_obj_0, false);
        lv_obj_set_scrollable(lv_obj_0, false);
        lv_obj_set_style_bg_color(lv_obj_0, lv_color_hex(0x111722), 0);
        lv_obj_set_style_bg_opa(lv_obj_0, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(lv_obj_0, 8, 0);
        lv_obj_set_style_border_width(lv_obj_0, 1, 0);
        lv_obj_set_style_outline_width(lv_obj_0, 0, 0);
        lv_obj_set_style_shadow_width(lv_obj_0, 0, 0);
        lv_obj_set_style_pad_all(lv_obj_0, -1, 0);
        lv_obj_set_style_border_color(lv_obj_0, lv_color_hex(0x1e293b), 0);
        lv_obj_set_style_border_opa(lv_obj_0, (255 * 100 / 100), 0);

        lv_obj_t * device_tab0 = lv_button_create(lv_obj_0);
        lv_obj_set_x(device_tab0, 3);
        lv_obj_set_y(device_tab0, 3);
        lv_obj_set_width(device_tab0, 134);
        lv_obj_set_height(device_tab0, 26);
        lv_obj_set_clickable(device_tab0, true);
        lv_obj_set_scrollable(device_tab0, false);
        lv_obj_set_style_bg_color(device_tab0, lv_color_hex(0x111722), 0);
        lv_obj_set_style_bg_opa(device_tab0, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(device_tab0, 6, 0);
        lv_obj_set_style_border_width(device_tab0, 0, 0);
        lv_obj_set_style_outline_width(device_tab0, 0, 0);
        lv_obj_set_style_shadow_width(device_tab0, 0, 0);
        lv_obj_set_style_pad_all(device_tab0, 0, 0);
        lv_obj_set_name(device_tab0, "device_tab0");
        lv_obj_set_style_bg_opa(device_tab0, (255 * 70 / 100), LV_STATE_PRESSED);
        lv_obj_t * lv_label_0 = lv_label_create(device_tab0);
        lv_obj_set_x(lv_label_0, 0);
        lv_obj_set_y(lv_label_0, 6);
        lv_label_set_text(lv_label_0, "CONTROLLER");
        lv_obj_set_style_text_font(lv_label_0, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_0, lv_color_hex(0x64748b), 0);
        lv_obj_set_clickable(lv_label_0, false);
        lv_obj_set_scrollable(lv_label_0, false);
        lv_obj_set_width(lv_label_0, 134);
        lv_obj_set_height(lv_label_0, 13);
        lv_label_set_long_mode(lv_label_0, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_letter_space(lv_label_0, 1, 0);

        lv_obj_t * device_tab1 = lv_button_create(lv_obj_0);
        lv_obj_set_x(device_tab1, 137);
        lv_obj_set_y(device_tab1, 3);
        lv_obj_set_width(device_tab1, 134);
        lv_obj_set_height(device_tab1, 26);
        lv_obj_set_clickable(device_tab1, true);
        lv_obj_set_scrollable(device_tab1, false);
        lv_obj_set_style_bg_color(device_tab1, lv_color_hex(0x111722), 0);
        lv_obj_set_style_bg_opa(device_tab1, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(device_tab1, 6, 0);
        lv_obj_set_style_border_width(device_tab1, 0, 0);
        lv_obj_set_style_outline_width(device_tab1, 0, 0);
        lv_obj_set_style_shadow_width(device_tab1, 0, 0);
        lv_obj_set_style_pad_all(device_tab1, 0, 0);
        lv_obj_set_name(device_tab1, "device_tab1");
        lv_obj_set_style_bg_opa(device_tab1, (255 * 70 / 100), LV_STATE_PRESSED);
        lv_obj_t * lv_label_1 = lv_label_create(device_tab1);
        lv_obj_set_x(lv_label_1, 0);
        lv_obj_set_y(lv_label_1, 6);
        lv_label_set_text(lv_label_1, "COMM LINK");
        lv_obj_set_style_text_font(lv_label_1, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_1, lv_color_hex(0x64748b), 0);
        lv_obj_set_clickable(lv_label_1, false);
        lv_obj_set_scrollable(lv_label_1, false);
        lv_obj_set_width(lv_label_1, 134);
        lv_obj_set_height(lv_label_1, 13);
        lv_label_set_long_mode(lv_label_1, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_1, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_letter_space(lv_label_1, 1, 0);

        lv_obj_t * device_tab2 = lv_button_create(lv_obj_0);
        lv_obj_set_x(device_tab2, 271);
        lv_obj_set_y(device_tab2, 3);
        lv_obj_set_width(device_tab2, 134);
        lv_obj_set_height(device_tab2, 26);
        lv_obj_set_clickable(device_tab2, true);
        lv_obj_set_scrollable(device_tab2, false);
        lv_obj_set_style_bg_color(device_tab2, lv_color_hex(0x111722), 0);
        lv_obj_set_style_bg_opa(device_tab2, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(device_tab2, 6, 0);
        lv_obj_set_style_border_width(device_tab2, 0, 0);
        lv_obj_set_style_outline_width(device_tab2, 0, 0);
        lv_obj_set_style_shadow_width(device_tab2, 0, 0);
        lv_obj_set_style_pad_all(device_tab2, 0, 0);
        lv_obj_set_name(device_tab2, "device_tab2");
        lv_obj_set_style_bg_opa(device_tab2, (255 * 70 / 100), LV_STATE_PRESSED);
        lv_obj_t * lv_label_2 = lv_label_create(device_tab2);
        lv_obj_set_x(lv_label_2, 0);
        lv_obj_set_y(lv_label_2, 6);
        lv_label_set_text(lv_label_2, "SETTINGS");
        lv_obj_set_style_text_font(lv_label_2, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_2, lv_color_hex(0x64748b), 0);
        lv_obj_set_clickable(lv_label_2, false);
        lv_obj_set_scrollable(lv_label_2, false);
        lv_obj_set_width(lv_label_2, 134);
        lv_obj_set_height(lv_label_2, 13);
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

