/**
 * @file rcc_faults_primary_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "rcc_faults_primary_gen.h"
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

lv_obj_t * rcc_faults_primary_create(lv_obj_t * parent, lv_font_t * caption_font, lv_font_t * body_font, lv_font_t * value_font)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "rcc_faults_primary_#");
        lv_obj_set_x(lv_obj_0, 0);
        lv_obj_set_y(lv_obj_0, 46);
        lv_obj_set_width(lv_obj_0, 408);
        lv_obj_set_height(lv_obj_0, 44);
        lv_obj_set_clickable(lv_obj_0, false);
        lv_obj_set_scrollable(lv_obj_0, false);
        lv_obj_set_style_bg_color(lv_obj_0, lv_color_hex(0x2b2309), 0);
        lv_obj_set_style_bg_opa(lv_obj_0, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(lv_obj_0, 10, 0);
        lv_obj_set_style_border_width(lv_obj_0, 1, 0);
        lv_obj_set_style_outline_width(lv_obj_0, 0, 0);
        lv_obj_set_style_shadow_width(lv_obj_0, 0, 0);
        lv_obj_set_style_pad_all(lv_obj_0, -1, 0);
        lv_obj_set_style_border_color(lv_obj_0, lv_color_hex(0xfacc15), 0);
        lv_obj_set_style_border_opa(lv_obj_0, (255 * 100 / 100), 0);

        lv_obj_t * fault_primary_name = lv_label_create(lv_obj_0);
        lv_obj_set_x(fault_primary_name, 13);
        lv_obj_set_y(fault_primary_name, 8);
        lv_label_set_text(fault_primary_name, "PRIMARY · IOUT_OVERCURRENT");
        lv_obj_set_style_text_font(fault_primary_name, value_font, 0);
        lv_obj_set_style_text_color(fault_primary_name, lv_color_hex(0xfacc15), 0);
        lv_obj_set_clickable(fault_primary_name, false);
        lv_obj_set_scrollable(fault_primary_name, false);
        lv_obj_set_width(fault_primary_name, 300);
        lv_obj_set_height(fault_primary_name, 13);
        lv_label_set_long_mode(fault_primary_name, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(fault_primary_name, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_name(fault_primary_name, "fault_primary_name");

        lv_obj_t * fault_primary_id = lv_label_create(lv_obj_0);
        lv_obj_set_x(fault_primary_id, 320);
        lv_obj_set_y(fault_primary_id, 7);
        lv_label_set_text(fault_primary_id, "0x0402");
        lv_obj_set_style_text_font(fault_primary_id, body_font, 0);
        lv_obj_set_style_text_color(fault_primary_id, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_clickable(fault_primary_id, false);
        lv_obj_set_scrollable(fault_primary_id, false);
        lv_obj_set_width(fault_primary_id, 75);
        lv_obj_set_height(fault_primary_id, 15);
        lv_label_set_long_mode(fault_primary_id, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(fault_primary_id, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_set_name(fault_primary_id, "fault_primary_id");

        lv_obj_t * fault_primary_action = lv_label_create(lv_obj_0);
        lv_obj_set_x(fault_primary_action, 13);
        lv_obj_set_y(fault_primary_action, 23);
        lv_label_set_text(fault_primary_action, "Action: check the load and the current limit");
        lv_obj_set_style_text_font(fault_primary_action, caption_font, 0);
        lv_obj_set_style_text_color(fault_primary_action, lv_color_hex(0xcbd5e1), 0);
        lv_obj_set_clickable(fault_primary_action, false);
        lv_obj_set_scrollable(fault_primary_action, false);
        lv_obj_set_width(fault_primary_action, 382);
        lv_obj_set_height(fault_primary_action, 13);
        lv_label_set_long_mode(fault_primary_action, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(fault_primary_action, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_name(fault_primary_action, "fault_primary_action");

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

