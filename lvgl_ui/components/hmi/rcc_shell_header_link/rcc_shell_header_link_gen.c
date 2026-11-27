/**
 * @file rcc_shell_header_link_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "rcc_shell_header_link_gen.h"
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

lv_obj_t * rcc_shell_header_link_create(lv_obj_t * parent, lv_font_t * font)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_button_0 = lv_button_create(parent);
        lv_obj_set_name_static(lv_button_0, "rcc_shell_header_link_#");
        lv_obj_set_x(lv_button_0, 239);
        lv_obj_set_y(lv_button_0, 5);
        lv_obj_set_width(lv_button_0, 123);
        lv_obj_set_height(lv_button_0, 23);
        lv_obj_set_scrollable(lv_button_0, false);
        lv_obj_set_clickable(lv_button_0, true);
        lv_obj_set_style_pad_all(lv_button_0, -1, 0);
        lv_obj_set_style_bg_color(lv_button_0, lv_color_hex(0x111722), 0);
        lv_obj_set_style_bg_opa(lv_button_0, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(lv_button_0, 10, 0);
        lv_obj_set_style_border_width(lv_button_0, 1, 0);
        lv_obj_set_style_outline_width(lv_button_0, 0, 0);
        lv_obj_set_style_shadow_width(lv_button_0, 0, 0);
        lv_obj_set_style_border_color(lv_button_0, lv_color_hex(0x1e293b), 0);
        lv_obj_set_style_bg_opa(lv_button_0, 178, LV_STATE_PRESSED);

        lv_obj_t * link_dot = lv_obj_create(lv_button_0);
        lv_obj_set_x(link_dot, 11);
        lv_obj_set_y(link_dot, 8);
        lv_obj_set_width(link_dot, 7);
        lv_obj_set_height(link_dot, 7);
        lv_obj_set_scrollable(link_dot, false);
        lv_obj_set_clickable(link_dot, false);
        lv_obj_set_style_pad_all(link_dot, 0, 0);
        lv_obj_set_style_bg_color(link_dot, lv_color_hex(0x22c55e), 0);
        lv_obj_set_style_bg_opa(link_dot, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(link_dot, 4, 0);
        lv_obj_set_style_border_width(link_dot, 0, 0);
        lv_obj_set_style_outline_width(link_dot, 0, 0);
        lv_obj_set_style_shadow_width(link_dot, 0, 0);
        lv_obj_set_name(link_dot, "link_dot");

        lv_obj_t * link_text = lv_label_create(lv_button_0);
        lv_obj_set_x(link_text, 24);
        lv_obj_set_y(link_text, 5);
        lv_label_set_text(link_text, "ONLINE · 0.4 s");
        lv_obj_set_scrollable(link_text, false);
        lv_obj_set_clickable(link_text, false);
        lv_obj_set_style_text_font(link_text, font, 0);
        lv_obj_set_style_text_color(link_text, lv_color_hex(0x22c55e), 0);
        lv_obj_set_style_text_align(link_text, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_letter_space(link_text, 0, 0);
        lv_obj_set_name(link_text, "link_text");

        the_root = lv_button_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

