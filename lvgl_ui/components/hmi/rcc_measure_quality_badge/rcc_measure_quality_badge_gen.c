/**
 * @file rcc_measure_quality_badge_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "rcc_measure_quality_badge_gen.h"
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

lv_obj_t * rcc_measure_quality_badge_create(lv_obj_t * parent, int32_t badge_x, int32_t badge_y, int32_t mark_x, int32_t mark_y, const void * mark, lv_color_t mark_color, lv_color_t badge_color)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "rcc_measure_quality_badge_#");
        lv_obj_set_scrollable(lv_obj_0, false);
        lv_obj_set_clickable(lv_obj_0, false);
        lv_obj_set_x(lv_obj_0, badge_x);
        lv_obj_set_y(lv_obj_0, badge_y);
        lv_obj_set_width(lv_obj_0, 18);
        lv_obj_set_height(lv_obj_0, 16);
        lv_obj_set_style_bg_opa(lv_obj_0, (255 * 100 / 100), 0);
        lv_obj_set_style_border_width(lv_obj_0, 0, 0);
        lv_obj_set_style_outline_width(lv_obj_0, 0, 0);
        lv_obj_set_style_shadow_width(lv_obj_0, 0, 0);
        lv_obj_set_style_pad_all(lv_obj_0, 0, 0);
        lv_obj_set_style_bg_color(lv_obj_0, badge_color, 0);
        lv_obj_set_style_radius(lv_obj_0, 4, 0);

        lv_obj_t * lv_image_0 = lv_image_create(lv_obj_0);
        lv_obj_set_x(lv_image_0, mark_x);
        lv_obj_set_y(lv_image_0, mark_y);
        lv_image_set_src(lv_image_0, mark);
        lv_obj_set_clickable(lv_image_0, false);
        lv_obj_set_scrollable(lv_image_0, false);
        lv_obj_set_style_image_recolor(lv_image_0, mark_color, 0);
        lv_obj_set_style_image_recolor_opa(lv_image_0, (255 * 100 / 100), 0);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

