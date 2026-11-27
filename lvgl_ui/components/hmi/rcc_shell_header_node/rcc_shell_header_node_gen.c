/**
 * @file rcc_shell_header_node_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "rcc_shell_header_node_gen.h"
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

lv_obj_t * rcc_shell_header_node_create(lv_obj_t * parent, lv_font_t * font)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_button_0 = lv_button_create(parent);
        lv_obj_set_name_static(lv_button_0, "rcc_shell_header_node_#");
        lv_obj_set_x(lv_button_0, 64);
        lv_obj_set_y(lv_button_0, 4);
        lv_obj_set_width(lv_button_0, 110);
        lv_obj_set_height(lv_button_0, 24);
        lv_obj_set_scrollable(lv_button_0, false);
        lv_obj_set_clickable(lv_button_0, true);
        lv_obj_set_style_pad_all(lv_button_0, 0, 0);
        lv_obj_set_style_bg_color(lv_button_0, lv_color_hex(0x090d14), 0);
        lv_obj_set_style_bg_opa(lv_button_0, (255 * 0 / 100), 0);
        lv_obj_set_style_radius(lv_button_0, 6, 0);
        lv_obj_set_style_border_width(lv_button_0, 0, 0);
        lv_obj_set_style_outline_width(lv_button_0, 0, 0);
        lv_obj_set_style_shadow_width(lv_button_0, 0, 0);
        lv_obj_set_style_bg_opa(lv_button_0, 178, LV_STATE_PRESSED);

        lv_obj_t * node_prefix = lv_label_create(lv_button_0);
        lv_obj_set_x(node_prefix, 0);
        lv_obj_set_y(node_prefix, 5);
        lv_label_set_text(node_prefix, "NODE");
        lv_obj_set_scrollable(node_prefix, false);
        lv_obj_set_clickable(node_prefix, false);
        lv_obj_set_style_text_font(node_prefix, font, 0);
        lv_obj_set_style_text_color(node_prefix, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_style_text_align(node_prefix, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_letter_space(node_prefix, 1, 0);
        lv_obj_set_name(node_prefix, "node_prefix");

        lv_obj_t * node_name = lv_label_create(lv_button_0);
        lv_obj_set_x(node_name, 49);
        lv_obj_set_y(node_name, 5);
        lv_label_set_text(node_name, "RCC-01");
        lv_obj_set_scrollable(node_name, false);
        lv_obj_set_clickable(node_name, false);
        lv_obj_set_style_text_font(node_name, font, 0);
        lv_obj_set_style_text_color(node_name, lv_color_hex(0xcbd5e1), 0);
        lv_obj_set_style_text_align(node_name, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_letter_space(node_name, 1, 0);
        lv_obj_set_name(node_name, "node_name");

        the_root = lv_button_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

