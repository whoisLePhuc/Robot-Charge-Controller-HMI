/**
 * @file rcc_wifi_list_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "rcc_wifi_list_gen.h"
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

lv_obj_t * rcc_wifi_list_create(lv_obj_t * parent, lv_font_t * caption_font, lv_font_t * detail_font, const void * refresh_icon)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "rcc_wifi_list_#");
        lv_obj_set_width(lv_obj_0, 480);
        lv_obj_set_height(lv_obj_0, 320);
        lv_obj_set_clickable(lv_obj_0, false);
        lv_obj_set_scrollable(lv_obj_0, false);
        lv_obj_set_style_bg_opa(lv_obj_0, (255 * 0 / 100), 0);
        lv_obj_set_style_pad_all(lv_obj_0, 0, 0);
        lv_obj_set_style_border_width(lv_obj_0, 0, 0);
        lv_obj_set_style_outline_width(lv_obj_0, 0, 0);
        lv_obj_set_style_shadow_width(lv_obj_0, 0, 0);
        lv_obj_set_style_radius(lv_obj_0, 0, 0);

        lv_obj_t * scan = lv_button_create(lv_obj_0);
        lv_obj_set_x(scan, 400);
        lv_obj_set_y(scan, 10);
        lv_obj_set_width(scan, 72);
        lv_obj_set_height(scan, 26);
        lv_obj_set_clickable(scan, true);
        lv_obj_set_style_radius(scan, 8, 0);
        lv_obj_set_style_border_width(scan, 1, 0);
        lv_obj_set_style_bg_opa(scan, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(scan, lv_color_hex(0x0c2538), 0);
        lv_obj_set_style_border_color(scan, lv_color_hex(0x1e4963), 0);
        lv_obj_set_style_pad_top(scan, -1, 0);
        lv_obj_set_style_pad_bottom(scan, -1, 0);
        lv_obj_set_style_pad_left(scan, -1, 0);
        lv_obj_set_style_pad_right(scan, -1, 0);
        lv_obj_set_scrollable(scan, false);
        lv_obj_set_style_shadow_width(scan, 0, 0);
        lv_obj_set_style_outline_width(scan, 0, 0);
        lv_obj_set_name(scan, "scan");
        lv_obj_set_style_bg_opa(scan, (255 * 70 / 100), LV_STATE_PRESSED);
        lv_obj_t * lv_label_0 = lv_label_create(scan);
        lv_obj_set_x(lv_label_0, 28);
        lv_obj_set_y(lv_label_0, 6);
        lv_obj_set_width(lv_label_0, 32);
        lv_obj_set_height(lv_label_0, 13);
        lv_obj_set_clickable(lv_label_0, false);
        lv_obj_set_style_text_font(lv_label_0, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_0, lv_color_hex(0x38bdf8), 0);
        lv_obj_set_scrollable(lv_label_0, false);
        lv_label_set_text(lv_label_0, "SCAN");
        lv_label_set_long_mode(lv_label_0, LV_LABEL_LONG_MODE_WRAP);
        lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_letter_space(lv_label_0, 1, 0);

        lv_obj_t * lv_image_0 = lv_image_create(scan);
        lv_obj_set_x(lv_image_0, 11);
        lv_obj_set_y(lv_image_0, 7);
        lv_obj_set_width(lv_image_0, 11);
        lv_obj_set_height(lv_image_0, 11);
        lv_obj_set_clickable(lv_image_0, false);
        lv_obj_set_scrollable(lv_image_0, false);
        lv_image_set_src(lv_image_0, refresh_icon);
        lv_obj_set_style_image_recolor(lv_image_0, lv_color_hex(0x38bdf8), 0);
        lv_obj_set_style_image_recolor_opa(lv_image_0, (255 * 100 / 100), 0);

        lv_obj_t * status = lv_obj_create(lv_obj_0);
        lv_obj_set_x(status, 8);
        lv_obj_set_y(status, 46);
        lv_obj_set_width(status, 464);
        lv_obj_set_height(status, 26);
        lv_obj_set_clickable(status, false);
        lv_obj_set_style_radius(status, 8, 0);
        lv_obj_set_style_border_width(status, 1, 0);
        lv_obj_set_style_bg_opa(status, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(status, lv_color_hex(0x0a2617), 0);
        lv_obj_set_style_border_color(status, lv_color_hex(0x22c55e), 0);
        lv_obj_set_style_pad_top(status, -1, 0);
        lv_obj_set_style_pad_bottom(status, -1, 0);
        lv_obj_set_style_pad_left(status, -1, 0);
        lv_obj_set_style_pad_right(status, -1, 0);
        lv_obj_set_scrollable(status, false);
        lv_obj_set_style_shadow_width(status, 0, 0);
        lv_obj_set_style_outline_width(status, 0, 0);
        lv_obj_set_name(status, "status");
        lv_obj_t * status_dot = lv_obj_create(status);
        lv_obj_set_x(status_dot, 10);
        lv_obj_set_y(status_dot, 10);
        lv_obj_set_width(status_dot, 6);
        lv_obj_set_height(status_dot, 6);
        lv_obj_set_clickable(status_dot, false);
        lv_obj_set_style_radius(status_dot, 3, 0);
        lv_obj_set_style_border_width(status_dot, 0, 0);
        lv_obj_set_style_bg_opa(status_dot, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(status_dot, lv_color_hex(0x22c55e), 0);
        lv_obj_set_style_pad_top(status_dot, 0, 0);
        lv_obj_set_style_pad_bottom(status_dot, 0, 0);
        lv_obj_set_style_pad_left(status_dot, 0, 0);
        lv_obj_set_style_pad_right(status_dot, 0, 0);
        lv_obj_set_scrollable(status_dot, false);
        lv_obj_set_style_shadow_width(status_dot, 0, 0);
        lv_obj_set_style_outline_width(status_dot, 0, 0);
        lv_obj_set_name(status_dot, "status_dot");

        lv_obj_t * status_text = lv_label_create(status);
        lv_obj_set_x(status_text, 24);
        lv_obj_set_y(status_text, 6);
        lv_obj_set_width(status_text, 69);
        lv_obj_set_height(status_text, 13);
        lv_obj_set_clickable(status_text, false);
        lv_obj_set_style_text_font(status_text, caption_font, 0);
        lv_obj_set_style_text_color(status_text, lv_color_hex(0xcbd5e1), 0);
        lv_obj_set_scrollable(status_text, false);
        lv_label_set_text(status_text, "Connected to");
        lv_label_set_long_mode(status_text, LV_LABEL_LONG_MODE_WRAP);
        lv_obj_set_style_text_align(status_text, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_name(status_text, "status_text");

        lv_obj_t * status_ssid = lv_label_create(status);
        lv_obj_set_x(status_ssid, 98);
        lv_obj_set_y(status_ssid, 6);
        lv_obj_set_width(status_ssid, 200);
        lv_obj_set_height(status_ssid, 13);
        lv_obj_set_clickable(status_ssid, false);
        lv_obj_set_style_text_font(status_ssid, caption_font, 0);
        lv_obj_set_style_text_color(status_ssid, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_scrollable(status_ssid, false);
        lv_label_set_text(status_ssid, "MSVN-Lab-5G");
        lv_label_set_long_mode(status_ssid, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(status_ssid, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_name(status_ssid, "status_ssid");

        lv_obj_t * disconnect = lv_button_create(status);
        lv_obj_set_x(disconnect, 360);
        lv_obj_set_y(disconnect, 0);
        lv_obj_set_width(disconnect, 104);
        lv_obj_set_height(disconnect, 26);
        lv_obj_set_clickable(disconnect, true);
        lv_obj_set_style_radius(disconnect, 8, 0);
        lv_obj_set_style_border_width(disconnect, 0, 0);
        lv_obj_set_style_bg_opa(disconnect, (255 * 0 / 100), 0);
        lv_obj_set_style_pad_top(disconnect, 0, 0);
        lv_obj_set_style_pad_bottom(disconnect, 0, 0);
        lv_obj_set_style_pad_left(disconnect, 0, 0);
        lv_obj_set_style_pad_right(disconnect, 0, 0);
        lv_obj_set_scrollable(disconnect, false);
        lv_obj_set_style_shadow_width(disconnect, 0, 0);
        lv_obj_set_style_outline_width(disconnect, 0, 0);
        lv_obj_set_name(disconnect, "disconnect");
        lv_obj_t * lv_label_1 = lv_label_create(disconnect);
        lv_obj_set_x(lv_label_1, 0);
        lv_obj_set_y(lv_label_1, 6);
        lv_obj_set_width(lv_label_1, 94);
        lv_obj_set_height(lv_label_1, 13);
        lv_obj_set_clickable(lv_label_1, false);
        lv_obj_set_style_text_font(lv_label_1, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_1, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_scrollable(lv_label_1, false);
        lv_label_set_text(lv_label_1, "DISCONNECT");
        lv_label_set_long_mode(lv_label_1, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_1, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_set_style_text_letter_space(lv_label_1, 1, 0);

        lv_obj_t * networks = lv_obj_create(lv_obj_0);
        lv_obj_set_x(networks, 8);
        lv_obj_set_y(networks, 78);
        lv_obj_set_width(networks, 464);
        lv_obj_set_height(networks, 214);
        lv_obj_set_clickable(networks, true);
        lv_obj_set_style_radius(networks, 0, 0);
        lv_obj_set_style_border_width(networks, 0, 0);
        lv_obj_set_style_bg_opa(networks, (255 * 0 / 100), 0);
        lv_obj_set_style_pad_top(networks, 0, 0);
        lv_obj_set_style_pad_bottom(networks, 0, 0);
        lv_obj_set_style_pad_left(networks, 0, 0);
        lv_obj_set_style_pad_right(networks, 0, 0);
        lv_obj_set_scrollable(networks, true);
        lv_obj_set_scrollbar_mode(networks, LV_SCROLLBAR_MODE_AUTO);
        lv_obj_set_style_shadow_width(networks, 0, 0);
        lv_obj_set_style_outline_width(networks, 0, 0);
        lv_obj_set_name(networks, "networks");
        lv_obj_set_style_bg_color(networks, lv_color_hex(0x64748b), LV_PART_SCROLLBAR);
        lv_obj_set_style_bg_opa(networks, (255 * 100 / 100), LV_PART_SCROLLBAR);
        lv_obj_set_style_width(networks, 3, LV_PART_SCROLLBAR);
        lv_obj_set_style_radius(networks, 2, LV_PART_SCROLLBAR);
        lv_obj_set_style_pad_right(networks, 1, LV_PART_SCROLLBAR);

        lv_obj_t * list_tip = lv_label_create(lv_obj_0);
        lv_obj_set_x(list_tip, 0);
        lv_obj_set_y(list_tip, 299);
        lv_obj_set_width(list_tip, 480);
        lv_obj_set_height(list_tip, 13);
        lv_obj_set_clickable(list_tip, false);
        lv_obj_set_style_text_font(list_tip, caption_font, 0);
        lv_obj_set_style_text_color(list_tip, lv_color_hex(0x64748b), 0);
        lv_obj_set_scrollable(list_tip, false);
        lv_label_set_text(list_tip, "Drag to scroll · tap a network to connect");
        lv_label_set_long_mode(list_tip, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(list_tip, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_name(list_tip, "list_tip");

        lv_obj_t * empty_hint = lv_label_create(lv_obj_0);
        lv_obj_set_name(empty_hint, "empty_hint");
        lv_label_set_text(empty_hint, "No networks found - tap SCAN");
        lv_obj_set_clickable(empty_hint, false);
        lv_obj_set_scrollable(empty_hint, false);
        lv_obj_set_x(empty_hint, 0);
        lv_obj_set_y(empty_hint, 151);
        lv_obj_set_width(empty_hint, 480);
        lv_obj_set_height(empty_hint, LV_SIZE_CONTENT);
        lv_obj_set_style_text_font(empty_hint, detail_font, 0);
        lv_obj_set_style_text_color(empty_hint, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_style_text_align(empty_hint, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_hidden(empty_hint, true);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

lv_obj_t * rcc_wifi_list_get_networks(lv_obj_t *parent)
{
    return lv_obj_find_by_name(parent, "networks");
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

