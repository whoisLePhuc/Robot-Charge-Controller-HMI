/**
 * @file rcc_shell_action_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "rcc_shell_action_gen.h"
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

lv_obj_t * rcc_shell_action_create(lv_obj_t * parent, int32_t left, int32_t button_width, const char * label, const void * glyph, lv_color_t bg, lv_color_t fg, lv_font_t * font)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_button_0 = lv_button_create(parent);
        lv_obj_set_name_static(lv_button_0, "rcc_shell_action_#");
        lv_obj_set_x(lv_button_0, left);
        lv_obj_set_y(lv_button_0, 0);
        lv_obj_set_width(lv_button_0, button_width);
        lv_obj_set_height(lv_button_0, 42);
        lv_obj_set_scrollable(lv_button_0, false);
        lv_obj_set_clickable(lv_button_0, true);
        lv_obj_set_style_pad_all(lv_button_0, 0, 0);
        lv_obj_set_style_bg_color(lv_button_0, bg, 0);
        lv_obj_set_style_bg_opa(lv_button_0, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(lv_button_0, 10, 0);
        lv_obj_set_style_border_width(lv_button_0, 1, 0);
        lv_obj_set_style_outline_width(lv_button_0, 0, 0);
        lv_obj_set_style_shadow_width(lv_button_0, 0, 0);
        lv_obj_set_style_border_color(lv_button_0, lv_color_hex(0xffffff), 0);
        lv_obj_set_style_bg_opa(lv_button_0, 178, LV_STATE_PRESSED);
        lv_obj_set_style_border_opa(lv_button_0, 36, 0);

        lv_obj_t * action_label = lv_label_create(lv_button_0);
        lv_obj_set_x(action_label, button_width / 2 - 13);
        lv_obj_set_y(action_label, 12);
        lv_label_set_text(action_label, label);
        lv_obj_set_scrollable(action_label, false);
        lv_obj_set_clickable(action_label, false);
        lv_obj_set_style_text_font(action_label, font, 0);
        lv_obj_set_style_text_color(action_label, fg, 0);
        lv_obj_set_style_text_align(action_label, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_letter_space(action_label, 1, 0);
        lv_obj_set_name(action_label, "action_label");

        lv_obj_t * action_icon = lv_image_create(lv_button_0);
        lv_obj_set_x(action_icon, button_width / 2 - 37);
        lv_obj_set_y(action_icon, 13);
        lv_image_set_src(action_icon, glyph);
        lv_obj_set_clickable(action_icon, false);
        lv_obj_set_scrollable(action_icon, false);
        lv_obj_set_style_image_recolor(action_icon, fg, 0);
        lv_obj_set_style_image_recolor_opa(action_icon, (255 * 100 / 100), 0);
        lv_obj_set_name(action_icon, "action_icon");

        the_root = lv_button_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

