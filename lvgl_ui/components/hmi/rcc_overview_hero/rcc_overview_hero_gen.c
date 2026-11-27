/**
 * @file rcc_overview_hero_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "rcc_overview_hero_gen.h"
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

lv_obj_t * rcc_overview_hero_create(lv_obj_t * parent, int32_t card_height, lv_font_t * heading, lv_font_t * small, const char * state_text, const char * sub_text, int32_t summary_x)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "rcc_overview_hero_#");
        lv_obj_set_scrollable(lv_obj_0, false);
        lv_obj_set_clickable(lv_obj_0, false);
        lv_obj_set_x(lv_obj_0, 0);
        lv_obj_set_y(lv_obj_0, 0);
        lv_obj_set_width(lv_obj_0, 408);
        lv_obj_set_height(lv_obj_0, card_height);
        lv_obj_set_style_bg_color(lv_obj_0, lv_color_hex(0x111722), 0);
        lv_obj_set_style_bg_opa(lv_obj_0, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(lv_obj_0, 10, 0);
        lv_obj_set_style_border_width(lv_obj_0, 1, 0);
        lv_obj_set_style_border_color(lv_obj_0, lv_color_hex(0x1e293b), 0);
        lv_obj_set_style_border_opa(lv_obj_0, (255 * 100 / 100), 0);
        lv_obj_set_style_outline_width(lv_obj_0, 0, 0);
        lv_obj_set_style_shadow_width(lv_obj_0, 0, 0);
        lv_obj_set_style_pad_all(lv_obj_0, -1, 0);

        lv_obj_t * state = lv_label_create(lv_obj_0);
        lv_obj_set_x(state, 13);
        lv_obj_set_y(state, 12);
        lv_label_set_text(state, state_text);
        lv_obj_set_scrollable(state, false);
        lv_obj_set_clickable(state, false);
        lv_obj_set_style_text_font(state, heading, 0);
        lv_obj_set_style_text_color(state, lv_color_hex(0xffffff), 0);
        lv_obj_set_name(state, "state");

        lv_obj_t * summary = lv_label_create(lv_obj_0);
        lv_obj_set_x(summary, summary_x);
        lv_obj_set_y(summary, 22);
        lv_label_set_text(summary, sub_text);
        lv_obj_set_scrollable(summary, false);
        lv_obj_set_clickable(summary, false);
        lv_obj_set_style_text_font(summary, small, 0);
        lv_obj_set_style_text_color(summary, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_width(summary, 395 - summary_x);
        lv_obj_set_height(summary, 13);
        lv_label_set_long_mode(summary, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(summary, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_set_name(summary, "summary");

        lv_obj_t * lv_obj_1 = lv_obj_create(lv_obj_0);
        lv_obj_set_x(lv_obj_1, 13);
        lv_obj_set_y(lv_obj_1, 42);
        lv_obj_set_width(lv_obj_1, 382);
        lv_obj_set_height(lv_obj_1, 1);
        lv_obj_set_clickable(lv_obj_1, false);
        lv_obj_set_scrollable(lv_obj_1, false);
        lv_obj_set_style_bg_color(lv_obj_1, lv_color_hex(0x1e293b), 0);
        lv_obj_set_style_bg_opa(lv_obj_1, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(lv_obj_1, 0, 0);
        lv_obj_set_style_border_width(lv_obj_1, 0, 0);
        lv_obj_set_style_outline_width(lv_obj_1, 0, 0);
        lv_obj_set_style_shadow_width(lv_obj_1, 0, 0);
        lv_obj_set_style_pad_all(lv_obj_1, 0, 0);

        lv_obj_t * steps = lv_obj_create(lv_obj_0);
        lv_obj_set_name(steps, "steps");
        lv_obj_set_x(steps, 0);
        lv_obj_set_y(steps, 0);
        lv_obj_set_width(steps, 408);
        lv_obj_set_height(steps, card_height);
        lv_obj_set_scrollable(steps, false);
        lv_obj_set_clickable(steps, false);
        lv_obj_set_style_bg_opa(steps, (255 * 0 / 100), 0);
        lv_obj_set_style_border_width(steps, 0, 0);
        lv_obj_set_style_outline_width(steps, 0, 0);
        lv_obj_set_style_shadow_width(steps, 0, 0);
        lv_obj_set_style_pad_all(steps, 0, 0);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

lv_obj_t * rcc_overview_hero_get_steps(lv_obj_t *parent)
{
    return lv_obj_find_by_name(parent, "steps");
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

