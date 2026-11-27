/**
 * @file hmi_wifi_taps_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "hmi_wifi_taps_gen.h"
#include "../lvgl_ui.h"

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

lv_obj_t * hmi_wifi_taps_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
        lv_obj_set_name_static(lv_obj_0, "hmi_wifi_taps_#");
        lv_obj_set_style_bg_color(lv_obj_0, lv_color_hex(0x090d14), 0);
        lv_obj_set_style_bg_opa(lv_obj_0, (255 * 100 / 100), 0);
        lv_obj_set_style_pad_all(lv_obj_0, 0, 0);
        lv_obj_set_style_border_width(lv_obj_0, 0, 0);
        lv_obj_set_scrollable(lv_obj_0, false);

        lv_obj_t * rcc_wifi_header_0 = rcc_wifi_header_create(lv_obj_0, "MSVN-Office", "Secured · Signal -61 dBm", hmi_f14b, hmi_f10, hmi_icon_back_14);
        lv_obj_t * rcc_wifi_header_0_back = rcc_wifi_header_get_back(rcc_wifi_header_0);
        if (rcc_wifi_header_0_back) {
        lv_obj_add_screen_create_event(rcc_wifi_header_0_back, LV_EVENT_CLICKED, hmi_wifi_list_create, LV_SCREEN_LOAD_ANIM_NONE, 0, 0);
        } else {
        LV_LOG_WARN("`back` slot of `rcc_wifi_header_0` doesn't exist");
        }

        lv_obj_t * rcc_wifi_pass_0 = rcc_wifi_pass_create(lv_obj_0, false, false, "3 / 63", "?123", hmi_f10, hmi_f12b, hmi_icon_eyeoff_14, hmi_icon_shift_14, hmi_icon_bksp_16);
        lv_obj_t * rcc_wifi_pass_0_password_field = rcc_wifi_pass_get_password_field(rcc_wifi_pass_0);
        if (rcc_wifi_pass_0_password_field) {
        lv_obj_t * lv_label_0 = lv_label_create(rcc_wifi_pass_0_password_field);
        lv_obj_set_x(lv_label_0, 12);
        lv_obj_set_y(lv_label_0, 9);
        lv_obj_set_width(lv_label_0, 31);
        lv_obj_set_height(lv_label_0, 16);
        lv_obj_set_clickable(lv_label_0, false);
        lv_obj_set_style_text_font(lv_label_0, hmi_f14b, 0);
        lv_obj_set_style_text_color(lv_label_0, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_scrollable(lv_label_0, false);
        lv_label_set_text(lv_label_0, "Qqa");
        lv_label_set_long_mode(lv_label_0, LV_LABEL_LONG_MODE_WRAP);
        lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * lv_obj_1 = lv_obj_create(rcc_wifi_pass_0_password_field);
        lv_obj_set_x(lv_obj_1, 45);
        lv_obj_set_y(lv_obj_1, 9);
        lv_obj_set_width(lv_obj_1, 2);
        lv_obj_set_height(lv_obj_1, 16);
        lv_obj_set_clickable(lv_obj_1, false);
        lv_obj_set_style_radius(lv_obj_1, 1, 0);
        lv_obj_set_style_border_width(lv_obj_1, 0, 0);
        lv_obj_set_style_bg_opa(lv_obj_1, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(lv_obj_1, lv_color_hex(0x38bdf8), 0);
        lv_obj_set_style_pad_top(lv_obj_1, 0, 0);
        lv_obj_set_style_pad_bottom(lv_obj_1, 0, 0);
        lv_obj_set_style_pad_left(lv_obj_1, 0, 0);
        lv_obj_set_style_pad_right(lv_obj_1, 0, 0);
        lv_obj_set_scrollable(lv_obj_1, false);
        lv_obj_set_style_shadow_width(lv_obj_1, 0, 0);
        lv_obj_set_style_outline_width(lv_obj_1, 0, 0);
        } else {
        LV_LOG_WARN("`password_field` slot of `rcc_wifi_pass_0` doesn't exist");
        }

        lv_obj_t * rcc_wifi_pass_0_password_eye_icon = rcc_wifi_pass_get_password_eye_icon(rcc_wifi_pass_0);
        if (rcc_wifi_pass_0_password_eye_icon) {
        lv_obj_set_style_image_recolor(rcc_wifi_pass_0_password_eye_icon, lv_color_hex(0x38bdf8), 0);
        } else {
        LV_LOG_WARN("`password_eye_icon` slot of `rcc_wifi_pass_0` doesn't exist");
        }

        lv_obj_t * rcc_wifi_pass_0_key_symbols = rcc_wifi_pass_get_key_symbols(rcc_wifi_pass_0);
        if (rcc_wifi_pass_0_key_symbols) {
        lv_obj_add_screen_create_event(rcc_wifi_pass_0_key_symbols, LV_EVENT_CLICKED, hmi_wifi_sym_create, LV_SCREEN_LOAD_ANIM_NONE, 0, 0);
        } else {
        LV_LOG_WARN("`key_symbols` slot of `rcc_wifi_pass_0` doesn't exist");
        }

        lv_obj_t * rcc_wifi_pass_0_key_go = rcc_wifi_pass_get_key_go(rcc_wifi_pass_0);
        if (rcc_wifi_pass_0_key_go) {
        lv_obj_add_screen_create_event(rcc_wifi_pass_0_key_go, LV_EVENT_CLICKED, hmi_wifi_busy_create, LV_SCREEN_LOAD_ANIM_NONE, 0, 0);
        } else {
        LV_LOG_WARN("`key_go` slot of `rcc_wifi_pass_0` doesn't exist");
        }

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

