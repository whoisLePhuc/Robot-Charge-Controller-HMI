/**
 * @file rcc_faults_summary_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "rcc_faults_summary_gen.h"
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

lv_obj_t * rcc_faults_summary_create(lv_obj_t * parent, lv_font_t * caption_font, lv_font_t * title_font, const void * badge_icon)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "rcc_faults_summary_#");
        lv_obj_set_x(lv_obj_0, 0);
        lv_obj_set_y(lv_obj_0, 0);
        lv_obj_set_width(lv_obj_0, 408);
        lv_obj_set_height(lv_obj_0, 40);
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

        lv_obj_t * fault_summary_icon = lv_image_create(lv_obj_0);
        lv_obj_set_x(fault_summary_icon, 13);
        lv_obj_set_y(fault_summary_icon, 11);
        lv_image_set_src(fault_summary_icon, badge_icon);
        lv_obj_set_style_image_recolor(fault_summary_icon, lv_color_hex(0x22c55e), 0);
        lv_obj_set_style_image_recolor_opa(fault_summary_icon, (255 * 100 / 100), 0);
        lv_obj_set_clickable(fault_summary_icon, false);
        lv_obj_set_scrollable(fault_summary_icon, false);
        lv_obj_set_name(fault_summary_icon, "fault_summary_icon");

        lv_obj_t * fault_summary_title = lv_label_create(lv_obj_0);
        lv_obj_set_x(fault_summary_title, 41);
        lv_obj_set_y(fault_summary_title, 12);
        lv_label_set_text(fault_summary_title, "All Systems Nominal — No Faults");
        lv_obj_set_style_text_font(fault_summary_title, title_font, 0);
        lv_obj_set_style_text_color(fault_summary_title, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_clickable(fault_summary_title, false);
        lv_obj_set_scrollable(fault_summary_title, false);
        lv_obj_set_width(fault_summary_title, 250);
        lv_obj_set_height(fault_summary_title, 16);
        lv_label_set_long_mode(fault_summary_title, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(fault_summary_title, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_name(fault_summary_title, "fault_summary_title");

        lv_obj_t * fault_summary_age = lv_label_create(lv_obj_0);
        lv_obj_set_x(fault_summary_age, 290);
        lv_obj_set_y(fault_summary_age, 13);
        lv_label_set_text(fault_summary_age, "Read 0.8 s ago");
        lv_obj_set_style_text_font(fault_summary_age, caption_font, 0);
        lv_obj_set_style_text_color(fault_summary_age, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_clickable(fault_summary_age, false);
        lv_obj_set_scrollable(fault_summary_age, false);
        lv_obj_set_width(fault_summary_age, 105);
        lv_obj_set_height(fault_summary_age, 13);
        lv_label_set_long_mode(fault_summary_age, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(fault_summary_age, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_set_name(fault_summary_age, "fault_summary_age");

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

