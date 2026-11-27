/**
 * @file rcc_faults_mask_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "rcc_faults_mask_gen.h"
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

lv_obj_t * rcc_faults_mask_create(lv_obj_t * parent, int32_t card_x, int32_t card_y, int32_t card_height, const char * list_title, lv_font_t * caption_font, lv_font_t * body_font)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "rcc_faults_mask_#");
        lv_obj_set_x(lv_obj_0, card_x);
        lv_obj_set_y(lv_obj_0, card_y);
        lv_obj_set_width(lv_obj_0, 200);
        lv_obj_set_height(lv_obj_0, card_height);
        lv_obj_set_clickable(lv_obj_0, false);
        lv_obj_set_scrollable(lv_obj_0, false);
        lv_obj_set_style_bg_color(lv_obj_0, lv_color_hex(0x111722), 0);
        lv_obj_set_style_bg_opa(lv_obj_0, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(lv_obj_0, 10, 0);
        lv_obj_set_style_border_width(lv_obj_0, 1, 0);
        lv_obj_set_style_outline_width(lv_obj_0, 0, 0);
        lv_obj_set_style_shadow_width(lv_obj_0, 0, 0);
        lv_obj_set_style_pad_all(lv_obj_0, -1, 0);
        lv_obj_set_style_border_color(lv_obj_0, lv_color_hex(0x1e293b), 0);
        lv_obj_set_style_border_opa(lv_obj_0, (255 * 100 / 100), 0);

        lv_obj_t * lv_label_0 = lv_label_create(lv_obj_0);
        lv_obj_set_x(lv_label_0, 13);
        lv_obj_set_y(lv_label_0, 8);
        lv_label_set_text(lv_label_0, list_title);
        lv_obj_set_style_text_font(lv_label_0, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_0, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_clickable(lv_label_0, false);
        lv_obj_set_scrollable(lv_label_0, false);
        lv_obj_set_style_text_letter_space(lv_label_0, 1, 0);

        lv_obj_t * fault_mask_value = lv_label_create(lv_obj_0);
        lv_obj_set_x(fault_mask_value, 81);
        lv_obj_set_y(fault_mask_value, 8);
        lv_label_set_text(fault_mask_value, "0x0000");
        lv_obj_set_style_text_font(fault_mask_value, caption_font, 0);
        lv_obj_set_style_text_color(fault_mask_value, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_clickable(fault_mask_value, false);
        lv_obj_set_scrollable(fault_mask_value, false);
        lv_obj_set_width(fault_mask_value, 106);
        lv_obj_set_height(fault_mask_value, 13);
        lv_label_set_long_mode(fault_mask_value, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(fault_mask_value, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_set_name(fault_mask_value, "fault_mask_value");

        lv_obj_t * fault_mask_empty = lv_label_create(lv_obj_0);
        lv_obj_set_x(fault_mask_empty, 13);
        lv_obj_set_y(fault_mask_empty, 24);
        lv_label_set_text(fault_mask_empty, "None (0x0)");
        lv_obj_set_style_text_font(fault_mask_empty, body_font, 0);
        lv_obj_set_style_text_color(fault_mask_empty, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_clickable(fault_mask_empty, false);
        lv_obj_set_scrollable(fault_mask_empty, false);
        lv_obj_set_width(fault_mask_empty, 174);
        lv_obj_set_height(fault_mask_empty, 15);
        lv_label_set_long_mode(fault_mask_empty, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(fault_mask_empty, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_name(fault_mask_empty, "fault_mask_empty");

        lv_obj_t * fault_mask_viewport = lv_obj_create(lv_obj_0);
        lv_obj_set_x(fault_mask_viewport, 0);
        lv_obj_set_y(fault_mask_viewport, 23);
        lv_obj_set_width(fault_mask_viewport, 200);
        lv_obj_set_height(fault_mask_viewport, card_height - 26);
        lv_obj_set_clickable(fault_mask_viewport, false);
        lv_obj_set_scrollable(fault_mask_viewport, false);
        lv_obj_set_style_bg_color(fault_mask_viewport, lv_color_hex(0x000000), 0);
        lv_obj_set_style_bg_opa(fault_mask_viewport, (255 * 0 / 100), 0);
        lv_obj_set_style_radius(fault_mask_viewport, 0, 0);
        lv_obj_set_style_border_width(fault_mask_viewport, 0, 0);
        lv_obj_set_style_outline_width(fault_mask_viewport, 0, 0);
        lv_obj_set_style_shadow_width(fault_mask_viewport, 0, 0);
        lv_obj_set_style_pad_all(fault_mask_viewport, 0, 0);
        lv_obj_set_name(fault_mask_viewport, "fault_mask_viewport");

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

