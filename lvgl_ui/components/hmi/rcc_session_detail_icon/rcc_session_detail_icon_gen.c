/**
 * @file rcc_session_detail_icon_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "rcc_session_detail_icon_gen.h"
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

lv_obj_t * rcc_session_detail_icon_create(lv_obj_t * parent, int32_t detail_x, int32_t detail_y, const void * mark)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_image_0 = lv_image_create(parent);
        lv_obj_set_name_static(lv_image_0, "rcc_session_detail_icon_#");
        lv_obj_set_scrollable(lv_image_0, false);
        lv_obj_set_clickable(lv_image_0, false);
        lv_obj_set_x(lv_image_0, detail_x);
        lv_obj_set_y(lv_image_0, detail_y);
        lv_image_set_src(lv_image_0, mark);
        lv_obj_set_style_image_recolor(lv_image_0, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_style_image_recolor_opa(lv_image_0, (255 * 100 / 100), 0);

        the_root = lv_image_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

