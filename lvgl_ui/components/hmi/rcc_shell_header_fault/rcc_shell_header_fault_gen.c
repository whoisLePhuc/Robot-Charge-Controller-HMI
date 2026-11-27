/**
 * @file rcc_shell_header_fault_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "rcc_shell_header_fault_gen.h"
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

lv_obj_t * rcc_shell_header_fault_create(lv_obj_t * parent, const void * glyph)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_button_0 = lv_button_create(parent);
        lv_obj_set_name_static(lv_button_0, "rcc_shell_header_fault_#");
        lv_obj_set_x(lv_button_0, 438);
        lv_obj_set_y(lv_button_0, 4);
        lv_obj_set_width(lv_button_0, 34);
        lv_obj_set_height(lv_button_0, 24);
        lv_obj_set_scrollable(lv_button_0, false);
        lv_obj_set_clickable(lv_button_0, true);
        lv_obj_set_style_pad_all(lv_button_0, -1, 0);
        lv_obj_set_style_bg_color(lv_button_0, lv_color_hex(0x111722), 0);
        lv_obj_set_style_bg_opa(lv_button_0, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(lv_button_0, 8, 0);
        lv_obj_set_style_border_width(lv_button_0, 1, 0);
        lv_obj_set_style_outline_width(lv_button_0, 0, 0);
        lv_obj_set_style_shadow_width(lv_button_0, 0, 0);
        lv_obj_set_style_border_color(lv_button_0, lv_color_hex(0x1e293b), 0);
        lv_obj_set_style_bg_opa(lv_button_0, 178, LV_STATE_PRESSED);

        lv_obj_t * fault_icon = lv_image_create(lv_button_0);
        lv_obj_set_x(fault_icon, 9);
        lv_obj_set_y(fault_icon, 5);
        lv_image_set_src(fault_icon, glyph);
        lv_obj_set_clickable(fault_icon, false);
        lv_obj_set_scrollable(fault_icon, false);
        lv_obj_set_style_image_recolor(fault_icon, lv_color_hex(0x64748b), 0);
        lv_obj_set_style_image_recolor_opa(fault_icon, (255 * 100 / 100), 0);
        lv_obj_set_name(fault_icon, "fault_icon");

        the_root = lv_button_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

