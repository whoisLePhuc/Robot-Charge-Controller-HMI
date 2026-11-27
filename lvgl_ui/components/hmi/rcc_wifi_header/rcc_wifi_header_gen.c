/**
 * @file rcc_wifi_header_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "rcc_wifi_header_gen.h"
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

lv_obj_t * rcc_wifi_header_create(lv_obj_t * parent, const char * heading, const char * subtitle, lv_font_t * title_font, lv_font_t * caption_font, const void * back_icon)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "rcc_wifi_header_#");
        lv_obj_set_width(lv_obj_0, 480);
        lv_obj_set_height(lv_obj_0, 42);
        lv_obj_set_clickable(lv_obj_0, false);
        lv_obj_set_scrollable(lv_obj_0, false);
        lv_obj_set_style_bg_opa(lv_obj_0, (255 * 0 / 100), 0);
        lv_obj_set_style_pad_all(lv_obj_0, 0, 0);
        lv_obj_set_style_border_width(lv_obj_0, 0, 0);
        lv_obj_set_style_outline_width(lv_obj_0, 0, 0);
        lv_obj_set_style_shadow_width(lv_obj_0, 0, 0);
        lv_obj_set_style_radius(lv_obj_0, 0, 0);

        lv_obj_t * back = lv_button_create(lv_obj_0);
        lv_obj_set_x(back, 8);
        lv_obj_set_y(back, 8);
        lv_obj_set_width(back, 34);
        lv_obj_set_height(back, 30);
        lv_obj_set_clickable(back, true);
        lv_obj_set_style_radius(back, 8, 0);
        lv_obj_set_style_border_width(back, 1, 0);
        lv_obj_set_style_bg_opa(back, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(back, lv_color_hex(0x172030), 0);
        lv_obj_set_style_border_color(back, lv_color_hex(0x1e293b), 0);
        lv_obj_set_style_pad_top(back, -1, 0);
        lv_obj_set_style_pad_bottom(back, -1, 0);
        lv_obj_set_style_pad_left(back, -1, 0);
        lv_obj_set_style_pad_right(back, -1, 0);
        lv_obj_set_scrollable(back, false);
        lv_obj_set_style_shadow_width(back, 0, 0);
        lv_obj_set_style_outline_width(back, 0, 0);
        lv_obj_set_name(back, "back");
        lv_obj_set_style_bg_opa(back, (255 * 70 / 100), LV_STATE_PRESSED);
        lv_obj_t * lv_image_0 = lv_image_create(back);
        lv_obj_set_x(lv_image_0, 10);
        lv_obj_set_y(lv_image_0, 8);
        lv_obj_set_width(lv_image_0, 14);
        lv_obj_set_height(lv_image_0, 14);
        lv_obj_set_clickable(lv_image_0, false);
        lv_obj_set_scrollable(lv_image_0, false);
        lv_image_set_src(lv_image_0, back_icon);
        lv_obj_set_style_image_recolor(lv_image_0, lv_color_hex(0xcbd5e1), 0);
        lv_obj_set_style_image_recolor_opa(lv_image_0, (255 * 100 / 100), 0);

        lv_obj_t * lv_label_0 = lv_label_create(lv_obj_0);
        lv_obj_set_x(lv_label_0, 52);
        lv_obj_set_y(lv_label_0, 8);
        lv_obj_set_width(lv_label_0, 330);
        lv_obj_set_height(lv_label_0, LV_SIZE_CONTENT);
        lv_obj_set_clickable(lv_label_0, false);
        lv_obj_set_style_text_font(lv_label_0, title_font, 0);
        lv_obj_set_style_text_color(lv_label_0, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_scrollable(lv_label_0, false);
        lv_label_set_text(lv_label_0, heading);
        lv_label_set_long_mode(lv_label_0, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * lv_label_1 = lv_label_create(lv_obj_0);
        lv_obj_set_x(lv_label_1, 52);
        lv_obj_set_y(lv_label_1, 25);
        lv_obj_set_width(lv_label_1, 330);
        lv_obj_set_height(lv_label_1, LV_SIZE_CONTENT);
        lv_obj_set_clickable(lv_label_1, false);
        lv_obj_set_style_text_font(lv_label_1, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_1, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_scrollable(lv_label_1, false);
        lv_label_set_text(lv_label_1, subtitle);
        lv_label_set_long_mode(lv_label_1, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_1, LV_TEXT_ALIGN_LEFT, 0);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

lv_obj_t * rcc_wifi_header_get_back(lv_obj_t *parent)
{
    return lv_obj_find_by_name(parent, "back");
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

