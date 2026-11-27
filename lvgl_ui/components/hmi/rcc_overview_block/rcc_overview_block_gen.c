/**
 * @file rcc_overview_block_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "rcc_overview_block_gen.h"
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

lv_obj_t * rcc_overview_block_create(lv_obj_t * parent, int32_t page_height, lv_font_t * small)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "rcc_overview_block_#");
        lv_obj_set_scrollable(lv_obj_0, false);
        lv_obj_set_clickable(lv_obj_0, false);
        lv_obj_set_x(lv_obj_0, 164);
        lv_obj_set_y(lv_obj_0, page_height - 21);
        lv_obj_set_width(lv_obj_0, 244);
        lv_obj_set_height(lv_obj_0, 19);
        lv_obj_set_style_bg_color(lv_obj_0, lv_color_hex(0x2b2309), 0);
        lv_obj_set_style_bg_opa(lv_obj_0, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(lv_obj_0, 6, 0);
        lv_obj_set_style_border_width(lv_obj_0, 1, 0);
        lv_obj_set_style_border_color(lv_obj_0, lv_color_hex(0xfacc15), 0);
        lv_obj_set_style_border_opa(lv_obj_0, (255 * 100 / 100), 0);
        lv_obj_set_style_outline_width(lv_obj_0, 0, 0);
        lv_obj_set_style_shadow_width(lv_obj_0, 0, 0);
        lv_obj_set_style_pad_all(lv_obj_0, -1, 0);

        lv_obj_t * reason = lv_label_create(lv_obj_0);
        lv_obj_set_x(reason, 0);
        lv_obj_set_y(reason, 3);
        lv_label_set_text(reason, "START blocked");
        lv_obj_set_scrollable(reason, false);
        lv_obj_set_clickable(reason, false);
        lv_obj_set_style_text_font(reason, small, 0);
        lv_obj_set_style_text_color(reason, lv_color_hex(0xfacc15), 0);
        lv_obj_set_width(reason, 244);
        lv_obj_set_height(reason, 13);
        lv_label_set_long_mode(reason, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(reason, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_name(reason, "reason");

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

