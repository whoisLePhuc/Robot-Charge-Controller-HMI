/**
 * @file rcc_session_summary_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "rcc_session_summary_gen.h"
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

lv_obj_t * rcc_session_summary_create(lv_obj_t * parent, int32_t card_x, const char * title, const char * value, lv_font_t * small, lv_font_t * bold)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "rcc_session_summary_#");
        lv_obj_set_scrollable(lv_obj_0, false);
        lv_obj_set_clickable(lv_obj_0, false);
        lv_obj_set_x(lv_obj_0, card_x);
        lv_obj_set_y(lv_obj_0, 0);
        lv_obj_set_width(lv_obj_0, 98);
        lv_obj_set_height(lv_obj_0, 39);
        lv_obj_set_style_bg_color(lv_obj_0, lv_color_hex(0x111722), 0);
        lv_obj_set_style_bg_opa(lv_obj_0, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(lv_obj_0, 8, 0);
        lv_obj_set_style_border_width(lv_obj_0, 1, 0);
        lv_obj_set_style_border_color(lv_obj_0, lv_color_hex(0x1e293b), 0);
        lv_obj_set_style_border_opa(lv_obj_0, (255 * 100 / 100), 0);
        lv_obj_set_style_outline_width(lv_obj_0, 0, 0);
        lv_obj_set_style_shadow_width(lv_obj_0, 0, 0);
        lv_obj_set_style_pad_all(lv_obj_0, -1, 0);

        lv_obj_t * lv_label_0 = lv_label_create(lv_obj_0);
        lv_obj_set_x(lv_label_0, 9);
        lv_obj_set_y(lv_label_0, 5);
        lv_label_set_text(lv_label_0, title);
        lv_obj_set_scrollable(lv_label_0, false);
        lv_obj_set_clickable(lv_label_0, false);
        lv_obj_set_style_text_font(lv_label_0, small, 0);
        lv_obj_set_style_text_color(lv_label_0, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_width(lv_label_0, 80);
        lv_obj_set_height(lv_label_0, 13);
        lv_label_set_long_mode(lv_label_0, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * summary_value = lv_label_create(lv_obj_0);
        lv_obj_set_x(summary_value, 9);
        lv_obj_set_y(summary_value, 19);
        lv_label_set_text(summary_value, value);
        lv_obj_set_scrollable(summary_value, false);
        lv_obj_set_clickable(summary_value, false);
        lv_obj_set_style_text_font(summary_value, bold, 0);
        lv_obj_set_style_text_color(summary_value, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_width(summary_value, 80);
        lv_obj_set_height(summary_value, 13);
        lv_label_set_long_mode(summary_value, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(summary_value, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_name(summary_value, "summary_value");

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

