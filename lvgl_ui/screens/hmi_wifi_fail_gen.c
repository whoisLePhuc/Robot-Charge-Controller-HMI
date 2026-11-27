/**
 * @file hmi_wifi_fail_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "hmi_wifi_fail_gen.h"
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

lv_obj_t * hmi_wifi_fail_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
        lv_obj_set_name_static(lv_obj_0, "hmi_wifi_fail_#");
        lv_obj_set_style_bg_color(lv_obj_0, lv_color_hex(0x090d14), 0);
        lv_obj_set_style_bg_opa(lv_obj_0, (255 * 100 / 100), 0);
        lv_obj_set_style_pad_all(lv_obj_0, 0, 0);
        lv_obj_set_style_border_width(lv_obj_0, 0, 0);
        lv_obj_set_scrollable(lv_obj_0, false);

        lv_obj_t * rcc_wifi_header_0 = rcc_wifi_header_create(lv_obj_0, "Connection failed", "MSVN-Office", hmi_f14b, hmi_f10, hmi_icon_back_14);
        lv_obj_t * rcc_wifi_header_0_back = rcc_wifi_header_get_back(rcc_wifi_header_0);
        if (rcc_wifi_header_0_back) {
        lv_obj_add_screen_create_event(rcc_wifi_header_0_back, LV_EVENT_CLICKED, hmi_wifi_list_create, LV_SCREEN_LOAD_ANIM_NONE, 0, 0);
        } else {
        LV_LOG_WARN("`back` slot of `rcc_wifi_header_0` doesn't exist");
        }

        lv_obj_t * rcc_wifi_fail_0 = rcc_wifi_fail_create(lv_obj_0, "Wrong password", "The access point rejected the credentials. Check the password and try again.", "1", "-61 dBm", true, hmi_f10, hmi_f12b, hmi_f14b, hmi_icon_warn_20);
        lv_obj_t * rcc_wifi_fail_0_choose_other = rcc_wifi_fail_get_choose_other(rcc_wifi_fail_0);
        if (rcc_wifi_fail_0_choose_other) {
        lv_obj_add_screen_create_event(rcc_wifi_fail_0_choose_other, LV_EVENT_CLICKED, hmi_wifi_list_create, LV_SCREEN_LOAD_ANIM_NONE, 0, 0);
        } else {
        LV_LOG_WARN("`choose_other` slot of `rcc_wifi_fail_0` doesn't exist");
        }

        lv_obj_t * rcc_wifi_fail_0_retry = rcc_wifi_fail_get_retry(rcc_wifi_fail_0);
        if (rcc_wifi_fail_0_retry) {
        lv_obj_add_screen_create_event(rcc_wifi_fail_0_retry, LV_EVENT_CLICKED, hmi_wifi_pass_create, LV_SCREEN_LOAD_ANIM_NONE, 0, 0);
        } else {
        LV_LOG_WARN("`retry` slot of `rcc_wifi_fail_0` doesn't exist");
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

