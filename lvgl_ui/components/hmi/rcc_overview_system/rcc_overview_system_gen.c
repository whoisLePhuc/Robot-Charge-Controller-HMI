/**
 * @file rcc_overview_system_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "rcc_overview_system_gen.h"
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

lv_obj_t * rcc_overview_system_create(lv_obj_t * parent, int32_t card_y, int32_t card_height, lv_font_t * small, lv_font_t * bold, const void * uart_icon, const void * fault_icon, const void * relay_icon, const void * session_icon)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "rcc_overview_system_#");
        lv_obj_set_scrollable(lv_obj_0, false);
        lv_obj_set_clickable(lv_obj_0, false);
        lv_obj_set_x(lv_obj_0, 0);
        lv_obj_set_y(lv_obj_0, card_y);
        lv_obj_set_width(lv_obj_0, 156);
        lv_obj_set_height(lv_obj_0, card_height);
        lv_obj_set_style_bg_color(lv_obj_0, lv_color_hex(0x111722), 0);
        lv_obj_set_style_bg_opa(lv_obj_0, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(lv_obj_0, 10, 0);
        lv_obj_set_style_border_width(lv_obj_0, 1, 0);
        lv_obj_set_style_border_color(lv_obj_0, lv_color_hex(0x1e293b), 0);
        lv_obj_set_style_border_opa(lv_obj_0, (255 * 100 / 100), 0);
        lv_obj_set_style_outline_width(lv_obj_0, 0, 0);
        lv_obj_set_style_shadow_width(lv_obj_0, 0, 0);
        lv_obj_set_style_pad_all(lv_obj_0, -1, 0);

        lv_obj_t * uart_hit = lv_obj_create(lv_obj_0);
        lv_obj_set_name(uart_hit, "uart_hit");
        lv_obj_set_x(uart_hit, 2);
        lv_obj_set_y(uart_hit, 4 + 0 * (22 + (card_height - 102) / 3));
        lv_obj_set_width(uart_hit, 152);
        lv_obj_set_height(uart_hit, 28);
        lv_obj_set_scrollable(uart_hit, false);
        lv_obj_set_clickable(uart_hit, true);
        lv_obj_set_style_radius(uart_hit, 6, 0);
        lv_obj_set_style_bg_color(uart_hit, lv_color_hex(0x172030), LV_STATE_PRESSED);
        lv_obj_set_style_bg_opa(uart_hit, (255 * 100 / 100), LV_STATE_PRESSED);
        lv_obj_set_style_bg_opa(uart_hit, (255 * 0 / 100), 0);
        lv_obj_set_style_border_width(uart_hit, 0, 0);
        lv_obj_set_style_outline_width(uart_hit, 0, 0);
        lv_obj_set_style_shadow_width(uart_hit, 0, 0);
        lv_obj_set_style_pad_all(uart_hit, 0, 0);

        lv_obj_t * uart_icon_view = lv_image_create(lv_obj_0);
        lv_obj_set_x(uart_icon_view, 11);
        lv_obj_set_y(uart_icon_view, 11 + 0 * (22 + (card_height - 102) / 3));
        lv_image_set_src(uart_icon_view, uart_icon);
        lv_obj_set_clickable(uart_icon_view, false);
        lv_obj_set_scrollable(uart_icon_view, false);
        lv_obj_set_style_image_recolor(uart_icon_view, lv_color_hex(0x22c55e), 0);
        lv_obj_set_style_image_recolor_opa(uart_icon_view, (255 * 100 / 100), 0);
        lv_obj_set_name(uart_icon_view, "uart_icon_view");

        lv_obj_t * uart_key = lv_label_create(lv_obj_0);
        lv_obj_set_x(uart_key, 29);
        lv_obj_set_y(uart_key, 11 + 0 * (22 + (card_height - 102) / 3));
        lv_label_set_text(uart_key, "UART");
        lv_obj_set_scrollable(uart_key, false);
        lv_obj_set_clickable(uart_key, false);
        lv_obj_set_style_text_font(uart_key, small, 0);
        lv_obj_set_style_text_color(uart_key, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_name(uart_key, "uart_key");

        lv_obj_t * uart_value = lv_label_create(lv_obj_0);
        lv_obj_set_x(uart_value, 64);
        lv_obj_set_y(uart_value, 11 + 0 * (22 + (card_height - 102) / 3));
        lv_label_set_text(uart_value, "0.4 s");
        lv_obj_set_scrollable(uart_value, false);
        lv_obj_set_clickable(uart_value, false);
        lv_obj_set_style_text_font(uart_value, bold, 0);
        lv_obj_set_style_text_color(uart_value, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_width(uart_value, 81);
        lv_obj_set_height(uart_value, 13);
        lv_label_set_long_mode(uart_value, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(uart_value, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_set_name(uart_value, "uart_value");

        lv_obj_t * fault_hit = lv_obj_create(lv_obj_0);
        lv_obj_set_name(fault_hit, "fault_hit");
        lv_obj_set_x(fault_hit, 2);
        lv_obj_set_y(fault_hit, 4 + 1 * (22 + (card_height - 102) / 3));
        lv_obj_set_width(fault_hit, 152);
        lv_obj_set_height(fault_hit, 28);
        lv_obj_set_scrollable(fault_hit, false);
        lv_obj_set_clickable(fault_hit, true);
        lv_obj_set_style_radius(fault_hit, 6, 0);
        lv_obj_set_style_bg_color(fault_hit, lv_color_hex(0x172030), LV_STATE_PRESSED);
        lv_obj_set_style_bg_opa(fault_hit, (255 * 100 / 100), LV_STATE_PRESSED);
        lv_obj_set_style_bg_opa(fault_hit, (255 * 0 / 100), 0);
        lv_obj_set_style_border_width(fault_hit, 0, 0);
        lv_obj_set_style_outline_width(fault_hit, 0, 0);
        lv_obj_set_style_shadow_width(fault_hit, 0, 0);
        lv_obj_set_style_pad_all(fault_hit, 0, 0);

        lv_obj_t * fault_icon_view = lv_image_create(lv_obj_0);
        lv_obj_set_x(fault_icon_view, 11);
        lv_obj_set_y(fault_icon_view, 12 + 1 * (22 + (card_height - 102) / 3));
        lv_image_set_src(fault_icon_view, fault_icon);
        lv_obj_set_clickable(fault_icon_view, false);
        lv_obj_set_scrollable(fault_icon_view, false);
        lv_obj_set_style_image_recolor(fault_icon_view, lv_color_hex(0x22c55e), 0);
        lv_obj_set_style_image_recolor_opa(fault_icon_view, (255 * 100 / 100), 0);
        lv_obj_set_name(fault_icon_view, "fault_icon_view");

        lv_obj_t * fault_key = lv_label_create(lv_obj_0);
        lv_obj_set_x(fault_key, 29);
        lv_obj_set_y(fault_key, 11 + 1 * (22 + (card_height - 102) / 3));
        lv_label_set_text(fault_key, "Fault");
        lv_obj_set_scrollable(fault_key, false);
        lv_obj_set_clickable(fault_key, false);
        lv_obj_set_style_text_font(fault_key, small, 0);
        lv_obj_set_style_text_color(fault_key, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_name(fault_key, "fault_key");

        lv_obj_t * fault_value = lv_label_create(lv_obj_0);
        lv_obj_set_x(fault_value, 61);
        lv_obj_set_y(fault_value, 11 + 1 * (22 + (card_height - 102) / 3));
        lv_label_set_text(fault_value, "No faults");
        lv_obj_set_scrollable(fault_value, false);
        lv_obj_set_clickable(fault_value, false);
        lv_obj_set_style_text_font(fault_value, bold, 0);
        lv_obj_set_style_text_color(fault_value, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_width(fault_value, 84);
        lv_obj_set_height(fault_value, 13);
        lv_label_set_long_mode(fault_value, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(fault_value, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_set_name(fault_value, "fault_value");

        lv_obj_t * relay_icon_view = lv_image_create(lv_obj_0);
        lv_obj_set_x(relay_icon_view, 11);
        lv_obj_set_y(relay_icon_view, 11 + 2 * (22 + (card_height - 102) / 3));
        lv_image_set_src(relay_icon_view, relay_icon);
        lv_obj_set_clickable(relay_icon_view, false);
        lv_obj_set_scrollable(relay_icon_view, false);
        lv_obj_set_style_image_recolor(relay_icon_view, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_style_image_recolor_opa(relay_icon_view, (255 * 100 / 100), 0);
        lv_obj_set_name(relay_icon_view, "relay_icon_view");

        lv_obj_t * relay_key = lv_label_create(lv_obj_0);
        lv_obj_set_x(relay_key, 29);
        lv_obj_set_y(relay_key, 11 + 2 * (22 + (card_height - 102) / 3));
        lv_label_set_text(relay_key, "Relay cmd");
        lv_obj_set_scrollable(relay_key, false);
        lv_obj_set_clickable(relay_key, false);
        lv_obj_set_style_text_font(relay_key, small, 0);
        lv_obj_set_style_text_color(relay_key, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_name(relay_key, "relay_key");

        lv_obj_t * relay_value = lv_label_create(lv_obj_0);
        lv_obj_set_x(relay_value, 90);
        lv_obj_set_y(relay_value, 11 + 2 * (22 + (card_height - 102) / 3));
        lv_label_set_text(relay_value, "CLOSED");
        lv_obj_set_scrollable(relay_value, false);
        lv_obj_set_clickable(relay_value, false);
        lv_obj_set_style_text_font(relay_value, bold, 0);
        lv_obj_set_style_text_color(relay_value, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_width(relay_value, 55);
        lv_obj_set_height(relay_value, 13);
        lv_label_set_long_mode(relay_value, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(relay_value, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_set_name(relay_value, "relay_value");

        lv_obj_t * session_hit = lv_obj_create(lv_obj_0);
        lv_obj_set_name(session_hit, "session_hit");
        lv_obj_set_x(session_hit, 2);
        lv_obj_set_y(session_hit, 4 + 3 * (22 + (card_height - 102) / 3));
        lv_obj_set_width(session_hit, 152);
        lv_obj_set_height(session_hit, 28);
        lv_obj_set_scrollable(session_hit, false);
        lv_obj_set_clickable(session_hit, true);
        lv_obj_set_style_radius(session_hit, 6, 0);
        lv_obj_set_style_bg_color(session_hit, lv_color_hex(0x172030), LV_STATE_PRESSED);
        lv_obj_set_style_bg_opa(session_hit, (255 * 100 / 100), LV_STATE_PRESSED);
        lv_obj_set_style_bg_opa(session_hit, (255 * 0 / 100), 0);
        lv_obj_set_style_border_width(session_hit, 0, 0);
        lv_obj_set_style_outline_width(session_hit, 0, 0);
        lv_obj_set_style_shadow_width(session_hit, 0, 0);
        lv_obj_set_style_pad_all(session_hit, 0, 0);

        lv_obj_t * session_icon_view = lv_image_create(lv_obj_0);
        lv_obj_set_x(session_icon_view, 11);
        lv_obj_set_y(session_icon_view, 11 + 3 * (22 + (card_height - 102) / 3));
        lv_image_set_src(session_icon_view, session_icon);
        lv_obj_set_clickable(session_icon_view, false);
        lv_obj_set_scrollable(session_icon_view, false);
        lv_obj_set_style_image_recolor(session_icon_view, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_style_image_recolor_opa(session_icon_view, (255 * 100 / 100), 0);
        lv_obj_set_name(session_icon_view, "session_icon_view");

        lv_obj_t * session_key = lv_label_create(lv_obj_0);
        lv_obj_set_x(session_key, 29);
        lv_obj_set_y(session_key, 11 + 3 * (22 + (card_height - 102) / 3));
        lv_label_set_text(session_key, "Sess");
        lv_obj_set_scrollable(session_key, false);
        lv_obj_set_clickable(session_key, false);
        lv_obj_set_style_text_font(session_key, small, 0);
        lv_obj_set_style_text_color(session_key, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_name(session_key, "session_key");

        lv_obj_t * session_value = lv_label_create(lv_obj_0);
        lv_obj_set_x(session_value, 57);
        lv_obj_set_y(session_value, 11 + 3 * (22 + (card_height - 102) / 3));
        lv_label_set_text(session_value, "17");
        lv_obj_set_scrollable(session_value, false);
        lv_obj_set_clickable(session_value, false);
        lv_obj_set_style_text_font(session_value, bold, 0);
        lv_obj_set_style_text_color(session_value, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_width(session_value, 88);
        lv_obj_set_height(session_value, 13);
        lv_label_set_long_mode(session_value, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(session_value, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_set_name(session_value, "session_value");

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

lv_obj_t * rcc_overview_system_get_uart_hit(lv_obj_t *parent)
{
    return lv_obj_find_by_name(parent, "uart_hit");
}

lv_obj_t * rcc_overview_system_get_fault_hit(lv_obj_t *parent)
{
    return lv_obj_find_by_name(parent, "fault_hit");
}

lv_obj_t * rcc_overview_system_get_session_hit(lv_obj_t *parent)
{
    return lv_obj_find_by_name(parent, "session_hit");
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

