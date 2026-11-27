/**
 * @file hmi_wifi_scroll_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "hmi_wifi_scroll_gen.h"
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

lv_obj_t * hmi_wifi_scroll_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
        lv_obj_set_name_static(lv_obj_0, "hmi_wifi_scroll_#");
        lv_obj_set_style_bg_color(lv_obj_0, lv_color_hex(0x090d14), 0);
        lv_obj_set_style_bg_opa(lv_obj_0, (255 * 100 / 100), 0);
        lv_obj_set_style_pad_all(lv_obj_0, 0, 0);
        lv_obj_set_style_border_width(lv_obj_0, 0, 0);
        lv_obj_set_scrollable(lv_obj_0, false);

        lv_obj_t * rcc_wifi_header_0 = rcc_wifi_header_create(lv_obj_0, "Wi-Fi", "6 networks found", hmi_f14b, hmi_f10, hmi_icon_back_14);
        lv_obj_t * rcc_wifi_header_0_back = rcc_wifi_header_get_back(rcc_wifi_header_0);
        if (rcc_wifi_header_0_back) {
        lv_obj_add_screen_create_event(rcc_wifi_header_0_back, LV_EVENT_CLICKED, hmi_device_settings_create, LV_SCREEN_LOAD_ANIM_NONE, 0, 0);
        } else {
        LV_LOG_WARN("`back` slot of `rcc_wifi_header_0` doesn't exist");
        }

        lv_obj_t * rcc_wifi_list_0 = rcc_wifi_list_create(lv_obj_0, hmi_f10, hmi_f12, hmi_icon_refresh_11);
        lv_obj_t * rcc_wifi_list_0_networks = rcc_wifi_list_get_networks(rcc_wifi_list_0);
        if (rcc_wifi_list_0_networks) {
        lv_obj_t * rcc_wifi_row_0 = rcc_wifi_row_create(rcc_wifi_list_0_networks, 0, "MSVN-Lab-5G", "Connected · 192.168.1.42", "-48 dBm", true, 330, lv_color_hex(0x0c2538), lv_color_hex(0x1e4963), lv_color_hex(0xcbd5e1), lv_color_hex(0x22c55e), lv_color_hex(0x22c55e), lv_color_hex(0x22c55e), lv_color_hex(0x22c55e), hmi_f10, hmi_f12b, hmi_icon_lock_11);
        lv_obj_set_y(rcc_wifi_row_0, 0);
        lv_obj_add_screen_create_event(rcc_wifi_row_0, LV_EVENT_CLICKED, hmi_wifi_info_create, LV_SCREEN_LOAD_ANIM_NONE, 0, 0);

        lv_obj_t * rcc_wifi_row_1 = rcc_wifi_row_create(rcc_wifi_list_0_networks, 1, "MSVN-Office", "Secured", "-61 dBm", true, 330, lv_color_hex(0x111722), lv_color_hex(0x1e293b), lv_color_hex(0x94a3b8), lv_color_hex(0x22c55e), lv_color_hex(0x22c55e), lv_color_hex(0x22c55e), lv_color_hex(0x2b3648), hmi_f10, hmi_f12b, hmi_icon_lock_11);
        lv_obj_set_y(rcc_wifi_row_1, 44);
        lv_obj_add_screen_create_event(rcc_wifi_row_1, LV_EVENT_CLICKED, hmi_wifi_pass_create, LV_SCREEN_LOAD_ANIM_NONE, 0, 0);

        lv_obj_t * rcc_wifi_row_2 = rcc_wifi_row_create(rcc_wifi_list_0_networks, 2, "Factory-IoT", "Secured", "-72 dBm", true, 330, lv_color_hex(0x111722), lv_color_hex(0x1e293b), lv_color_hex(0x94a3b8), lv_color_hex(0xfacc15), lv_color_hex(0xfacc15), lv_color_hex(0x2b3648), lv_color_hex(0x2b3648), hmi_f10, hmi_f12b, hmi_icon_lock_11);
        lv_obj_set_y(rcc_wifi_row_2, 88);

        lv_obj_t * rcc_wifi_row_3 = rcc_wifi_row_create(rcc_wifi_list_0_networks, 3, "Guest", "Open network", "-83 dBm", false, 345, lv_color_hex(0x111722), lv_color_hex(0x1e293b), lv_color_hex(0x94a3b8), lv_color_hex(0xfb923c), lv_color_hex(0x2b3648), lv_color_hex(0x2b3648), lv_color_hex(0x2b3648), hmi_f10, hmi_f12b, hmi_icon_lock_11);
        lv_obj_set_y(rcc_wifi_row_3, 132);
        lv_obj_add_screen_create_event(rcc_wifi_row_3, LV_EVENT_CLICKED, hmi_wifi_open_create, LV_SCREEN_LOAD_ANIM_NONE, 0, 0);

        lv_obj_t * rcc_wifi_row_4 = rcc_wifi_row_create(rcc_wifi_list_0_networks, 4, "RCC-Service", "Secured", "-86 dBm", true, 330, lv_color_hex(0x111722), lv_color_hex(0x1e293b), lv_color_hex(0x94a3b8), lv_color_hex(0xfb923c), lv_color_hex(0x2b3648), lv_color_hex(0x2b3648), lv_color_hex(0x2b3648), hmi_f10, hmi_f12b, hmi_icon_lock_11);
        lv_obj_set_y(rcc_wifi_row_4, 176);

        lv_obj_t * rcc_wifi_row_5 = rcc_wifi_row_create(rcc_wifi_list_0_networks, 5, "Warehouse-AP", "Secured", "-89 dBm", true, 330, lv_color_hex(0x111722), lv_color_hex(0x1e293b), lv_color_hex(0x94a3b8), lv_color_hex(0xfb923c), lv_color_hex(0x2b3648), lv_color_hex(0x2b3648), lv_color_hex(0x2b3648), hmi_f10, hmi_f12b, hmi_icon_lock_11);
        lv_obj_set_y(rcc_wifi_row_5, 220);
        } else {
        LV_LOG_WARN("`networks` slot of `rcc_wifi_list_0` doesn't exist");
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

