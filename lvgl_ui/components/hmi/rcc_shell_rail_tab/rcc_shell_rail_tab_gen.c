/**
 * @file rcc_shell_rail_tab_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "rcc_shell_rail_tab_gen.h"
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

lv_obj_t * rcc_shell_rail_tab_create(lv_obj_t * parent, int32_t top, int32_t tab_height, const char * label, const void * glyph, lv_font_t * font)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "rcc_shell_rail_tab_#");
        lv_obj_set_x(lv_obj_0, 0);
        lv_obj_set_y(lv_obj_0, top);
        lv_obj_set_width(lv_obj_0, 56);
        lv_obj_set_height(lv_obj_0, tab_height);
        lv_obj_set_scrollable(lv_obj_0, false);
        lv_obj_set_clickable(lv_obj_0, false);
        lv_obj_set_style_pad_all(lv_obj_0, 0, 0);
        lv_obj_set_style_bg_color(lv_obj_0, lv_color_hex(0x090d14), 0);
        lv_obj_set_style_bg_opa(lv_obj_0, (255 * 0 / 100), 0);
        lv_obj_set_style_radius(lv_obj_0, 0, 0);
        lv_obj_set_style_border_width(lv_obj_0, 0, 0);
        lv_obj_set_style_outline_width(lv_obj_0, 0, 0);
        lv_obj_set_style_shadow_width(lv_obj_0, 0, 0);

        lv_obj_t * tab_hit = lv_button_create(lv_obj_0);
        lv_obj_set_x(tab_hit, 4);
        lv_obj_set_y(tab_hit, 0);
        lv_obj_set_width(tab_hit, 47);
        lv_obj_set_height(tab_hit, tab_height);
        lv_obj_set_scrollable(tab_hit, false);
        lv_obj_set_clickable(tab_hit, true);
        lv_obj_set_style_pad_all(tab_hit, 0, 0);
        lv_obj_set_style_bg_color(tab_hit, lv_color_hex(0x06090e), 0);
        lv_obj_set_style_bg_opa(tab_hit, (255 * 0 / 100), 0);
        lv_obj_set_style_radius(tab_hit, 10, 0);
        lv_obj_set_style_border_width(tab_hit, 0, 0);
        lv_obj_set_style_outline_width(tab_hit, 0, 0);
        lv_obj_set_style_shadow_width(tab_hit, 0, 0);
        lv_obj_set_style_bg_opa(tab_hit, 178, LV_STATE_PRESSED);
        lv_obj_set_name(tab_hit, "tab_hit");
        lv_obj_t * tab_icon = lv_image_create(tab_hit);
        lv_obj_set_x(tab_icon, 14);
        lv_obj_set_y(tab_icon, 12);
        lv_image_set_src(tab_icon, glyph);
        lv_obj_set_clickable(tab_icon, false);
        lv_obj_set_scrollable(tab_icon, false);
        lv_obj_set_style_image_recolor(tab_icon, lv_color_hex(0x64748b), 0);
        lv_obj_set_style_image_recolor_opa(tab_icon, (255 * 100 / 100), 0);
        lv_obj_set_name(tab_icon, "tab_icon");

        lv_obj_t * tab_label = lv_label_create(tab_hit);
        lv_obj_set_x(tab_label, 0);
        lv_obj_set_y(tab_label, 35);
        lv_label_set_text(tab_label, label);
        lv_obj_set_scrollable(tab_label, false);
        lv_obj_set_clickable(tab_label, false);
        lv_obj_set_style_text_font(tab_label, font, 0);
        lv_obj_set_style_text_color(tab_label, lv_color_hex(0x64748b), 0);
        lv_obj_set_style_text_align(tab_label, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_letter_space(tab_label, 0, 0);
        lv_obj_set_name(tab_label, "tab_label");

        lv_obj_t * tab_mark = lv_obj_create(lv_obj_0);
        lv_obj_set_x(tab_mark, 0);
        lv_obj_set_y(tab_mark, 14);
        lv_obj_set_width(tab_mark, 3);
        lv_obj_set_height(tab_mark, 32);
        lv_obj_set_scrollable(tab_mark, false);
        lv_obj_set_clickable(tab_mark, false);
        lv_obj_set_style_pad_all(tab_mark, 0, 0);
        lv_obj_set_style_bg_color(tab_mark, lv_color_hex(0x38bdf8), 0);
        lv_obj_set_style_bg_opa(tab_mark, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(tab_mark, 2, 0);
        lv_obj_set_style_border_width(tab_mark, 0, 0);
        lv_obj_set_style_outline_width(tab_mark, 0, 0);
        lv_obj_set_style_shadow_width(tab_mark, 0, 0);
        lv_obj_set_name(tab_mark, "tab_mark");

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

