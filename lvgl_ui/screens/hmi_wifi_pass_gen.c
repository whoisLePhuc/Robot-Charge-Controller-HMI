/**
 * @file hmi_wifi_pass_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "hmi_wifi_pass_gen.h"
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

lv_obj_t * hmi_wifi_pass_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
        lv_obj_set_name_static(lv_obj_0, "hmi_wifi_pass_#");
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

        lv_obj_t * rcc_wifi_pass_0 = rcc_wifi_pass_create(lv_obj_0, false, false, "6 / 63", "?123", hmi_f10, hmi_f12b, hmi_icon_eye_14, hmi_icon_shift_14, hmi_icon_bksp_16);
        lv_obj_t * rcc_wifi_pass_0_password_field = rcc_wifi_pass_get_password_field(rcc_wifi_pass_0);
        if (rcc_wifi_pass_0_password_field) {
        lv_obj_t * lv_obj_1 = lv_obj_create(rcc_wifi_pass_0_password_field);
        lv_obj_set_x(lv_obj_1, 12);
        lv_obj_set_y(lv_obj_1, 14);
        lv_obj_set_width(lv_obj_1, 6);
        lv_obj_set_height(lv_obj_1, 6);
        lv_obj_set_clickable(lv_obj_1, false);
        lv_obj_set_style_radius(lv_obj_1, 3, 0);
        lv_obj_set_style_border_width(lv_obj_1, 0, 0);
        lv_obj_set_style_bg_opa(lv_obj_1, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(lv_obj_1, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_style_pad_top(lv_obj_1, 0, 0);
        lv_obj_set_style_pad_bottom(lv_obj_1, 0, 0);
        lv_obj_set_style_pad_left(lv_obj_1, 0, 0);
        lv_obj_set_style_pad_right(lv_obj_1, 0, 0);
        lv_obj_set_scrollable(lv_obj_1, false);
        lv_obj_set_style_shadow_width(lv_obj_1, 0, 0);
        lv_obj_set_style_outline_width(lv_obj_1, 0, 0);

        lv_obj_t * lv_obj_2 = lv_obj_create(rcc_wifi_pass_0_password_field);
        lv_obj_set_x(lv_obj_2, 23);
        lv_obj_set_y(lv_obj_2, 14);
        lv_obj_set_width(lv_obj_2, 6);
        lv_obj_set_height(lv_obj_2, 6);
        lv_obj_set_clickable(lv_obj_2, false);
        lv_obj_set_style_radius(lv_obj_2, 3, 0);
        lv_obj_set_style_border_width(lv_obj_2, 0, 0);
        lv_obj_set_style_bg_opa(lv_obj_2, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(lv_obj_2, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_style_pad_top(lv_obj_2, 0, 0);
        lv_obj_set_style_pad_bottom(lv_obj_2, 0, 0);
        lv_obj_set_style_pad_left(lv_obj_2, 0, 0);
        lv_obj_set_style_pad_right(lv_obj_2, 0, 0);
        lv_obj_set_scrollable(lv_obj_2, false);
        lv_obj_set_style_shadow_width(lv_obj_2, 0, 0);
        lv_obj_set_style_outline_width(lv_obj_2, 0, 0);

        lv_obj_t * lv_obj_3 = lv_obj_create(rcc_wifi_pass_0_password_field);
        lv_obj_set_x(lv_obj_3, 34);
        lv_obj_set_y(lv_obj_3, 14);
        lv_obj_set_width(lv_obj_3, 6);
        lv_obj_set_height(lv_obj_3, 6);
        lv_obj_set_clickable(lv_obj_3, false);
        lv_obj_set_style_radius(lv_obj_3, 3, 0);
        lv_obj_set_style_border_width(lv_obj_3, 0, 0);
        lv_obj_set_style_bg_opa(lv_obj_3, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(lv_obj_3, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_style_pad_top(lv_obj_3, 0, 0);
        lv_obj_set_style_pad_bottom(lv_obj_3, 0, 0);
        lv_obj_set_style_pad_left(lv_obj_3, 0, 0);
        lv_obj_set_style_pad_right(lv_obj_3, 0, 0);
        lv_obj_set_scrollable(lv_obj_3, false);
        lv_obj_set_style_shadow_width(lv_obj_3, 0, 0);
        lv_obj_set_style_outline_width(lv_obj_3, 0, 0);

        lv_obj_t * lv_obj_4 = lv_obj_create(rcc_wifi_pass_0_password_field);
        lv_obj_set_x(lv_obj_4, 45);
        lv_obj_set_y(lv_obj_4, 14);
        lv_obj_set_width(lv_obj_4, 6);
        lv_obj_set_height(lv_obj_4, 6);
        lv_obj_set_clickable(lv_obj_4, false);
        lv_obj_set_style_radius(lv_obj_4, 3, 0);
        lv_obj_set_style_border_width(lv_obj_4, 0, 0);
        lv_obj_set_style_bg_opa(lv_obj_4, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(lv_obj_4, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_style_pad_top(lv_obj_4, 0, 0);
        lv_obj_set_style_pad_bottom(lv_obj_4, 0, 0);
        lv_obj_set_style_pad_left(lv_obj_4, 0, 0);
        lv_obj_set_style_pad_right(lv_obj_4, 0, 0);
        lv_obj_set_scrollable(lv_obj_4, false);
        lv_obj_set_style_shadow_width(lv_obj_4, 0, 0);
        lv_obj_set_style_outline_width(lv_obj_4, 0, 0);

        lv_obj_t * lv_obj_5 = lv_obj_create(rcc_wifi_pass_0_password_field);
        lv_obj_set_x(lv_obj_5, 56);
        lv_obj_set_y(lv_obj_5, 14);
        lv_obj_set_width(lv_obj_5, 6);
        lv_obj_set_height(lv_obj_5, 6);
        lv_obj_set_clickable(lv_obj_5, false);
        lv_obj_set_style_radius(lv_obj_5, 3, 0);
        lv_obj_set_style_border_width(lv_obj_5, 0, 0);
        lv_obj_set_style_bg_opa(lv_obj_5, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(lv_obj_5, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_style_pad_top(lv_obj_5, 0, 0);
        lv_obj_set_style_pad_bottom(lv_obj_5, 0, 0);
        lv_obj_set_style_pad_left(lv_obj_5, 0, 0);
        lv_obj_set_style_pad_right(lv_obj_5, 0, 0);
        lv_obj_set_scrollable(lv_obj_5, false);
        lv_obj_set_style_shadow_width(lv_obj_5, 0, 0);
        lv_obj_set_style_outline_width(lv_obj_5, 0, 0);

        lv_obj_t * lv_obj_6 = lv_obj_create(rcc_wifi_pass_0_password_field);
        lv_obj_set_x(lv_obj_6, 67);
        lv_obj_set_y(lv_obj_6, 14);
        lv_obj_set_width(lv_obj_6, 6);
        lv_obj_set_height(lv_obj_6, 6);
        lv_obj_set_clickable(lv_obj_6, false);
        lv_obj_set_style_radius(lv_obj_6, 3, 0);
        lv_obj_set_style_border_width(lv_obj_6, 0, 0);
        lv_obj_set_style_bg_opa(lv_obj_6, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(lv_obj_6, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_style_pad_top(lv_obj_6, 0, 0);
        lv_obj_set_style_pad_bottom(lv_obj_6, 0, 0);
        lv_obj_set_style_pad_left(lv_obj_6, 0, 0);
        lv_obj_set_style_pad_right(lv_obj_6, 0, 0);
        lv_obj_set_scrollable(lv_obj_6, false);
        lv_obj_set_style_shadow_width(lv_obj_6, 0, 0);
        lv_obj_set_style_outline_width(lv_obj_6, 0, 0);

        lv_obj_t * lv_obj_7 = lv_obj_create(rcc_wifi_pass_0_password_field);
        lv_obj_set_x(lv_obj_7, 78);
        lv_obj_set_y(lv_obj_7, 9);
        lv_obj_set_width(lv_obj_7, 2);
        lv_obj_set_height(lv_obj_7, 16);
        lv_obj_set_clickable(lv_obj_7, false);
        lv_obj_set_style_radius(lv_obj_7, 1, 0);
        lv_obj_set_style_border_width(lv_obj_7, 0, 0);
        lv_obj_set_style_bg_opa(lv_obj_7, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(lv_obj_7, lv_color_hex(0x38bdf8), 0);
        lv_obj_set_style_pad_top(lv_obj_7, 0, 0);
        lv_obj_set_style_pad_bottom(lv_obj_7, 0, 0);
        lv_obj_set_style_pad_left(lv_obj_7, 0, 0);
        lv_obj_set_style_pad_right(lv_obj_7, 0, 0);
        lv_obj_set_scrollable(lv_obj_7, false);
        lv_obj_set_style_shadow_width(lv_obj_7, 0, 0);
        lv_obj_set_style_outline_width(lv_obj_7, 0, 0);
        } else {
        LV_LOG_WARN("`password_field` slot of `rcc_wifi_pass_0` doesn't exist");
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

