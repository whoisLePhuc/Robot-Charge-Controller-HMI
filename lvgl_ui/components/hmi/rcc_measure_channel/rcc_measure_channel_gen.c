/**
 * @file rcc_measure_channel_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "rcc_measure_channel_gen.h"
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

lv_obj_t * rcc_measure_channel_create(lv_obj_t * parent, int32_t card_x, const char * title, const char * limit, const char * unit, lv_color_t hue, int32_t limit_x, const char * reading, const char * filtered, int32_t unit_x, lv_font_t * small, lv_font_t * reading_font, lv_font_t * unit_font)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "rcc_measure_channel_#");
        lv_obj_set_scrollable(lv_obj_0, false);
        lv_obj_set_clickable(lv_obj_0, false);
        lv_obj_set_x(lv_obj_0, card_x);
        lv_obj_set_y(lv_obj_0, 0);
        lv_obj_set_width(lv_obj_0, 200);
        lv_obj_set_height(lv_obj_0, 76);
        lv_obj_set_style_bg_color(lv_obj_0, lv_color_hex(0x111722), 0);
        lv_obj_set_style_bg_opa(lv_obj_0, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(lv_obj_0, 10, 0);
        lv_obj_set_style_border_width(lv_obj_0, 1, 0);
        lv_obj_set_style_border_color(lv_obj_0, lv_color_hex(0x1e293b), 0);
        lv_obj_set_style_border_opa(lv_obj_0, (255 * 100 / 100), 0);
        lv_obj_set_style_outline_width(lv_obj_0, 0, 0);
        lv_obj_set_style_shadow_width(lv_obj_0, 0, 0);
        lv_obj_set_style_pad_all(lv_obj_0, -1, 0);

        lv_obj_t * channel_title = lv_label_create(lv_obj_0);
        lv_obj_set_x(channel_title, 13);
        lv_obj_set_y(channel_title, 8);
        lv_label_set_text(channel_title, title);
        lv_obj_set_scrollable(channel_title, false);
        lv_obj_set_clickable(channel_title, false);
        lv_obj_set_style_text_font(channel_title, small, 0);
        lv_obj_set_style_text_color(channel_title, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_style_text_letter_space(channel_title, 1, 0);
        lv_obj_set_name(channel_title, "channel_title");

        lv_obj_t * channel_limit = lv_label_create(lv_obj_0);
        lv_obj_set_x(channel_limit, limit_x);
        lv_obj_set_y(channel_limit, 8);
        lv_label_set_text(channel_limit, limit);
        lv_obj_set_scrollable(channel_limit, false);
        lv_obj_set_clickable(channel_limit, false);
        lv_obj_set_style_text_font(channel_limit, small, 0);
        lv_obj_set_style_text_color(channel_limit, lv_color_hex(0x64748b), 0);
        lv_obj_set_name(channel_limit, "channel_limit");

        lv_obj_t * channel_badge = lv_obj_create(lv_obj_0);
        lv_obj_set_x(channel_badge, 141);
        lv_obj_set_y(channel_badge, 7);
        lv_obj_set_width(channel_badge, 46);
        lv_obj_set_height(channel_badge, 17);
        lv_obj_set_clickable(channel_badge, false);
        lv_obj_set_scrollable(channel_badge, false);
        lv_obj_set_style_bg_color(channel_badge, lv_color_hex(0x172030), 0);
        lv_obj_set_style_bg_opa(channel_badge, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(channel_badge, 6, 0);
        lv_obj_set_style_border_width(channel_badge, 0, 0);
        lv_obj_set_style_outline_width(channel_badge, 0, 0);
        lv_obj_set_style_shadow_width(channel_badge, 0, 0);
        lv_obj_set_style_pad_all(channel_badge, 0, 0);
        lv_obj_set_name(channel_badge, "channel_badge");

        lv_obj_t * channel_status = lv_label_create(lv_obj_0);
        lv_obj_set_x(channel_status, 148);
        lv_obj_set_y(channel_status, 9);
        lv_label_set_text(channel_status, "VALID");
        lv_obj_set_scrollable(channel_status, false);
        lv_obj_set_clickable(channel_status, false);
        lv_obj_set_style_text_font(channel_status, small, 0);
        lv_obj_set_style_text_color(channel_status, lv_color_hex(0x22c55e), 0);
        lv_obj_set_name(channel_status, "channel_status");

        lv_obj_t * channel_value = lv_label_create(lv_obj_0);
        lv_obj_set_x(channel_value, 13);
        lv_obj_set_y(channel_value, 29);
        lv_label_set_text(channel_value, reading);
        lv_obj_set_scrollable(channel_value, false);
        lv_obj_set_clickable(channel_value, false);
        lv_obj_set_style_text_font(channel_value, reading_font, 0);
        lv_obj_set_style_text_color(channel_value, hue, 0);
        lv_obj_set_name(channel_value, "channel_value");

        lv_obj_t * channel_unit = lv_label_create(lv_obj_0);
        lv_obj_set_x(channel_unit, unit_x);
        lv_obj_set_y(channel_unit, 33);
        lv_label_set_text(channel_unit, unit);
        lv_obj_set_scrollable(channel_unit, false);
        lv_obj_set_clickable(channel_unit, false);
        lv_obj_set_style_text_font(channel_unit, unit_font, 0);
        lv_obj_set_style_text_color(channel_unit, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_name(channel_unit, "channel_unit");

        lv_obj_t * channel_filtered = lv_label_create(lv_obj_0);
        lv_obj_set_x(channel_filtered, 13);
        lv_obj_set_y(channel_filtered, 56);
        lv_label_set_text(channel_filtered, filtered);
        lv_obj_set_scrollable(channel_filtered, false);
        lv_obj_set_clickable(channel_filtered, false);
        lv_obj_set_style_text_font(channel_filtered, small, 0);
        lv_obj_set_style_text_color(channel_filtered, lv_color_hex(0xcbd5e1), 0);
        lv_obj_set_width(channel_filtered, 174);
        lv_obj_set_height(channel_filtered, 13);
        lv_label_set_long_mode(channel_filtered, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(channel_filtered, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_name(channel_filtered, "channel_filtered");

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

