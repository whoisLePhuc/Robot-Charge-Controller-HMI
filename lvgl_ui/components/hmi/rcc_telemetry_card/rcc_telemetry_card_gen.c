/**
 * @file rcc_telemetry_card_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "rcc_telemetry_card_gen.h"
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

lv_obj_t * rcc_telemetry_card_create(lv_obj_t * parent, int32_t card_x, int32_t card_y, int32_t card_height, const char * title, const char * unit, lv_color_t accent, const char * reading, const char * summary, int32_t fill_width, int32_t unit_x, lv_color_t value_color, bool hide_unit, bool hide_fill, lv_font_t * caption_font, lv_font_t * value_font, lv_font_t * unit_font, const void * card_icon)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "rcc_telemetry_card_#");
        lv_obj_set_scrollable(lv_obj_0, false);
        lv_obj_set_clickable(lv_obj_0, true);
        lv_obj_set_x(lv_obj_0, card_x);
        lv_obj_set_y(lv_obj_0, card_y);
        lv_obj_set_width(lv_obj_0, 77);
        lv_obj_set_height(lv_obj_0, card_height);
        lv_obj_set_style_border_color(lv_obj_0, lv_color_hex(0x334155), LV_STATE_PRESSED);
        lv_obj_set_style_bg_color(lv_obj_0, lv_color_hex(0x111722), 0);
        lv_obj_set_style_bg_opa(lv_obj_0, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(lv_obj_0, 10, 0);
        lv_obj_set_style_border_width(lv_obj_0, 1, 0);
        lv_obj_set_style_border_color(lv_obj_0, lv_color_hex(0x1e293b), 0);
        lv_obj_set_style_border_opa(lv_obj_0, (255 * 100 / 100), 0);
        lv_obj_set_style_outline_width(lv_obj_0, 0, 0);
        lv_obj_set_style_shadow_width(lv_obj_0, 0, 0);
        lv_obj_set_style_pad_all(lv_obj_0, -1, 0);

        lv_obj_t * lv_label_0 = lv_label_create(lv_obj_0);
        lv_obj_set_x(lv_label_0, 9);
        lv_obj_set_y(lv_label_0, 8);
        lv_label_set_text(lv_label_0, title);
        lv_obj_set_scrollable(lv_label_0, false);
        lv_obj_set_clickable(lv_label_0, false);
        lv_obj_set_style_text_font(lv_label_0, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_0, lv_color_hex(0x94a3b8), 0);

        lv_obj_t * voltage_icon = lv_image_create(lv_obj_0);
        lv_obj_set_x(voltage_icon, 55);
        lv_obj_set_y(voltage_icon, 8);
        lv_image_set_src(voltage_icon, card_icon);
        lv_obj_set_clickable(voltage_icon, false);
        lv_obj_set_scrollable(voltage_icon, false);
        lv_obj_set_style_image_recolor(voltage_icon, accent, 0);
        lv_obj_set_style_image_recolor_opa(voltage_icon, (255 * 100 / 100), 0);
        lv_obj_set_name(voltage_icon, "voltage_icon");

        lv_obj_t * voltage_value = lv_label_create(lv_obj_0);
        lv_obj_set_x(voltage_value, 9);
        lv_obj_set_y(voltage_value, 28 + (card_height - 74) / 3);
        lv_label_set_text(voltage_value, reading);
        lv_obj_set_scrollable(voltage_value, false);
        lv_obj_set_clickable(voltage_value, false);
        lv_obj_set_style_text_font(voltage_value, value_font, 0);
        lv_obj_set_style_text_color(voltage_value, value_color, 0);
        lv_obj_set_name(voltage_value, "voltage_value");

        lv_obj_t * voltage_unit = lv_label_create(lv_obj_0);
        lv_obj_set_x(voltage_unit, unit_x);
        lv_obj_set_y(voltage_unit, 29 + (card_height - 74) / 3);
        lv_label_set_text(voltage_unit, unit);
        lv_obj_set_scrollable(voltage_unit, false);
        lv_obj_set_clickable(voltage_unit, false);
        lv_obj_set_style_text_font(voltage_unit, unit_font, 0);
        lv_obj_set_style_text_color(voltage_unit, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_hidden(voltage_unit, hide_unit);
        lv_obj_set_name(voltage_unit, "voltage_unit");

        lv_obj_t * voltage_track = lv_obj_create(lv_obj_0);
        lv_obj_set_x(voltage_track, 9);
        lv_obj_set_y(voltage_track, 48 + 2 * ((card_height - 74) / 3));
        lv_obj_set_width(voltage_track, 59);
        lv_obj_set_height(voltage_track, 5);
        lv_obj_set_clickable(voltage_track, false);
        lv_obj_set_scrollable(voltage_track, false);
        lv_obj_set_style_bg_color(voltage_track, lv_color_hex(0x172030), 0);
        lv_obj_set_style_bg_opa(voltage_track, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(voltage_track, 3, 0);
        lv_obj_set_style_border_width(voltage_track, 0, 0);
        lv_obj_set_style_outline_width(voltage_track, 0, 0);
        lv_obj_set_style_shadow_width(voltage_track, 0, 0);
        lv_obj_set_style_pad_all(voltage_track, 0, 0);
        lv_obj_set_name(voltage_track, "voltage_track");

        lv_obj_t * voltage_fill = lv_obj_create(lv_obj_0);
        lv_obj_set_x(voltage_fill, 9);
        lv_obj_set_y(voltage_fill, 48 + 2 * ((card_height - 74) / 3));
        lv_obj_set_width(voltage_fill, fill_width);
        lv_obj_set_height(voltage_fill, 5);
        lv_obj_set_clickable(voltage_fill, false);
        lv_obj_set_scrollable(voltage_fill, false);
        lv_obj_set_style_bg_color(voltage_fill, accent, 0);
        lv_obj_set_style_bg_opa(voltage_fill, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(voltage_fill, 3, 0);
        lv_obj_set_style_border_width(voltage_fill, 0, 0);
        lv_obj_set_style_outline_width(voltage_fill, 0, 0);
        lv_obj_set_style_shadow_width(voltage_fill, 0, 0);
        lv_obj_set_style_pad_all(voltage_fill, 0, 0);
        lv_obj_set_name(voltage_fill, "voltage_fill");
        lv_obj_set_hidden(voltage_fill, hide_fill);

        lv_obj_t * lv_obj_1 = lv_obj_create(lv_obj_0);
        lv_obj_set_x(lv_obj_1, 56);
        lv_obj_set_y(lv_obj_1, 48 + 2 * ((card_height - 74) / 3));
        lv_obj_set_width(lv_obj_1, 2);
        lv_obj_set_height(lv_obj_1, 5);
        lv_obj_set_clickable(lv_obj_1, false);
        lv_obj_set_scrollable(lv_obj_1, false);
        lv_obj_set_style_bg_color(lv_obj_1, lv_color_hex(0xffffff), 0);
        lv_obj_set_style_bg_opa(lv_obj_1, 115, 0);
        lv_obj_set_style_radius(lv_obj_1, 0, 0);
        lv_obj_set_style_border_width(lv_obj_1, 0, 0);
        lv_obj_set_style_outline_width(lv_obj_1, 0, 0);
        lv_obj_set_style_shadow_width(lv_obj_1, 0, 0);
        lv_obj_set_style_pad_all(lv_obj_1, 0, 0);

        lv_obj_t * voltage_caption = lv_label_create(lv_obj_0);
        lv_obj_set_x(voltage_caption, 9);
        lv_obj_set_y(voltage_caption, card_height - 21);
        lv_label_set_text(voltage_caption, summary);
        lv_obj_set_scrollable(voltage_caption, false);
        lv_obj_set_clickable(voltage_caption, false);
        lv_obj_set_style_text_font(voltage_caption, caption_font, 0);
        lv_obj_set_style_text_color(voltage_caption, lv_color_hex(0x64748b), 0);
        lv_obj_set_width(voltage_caption, 59);
        lv_obj_set_height(voltage_caption, 13);
        lv_label_set_long_mode(voltage_caption, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(voltage_caption, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_name(voltage_caption, "voltage_caption");

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

