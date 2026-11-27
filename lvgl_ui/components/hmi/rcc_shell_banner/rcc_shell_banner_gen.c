/**
 * @file rcc_shell_banner_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "rcc_shell_banner_gen.h"
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

lv_obj_t * rcc_shell_banner_create(lv_obj_t * parent, const void * glyph, lv_font_t * font)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_button_0 = lv_button_create(parent);
        lv_obj_set_name_static(lv_button_0, "rcc_shell_banner_#");
        lv_obj_set_x(lv_button_0, 64);
        lv_obj_set_y(lv_button_0, 32);
        lv_obj_set_width(lv_button_0, 408);
        lv_obj_set_height(lv_button_0, 20);
        lv_obj_set_scrollable(lv_button_0, false);
        lv_obj_set_clickable(lv_button_0, true);
        lv_obj_set_style_pad_all(lv_button_0, -1, 0);
        lv_obj_set_style_bg_color(lv_button_0, lv_color_hex(0x161e2b), 0);
        lv_obj_set_style_bg_opa(lv_button_0, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(lv_button_0, 6, 0);
        lv_obj_set_style_border_width(lv_button_0, 1, 0);
        lv_obj_set_style_outline_width(lv_button_0, 0, 0);
        lv_obj_set_style_shadow_width(lv_button_0, 0, 0);
        lv_obj_set_style_border_color(lv_button_0, lv_color_hex(0xffffff), 0);
        lv_obj_set_style_bg_opa(lv_button_0, 178, LV_STATE_PRESSED);
        lv_obj_set_style_border_opa(lv_button_0, 26, 0);

        lv_obj_t * banner_icon = lv_image_create(lv_button_0);
        lv_obj_set_x(banner_icon, 0);
        lv_obj_set_y(banner_icon, 0);
        lv_image_set_src(banner_icon, glyph);
        lv_obj_set_clickable(banner_icon, false);
        lv_obj_set_scrollable(banner_icon, false);
        lv_obj_set_style_image_recolor(banner_icon, lv_color_hex(0xfacc15), 0);
        lv_obj_set_style_image_recolor_opa(banner_icon, (255 * 100 / 100), 0);
        lv_obj_set_name(banner_icon, "banner_icon");

        lv_obj_t * banner_text = lv_label_create(lv_button_0);
        lv_obj_set_x(banner_text, 0);
        lv_obj_set_y(banner_text, 0);
        lv_label_set_text(banner_text, "Controller offline");
        lv_obj_set_scrollable(banner_text, false);
        lv_obj_set_clickable(banner_text, false);
        lv_obj_set_style_text_font(banner_text, font, 0);
        lv_obj_set_style_text_color(banner_text, lv_color_hex(0xfacc15), 0);
        lv_obj_set_style_text_align(banner_text, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_letter_space(banner_text, 0, 0);
        lv_obj_set_name(banner_text, "banner_text");

        the_root = lv_button_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

