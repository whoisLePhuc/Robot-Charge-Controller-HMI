/**
 * @file rcc_session_events_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "rcc_session_events_gen.h"
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

lv_obj_t * rcc_session_events_create(lv_obj_t * parent, int32_t event_height, lv_font_t * small, lv_font_t * bold, lv_font_t * body_font, const void * sync_icon, const void * offline_icon, int32_t sync_icon_x, int32_t sync_text_x)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "rcc_session_events_#");
        lv_obj_set_scrollable(lv_obj_0, false);
        lv_obj_set_clickable(lv_obj_0, false);
        lv_obj_set_x(lv_obj_0, 0);
        lv_obj_set_y(lv_obj_0, 45);
        lv_obj_set_width(lv_obj_0, 408);
        lv_obj_set_height(lv_obj_0, event_height);
        lv_obj_set_style_bg_color(lv_obj_0, lv_color_hex(0x111722), 0);
        lv_obj_set_style_bg_opa(lv_obj_0, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(lv_obj_0, 10, 0);
        lv_obj_set_style_border_width(lv_obj_0, 1, 0);
        lv_obj_set_style_border_color(lv_obj_0, lv_color_hex(0x1e293b), 0);
        lv_obj_set_style_border_opa(lv_obj_0, (255 * 100 / 100), 0);
        lv_obj_set_style_outline_width(lv_obj_0, 0, 0);
        lv_obj_set_style_shadow_width(lv_obj_0, 0, 0);
        lv_obj_set_style_pad_all(lv_obj_0, -1, 0);

        lv_obj_t * event_heading = lv_label_create(lv_obj_0);
        lv_obj_set_x(event_heading, 11);
        lv_obj_set_y(event_heading, 10);
        lv_label_set_text(event_heading, "EVENT LOG");
        lv_obj_set_scrollable(event_heading, false);
        lv_obj_set_clickable(event_heading, false);
        lv_obj_set_style_text_font(event_heading, small, 0);
        lv_obj_set_style_text_color(event_heading, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_style_text_letter_space(event_heading, 1, 0);
        lv_obj_set_name(event_heading, "event_heading");

        lv_obj_t * sync = lv_button_create(lv_obj_0);
        lv_obj_set_name(sync, "sync");
        lv_obj_set_x(sync, 330);
        lv_obj_set_y(sync, 6);
        lv_obj_set_width(sync, 67);
        lv_obj_set_height(sync, 22);
        lv_obj_set_scrollable(sync, false);
        lv_obj_set_style_bg_color(sync, lv_color_hex(0x0c2538), 0);
        lv_obj_set_style_bg_opa(sync, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(sync, 8, 0);
        lv_obj_set_style_border_width(sync, 1, 0);
        lv_obj_set_style_border_color(sync, lv_color_hex(0x1e4963), 0);
        lv_obj_set_style_border_opa(sync, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_opa(sync, 178, LV_STATE_PRESSED);
        lv_obj_set_style_outline_width(sync, 0, 0);
        lv_obj_set_style_shadow_width(sync, 0, 0);
        lv_obj_set_style_pad_all(sync, 0, 0);
        lv_obj_t * lv_image_0 = lv_image_create(sync);
        lv_obj_set_x(lv_image_0, sync_icon_x);
        lv_obj_set_y(lv_image_0, 4);
        lv_image_set_src(lv_image_0, sync_icon);
        lv_obj_set_clickable(lv_image_0, false);
        lv_obj_set_scrollable(lv_image_0, false);
        lv_obj_set_style_image_recolor(lv_image_0, lv_color_hex(0x38bdf8), 0);
        lv_obj_set_style_image_recolor_opa(lv_image_0, (255 * 100 / 100), 0);

        lv_obj_t * lv_label_0 = lv_label_create(sync);
        lv_obj_set_x(lv_label_0, sync_text_x);
        lv_obj_set_y(lv_label_0, 3);
        lv_label_set_text(lv_label_0, "SYNC");
        lv_obj_set_scrollable(lv_label_0, false);
        lv_obj_set_clickable(lv_label_0, false);
        lv_obj_set_style_text_font(lv_label_0, small, 0);
        lv_obj_set_style_text_color(lv_label_0, lv_color_hex(0x38bdf8), 0);
        lv_obj_set_style_text_letter_space(lv_label_0, 1, 0);

        lv_obj_t * event_offline_icon = lv_image_create(lv_obj_0);
        lv_obj_set_x(event_offline_icon, 90);
        lv_obj_set_y(event_offline_icon, 34 + (event_height - 50) / 2);
        lv_image_set_src(event_offline_icon, offline_icon);
        lv_obj_set_clickable(event_offline_icon, false);
        lv_obj_set_scrollable(event_offline_icon, false);
        lv_obj_set_style_image_recolor(event_offline_icon, lv_color_hex(0xfacc15), 0);
        lv_obj_set_style_image_recolor_opa(event_offline_icon, (255 * 100 / 100), 0);
        lv_obj_set_name(event_offline_icon, "event_offline_icon");

        lv_obj_t * event_offline_text = lv_label_create(lv_obj_0);
        lv_obj_set_x(event_offline_text, 112);
        lv_obj_set_y(event_offline_text, 34 + (event_height - 47) / 2);
        lv_label_set_text(event_offline_text, "Event log unavailable — UART link lost");
        lv_obj_set_scrollable(event_offline_text, false);
        lv_obj_set_clickable(event_offline_text, false);
        lv_obj_set_style_text_font(event_offline_text, bold, 0);
        lv_obj_set_style_text_color(event_offline_text, lv_color_hex(0xfacc15), 0);
        lv_obj_set_width(event_offline_text, 290);
        lv_obj_set_height(event_offline_text, 13);
        lv_label_set_long_mode(event_offline_text, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(event_offline_text, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_name(event_offline_text, "event_offline_text");

        lv_obj_t * event_empty = lv_label_create(lv_obj_0);
        lv_obj_set_x(event_empty, 11);
        lv_obj_set_y(event_empty, 36);
        lv_label_set_text(event_empty, "No events held by the controller");
        lv_obj_set_scrollable(event_empty, false);
        lv_obj_set_clickable(event_empty, false);
        lv_obj_set_style_text_font(event_empty, body_font, 0);
        lv_obj_set_style_text_color(event_empty, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_width(event_empty, 386);
        lv_obj_set_height(event_empty, 15);
        lv_label_set_long_mode(event_empty, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(event_empty, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_name(event_empty, "event_empty");

        lv_obj_t * event_rows = lv_obj_create(lv_obj_0);
        lv_obj_set_name(event_rows, "event_rows");
        lv_obj_set_x(event_rows, 0);
        lv_obj_set_y(event_rows, 34);
        lv_obj_set_width(event_rows, 408);
        lv_obj_set_height(event_rows, event_height - 37);
        lv_obj_set_scrollable(event_rows, false);
        lv_obj_set_clickable(event_rows, false);
        lv_obj_set_style_bg_opa(event_rows, (255 * 0 / 100), 0);
        lv_obj_set_style_border_width(event_rows, 0, 0);
        lv_obj_set_style_outline_width(event_rows, 0, 0);
        lv_obj_set_style_shadow_width(event_rows, 0, 0);
        lv_obj_set_style_pad_all(event_rows, 0, 0);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

