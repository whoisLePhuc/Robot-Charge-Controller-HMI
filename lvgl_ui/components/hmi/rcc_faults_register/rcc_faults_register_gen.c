/**
 * @file rcc_faults_register_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "rcc_faults_register_gen.h"
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

lv_obj_t * rcc_faults_register_create(lv_obj_t * parent, int32_t card_x, int32_t card_height, const char * register_title, const char * register_mask, const char * register_value, const char * register_note, lv_color_t value_color, lv_font_t * caption_font, lv_font_t * title_font)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "rcc_faults_register_#");
        lv_obj_set_x(lv_obj_0, card_x);
        lv_obj_set_y(lv_obj_0, 46);
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
        lv_label_set_text(lv_label_0, register_title);
        lv_obj_set_style_text_font(lv_label_0, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_0, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_clickable(lv_label_0, false);
        lv_obj_set_scrollable(lv_label_0, false);
        lv_obj_set_style_text_letter_space(lv_label_0, 1, 0);

        lv_obj_t * lv_label_1 = lv_label_create(lv_obj_0);
        lv_obj_set_x(lv_label_1, 111);
        lv_obj_set_y(lv_label_1, 8);
        lv_label_set_text(lv_label_1, register_mask);
        lv_obj_set_style_text_font(lv_label_1, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_1, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_clickable(lv_label_1, false);
        lv_obj_set_scrollable(lv_label_1, false);
        lv_obj_set_width(lv_label_1, 76);
        lv_obj_set_height(lv_label_1, 13);
        lv_label_set_long_mode(lv_label_1, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_1, LV_TEXT_ALIGN_RIGHT, 0);

        lv_obj_t * lv_label_2 = lv_label_create(lv_obj_0);
        lv_obj_set_x(lv_label_2, 13);
        lv_obj_set_y(lv_label_2, 23 + (card_height - 60) / 2);
        lv_label_set_text(lv_label_2, register_value);
        lv_obj_set_style_text_font(lv_label_2, title_font, 0);
        lv_obj_set_style_text_color(lv_label_2, value_color, 0);
        lv_obj_set_clickable(lv_label_2, false);
        lv_obj_set_scrollable(lv_label_2, false);
        lv_obj_set_width(lv_label_2, 174);
        lv_obj_set_height(lv_label_2, 16);
        lv_label_set_long_mode(lv_label_2, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_2, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * lv_label_3 = lv_label_create(lv_obj_0);
        lv_obj_set_x(lv_label_3, 13);
        lv_obj_set_y(lv_label_3, card_height - 20);
        lv_label_set_text(lv_label_3, register_note);
        lv_obj_set_style_text_font(lv_label_3, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_3, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_clickable(lv_label_3, false);
        lv_obj_set_scrollable(lv_label_3, false);
        lv_obj_set_width(lv_label_3, 174);
        lv_obj_set_height(lv_label_3, 13);
        lv_label_set_long_mode(lv_label_3, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_3, LV_TEXT_ALIGN_LEFT, 0);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

