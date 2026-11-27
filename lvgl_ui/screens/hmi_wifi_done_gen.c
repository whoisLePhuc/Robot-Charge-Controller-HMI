/**
 * @file hmi_wifi_done_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "hmi_wifi_done_gen.h"
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

lv_obj_t * hmi_wifi_done_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
        lv_obj_set_name_static(lv_obj_0, "hmi_wifi_done_#");
        lv_obj_set_style_bg_color(lv_obj_0, lv_color_hex(0x090d14), 0);
        lv_obj_set_style_bg_opa(lv_obj_0, (255 * 100 / 100), 0);
        lv_obj_set_style_pad_all(lv_obj_0, 0, 0);
        lv_obj_set_style_border_width(lv_obj_0, 0, 0);
        lv_obj_set_scrollable(lv_obj_0, false);

        lv_obj_t * rcc_wifi_header_0 = rcc_wifi_header_create(lv_obj_0, "Wi-Fi", "Connected", hmi_f14b, hmi_f10, hmi_icon_back_14);
        lv_obj_t * rcc_wifi_header_0_back = rcc_wifi_header_get_back(rcc_wifi_header_0);
        if (rcc_wifi_header_0_back) {
        lv_obj_add_screen_create_event(rcc_wifi_header_0_back, LV_EVENT_CLICKED, hmi_wifi_list_create, LV_SCREEN_LOAD_ANIM_NONE, 0, 0);
        } else {
        LV_LOG_WARN("`back` slot of `rcc_wifi_header_0` doesn't exist");
        }

        lv_obj_t * rcc_wifi_info_0 = rcc_wifi_info_create(lv_obj_0, "MSVN-Office", "192.168.1.42", "192.168.1.1", "-61 dBm · Good", "A4:CF:12:9B:3E:70", "WEB OTA", "Locked", " (Target: )", "ENABLE OTA", false, hmi_f10, hmi_f12b, hmi_f14b, hmi_icon_wifi_22);
        lv_obj_t * rcc_wifi_info_0_done = rcc_wifi_info_get_done(rcc_wifi_info_0);
        if (rcc_wifi_info_0_done) {
        lv_obj_add_screen_create_event(rcc_wifi_info_0_done, LV_EVENT_CLICKED, hmi_device_settings_create, LV_SCREEN_LOAD_ANIM_NONE, 0, 0);
        } else {
        LV_LOG_WARN("`done` slot of `rcc_wifi_info_0` doesn't exist");
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

