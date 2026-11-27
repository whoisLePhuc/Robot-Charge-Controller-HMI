/**
 * @file rcc_session_detail_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "rcc_session_detail_gen.h"
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

lv_obj_t * rcc_session_detail_create(lv_obj_t * parent, int32_t detail_x, int32_t detail_y, int32_t detail_width, const char * text, lv_font_t * small)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_label_0 = lv_label_create(parent);
        lv_obj_set_name_static(lv_label_0, "rcc_session_detail_#");
        lv_obj_set_scrollable(lv_label_0, false);
        lv_obj_set_clickable(lv_label_0, false);
        lv_obj_set_x(lv_label_0, detail_x);
        lv_obj_set_y(lv_label_0, detail_y);
        lv_obj_set_width(lv_label_0, detail_width);
        lv_obj_set_height(lv_label_0, 13);
        lv_label_set_text(lv_label_0, text);
        lv_label_set_long_mode(lv_label_0, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_font(lv_label_0, small, 0);
        lv_obj_set_style_text_color(lv_label_0, lv_color_hex(0xcbd5e1), 0);
        lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_RIGHT, 0);

        the_root = lv_label_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

