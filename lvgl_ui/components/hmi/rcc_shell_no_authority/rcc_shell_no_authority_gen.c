/**
 * @file rcc_shell_no_authority_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "rcc_shell_no_authority_gen.h"
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

lv_obj_t * rcc_shell_no_authority_create(lv_obj_t * parent, const void * glyph, lv_font_t * title_font, lv_font_t * body_font)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "rcc_shell_no_authority_#");
        lv_obj_set_x(lv_obj_0, 0);
        lv_obj_set_y(lv_obj_0, 0);
        lv_obj_set_width(lv_obj_0, 408);
        lv_obj_set_height(lv_obj_0, 42);
        lv_obj_set_scrollable(lv_obj_0, false);
        lv_obj_set_clickable(lv_obj_0, false);
        lv_obj_set_style_pad_all(lv_obj_0, -1, 0);
        lv_obj_set_style_bg_color(lv_obj_0, lv_color_hex(0x2b2309), 0);
        lv_obj_set_style_bg_opa(lv_obj_0, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(lv_obj_0, 10, 0);
        lv_obj_set_style_border_width(lv_obj_0, 1, 0);
        lv_obj_set_style_outline_width(lv_obj_0, 0, 0);
        lv_obj_set_style_shadow_width(lv_obj_0, 0, 0);
        lv_obj_set_style_border_color(lv_obj_0, lv_color_hex(0xfacc15), 0);

        lv_obj_t * lv_image_0 = lv_image_create(lv_obj_0);
        lv_obj_set_x(lv_image_0, 12);
        lv_obj_set_y(lv_image_0, 11);
        lv_image_set_src(lv_image_0, glyph);
        lv_obj_set_clickable(lv_image_0, false);
        lv_obj_set_scrollable(lv_image_0, false);
        lv_obj_set_style_image_recolor(lv_image_0, lv_color_hex(0xfacc15), 0);
        lv_obj_set_style_image_recolor_opa(lv_image_0, (255 * 100 / 100), 0);

        lv_obj_t * lv_label_0 = lv_label_create(lv_obj_0);
        lv_obj_set_x(lv_label_0, 42);
        lv_obj_set_y(lv_label_0, 8);
        lv_label_set_text(lv_label_0, "No control authority — monitoring only");
        lv_obj_set_scrollable(lv_label_0, false);
        lv_obj_set_clickable(lv_label_0, false);
        lv_obj_set_style_text_font(lv_label_0, title_font, 0);
        lv_obj_set_style_text_color(lv_label_0, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_letter_space(lv_label_0, 0, 0);
        lv_obj_set_width(lv_label_0, 354);
        lv_obj_set_height(lv_label_0, 13);
        lv_label_set_long_mode(lv_label_0, LV_LABEL_LONG_MODE_DOTS);

        lv_obj_t * lv_label_1 = lv_label_create(lv_obj_0);
        lv_obj_set_x(lv_label_1, 42);
        lv_obj_set_y(lv_label_1, 22);
        lv_label_set_text(lv_label_1, "Use physical E-stop. Grant control on the controller service port.");
        lv_obj_set_scrollable(lv_label_1, false);
        lv_obj_set_clickable(lv_label_1, false);
        lv_obj_set_style_text_font(lv_label_1, body_font, 0);
        lv_obj_set_style_text_color(lv_label_1, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_style_text_align(lv_label_1, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_letter_space(lv_label_1, 0, 0);
        lv_obj_set_width(lv_label_1, 354);
        lv_obj_set_height(lv_label_1, 13);
        lv_label_set_long_mode(lv_label_1, LV_LABEL_LONG_MODE_DOTS);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

