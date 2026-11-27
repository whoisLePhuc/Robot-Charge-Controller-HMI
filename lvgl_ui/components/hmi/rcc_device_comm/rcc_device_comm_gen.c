/**
 * @file rcc_device_comm_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "rcc_device_comm_gen.h"
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

lv_obj_t * rcc_device_comm_create(lv_obj_t * parent, int32_t card_height, lv_font_t * caption_font, lv_font_t * body_font, lv_font_t * value_font, lv_font_t * title_font)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "rcc_device_comm_#");
        lv_obj_set_x(lv_obj_0, 0);
        lv_obj_set_y(lv_obj_0, 68);
        lv_obj_set_width(lv_obj_0, 408);
        lv_obj_set_height(lv_obj_0, card_height);
        lv_obj_set_clickable(lv_obj_0, false);
        lv_obj_set_scrollable(lv_obj_0, false);
        lv_obj_set_style_bg_color(lv_obj_0, lv_color_hex(0x000000), 0);
        lv_obj_set_style_bg_opa(lv_obj_0, (255 * 0 / 100), 0);
        lv_obj_set_style_radius(lv_obj_0, 0, 0);
        lv_obj_set_style_border_width(lv_obj_0, 0, 0);
        lv_obj_set_style_outline_width(lv_obj_0, 0, 0);
        lv_obj_set_style_shadow_width(lv_obj_0, 0, 0);
        lv_obj_set_style_pad_all(lv_obj_0, 0, 0);

        lv_obj_t * lv_obj_1 = lv_obj_create(lv_obj_0);
        lv_obj_set_x(lv_obj_1, 0);
        lv_obj_set_y(lv_obj_1, 0);
        lv_obj_set_width(lv_obj_1, 233);
        lv_obj_set_height(lv_obj_1, card_height);
        lv_obj_set_clickable(lv_obj_1, false);
        lv_obj_set_scrollable(lv_obj_1, false);
        lv_obj_set_style_bg_color(lv_obj_1, lv_color_hex(0x111722), 0);
        lv_obj_set_style_bg_opa(lv_obj_1, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(lv_obj_1, 10, 0);
        lv_obj_set_style_border_width(lv_obj_1, 1, 0);
        lv_obj_set_style_outline_width(lv_obj_1, 0, 0);
        lv_obj_set_style_shadow_width(lv_obj_1, 0, 0);
        lv_obj_set_style_pad_all(lv_obj_1, -1, 0);
        lv_obj_set_style_border_color(lv_obj_1, lv_color_hex(0x1e293b), 0);
        lv_obj_set_style_border_opa(lv_obj_1, (255 * 100 / 100), 0);
        lv_obj_t * device_comm_dot = lv_obj_create(lv_obj_1);
        lv_obj_set_x(device_comm_dot, 13);
        lv_obj_set_y(device_comm_dot, 14);
        lv_obj_set_width(device_comm_dot, 7);
        lv_obj_set_height(device_comm_dot, 7);
        lv_obj_set_clickable(device_comm_dot, false);
        lv_obj_set_scrollable(device_comm_dot, false);
        lv_obj_set_style_bg_color(device_comm_dot, lv_color_hex(0x22c55e), 0);
        lv_obj_set_style_bg_opa(device_comm_dot, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(device_comm_dot, 4, 0);
        lv_obj_set_style_border_width(device_comm_dot, 0, 0);
        lv_obj_set_style_outline_width(device_comm_dot, 0, 0);
        lv_obj_set_style_shadow_width(device_comm_dot, 0, 0);
        lv_obj_set_style_pad_all(device_comm_dot, 0, 0);
        lv_obj_set_name(device_comm_dot, "device_comm_dot");

        lv_obj_t * device_comm_state = lv_label_create(lv_obj_1);
        lv_obj_set_x(device_comm_state, 26);
        lv_obj_set_y(device_comm_state, 9);
        lv_label_set_text(device_comm_state, "ONLINE");
        lv_obj_set_style_text_font(device_comm_state, title_font, 0);
        lv_obj_set_style_text_color(device_comm_state, lv_color_hex(0x22c55e), 0);
        lv_obj_set_clickable(device_comm_state, false);
        lv_obj_set_scrollable(device_comm_state, false);
        lv_obj_set_width(device_comm_state, 80);
        lv_obj_set_height(device_comm_state, 16);
        lv_label_set_long_mode(device_comm_state, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(device_comm_state, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_name(device_comm_state, "device_comm_state");

        lv_obj_t * device_comm_reply = lv_label_create(lv_obj_1);
        lv_obj_set_x(device_comm_reply, 110);
        lv_obj_set_y(device_comm_reply, 10);
        lv_label_set_text(device_comm_reply, "Reply 0.4 s ago");
        lv_obj_set_style_text_font(device_comm_reply, caption_font, 0);
        lv_obj_set_style_text_color(device_comm_reply, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_clickable(device_comm_reply, false);
        lv_obj_set_scrollable(device_comm_reply, false);
        lv_obj_set_width(device_comm_reply, 110);
        lv_obj_set_height(device_comm_reply, 13);
        lv_label_set_long_mode(device_comm_reply, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(device_comm_reply, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_set_name(device_comm_reply, "device_comm_reply");

        lv_obj_t * lv_label_0 = lv_label_create(lv_obj_1);
        lv_obj_set_x(lv_label_0, 13);
        lv_obj_set_y(lv_label_0, card_height - 61 + 1);
        lv_label_set_text(lv_label_0, "Timeouts");
        lv_obj_set_style_text_font(lv_label_0, body_font, 0);
        lv_obj_set_style_text_color(lv_label_0, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_clickable(lv_label_0, false);
        lv_obj_set_scrollable(lv_label_0, false);
        lv_obj_set_width(lv_label_0, 98);
        lv_obj_set_height(lv_label_0, 15);
        lv_label_set_long_mode(lv_label_0, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * device_timeouts = lv_label_create(lv_obj_1);
        lv_obj_set_x(device_timeouts, 16);
        lv_obj_set_y(device_timeouts, card_height - 61 + 2);
        lv_label_set_text(device_timeouts, "0");
        lv_obj_set_style_text_font(device_timeouts, value_font, 0);
        lv_obj_set_style_text_color(device_timeouts, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_clickable(device_timeouts, false);
        lv_obj_set_scrollable(device_timeouts, false);
        lv_obj_set_width(device_timeouts, 96);
        lv_obj_set_height(device_timeouts, 13);
        lv_label_set_long_mode(device_timeouts, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(device_timeouts, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_set_name(device_timeouts, "device_timeouts");

        lv_obj_t * lv_label_1 = lv_label_create(lv_obj_1);
        lv_obj_set_x(lv_label_1, 13);
        lv_obj_set_y(lv_label_1, card_height - 43 + 1);
        lv_label_set_text(lv_label_1, "Bad frames");
        lv_obj_set_style_text_font(lv_label_1, body_font, 0);
        lv_obj_set_style_text_color(lv_label_1, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_clickable(lv_label_1, false);
        lv_obj_set_scrollable(lv_label_1, false);
        lv_obj_set_width(lv_label_1, 98);
        lv_obj_set_height(lv_label_1, 15);
        lv_label_set_long_mode(lv_label_1, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_1, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * device_bad_frames = lv_label_create(lv_obj_1);
        lv_obj_set_x(device_bad_frames, 16);
        lv_obj_set_y(device_bad_frames, card_height - 43 + 2);
        lv_label_set_text(device_bad_frames, "0");
        lv_obj_set_style_text_font(device_bad_frames, value_font, 0);
        lv_obj_set_style_text_color(device_bad_frames, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_clickable(device_bad_frames, false);
        lv_obj_set_scrollable(device_bad_frames, false);
        lv_obj_set_width(device_bad_frames, 96);
        lv_obj_set_height(device_bad_frames, 13);
        lv_label_set_long_mode(device_bad_frames, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(device_bad_frames, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_set_name(device_bad_frames, "device_bad_frames");

        lv_obj_t * lv_label_2 = lv_label_create(lv_obj_1);
        lv_obj_set_x(lv_label_2, 13);
        lv_obj_set_y(lv_label_2, card_height - 25 + 1);
        lv_label_set_text(lv_label_2, "Missed ev.");
        lv_obj_set_style_text_font(lv_label_2, body_font, 0);
        lv_obj_set_style_text_color(lv_label_2, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_clickable(lv_label_2, false);
        lv_obj_set_scrollable(lv_label_2, false);
        lv_obj_set_width(lv_label_2, 98);
        lv_obj_set_height(lv_label_2, 15);
        lv_label_set_long_mode(lv_label_2, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_2, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * device_missed = lv_label_create(lv_obj_1);
        lv_obj_set_x(device_missed, 16);
        lv_obj_set_y(device_missed, card_height - 25 + 2);
        lv_label_set_text(device_missed, "3");
        lv_obj_set_style_text_font(device_missed, value_font, 0);
        lv_obj_set_style_text_color(device_missed, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_clickable(device_missed, false);
        lv_obj_set_scrollable(device_missed, false);
        lv_obj_set_width(device_missed, 96);
        lv_obj_set_height(device_missed, 13);
        lv_label_set_long_mode(device_missed, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(device_missed, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_set_name(device_missed, "device_missed");

        lv_obj_t * lv_label_3 = lv_label_create(lv_obj_1);
        lv_obj_set_x(lv_label_3, 122);
        lv_obj_set_y(lv_label_3, card_height - 61 + 1);
        lv_label_set_text(lv_label_3, "Status age");
        lv_obj_set_style_text_font(lv_label_3, body_font, 0);
        lv_obj_set_style_text_color(lv_label_3, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_clickable(lv_label_3, false);
        lv_obj_set_scrollable(lv_label_3, false);
        lv_obj_set_width(lv_label_3, 98);
        lv_obj_set_height(lv_label_3, 15);
        lv_label_set_long_mode(lv_label_3, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_3, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * device_status_age = lv_label_create(lv_obj_1);
        lv_obj_set_x(device_status_age, 125);
        lv_obj_set_y(device_status_age, card_height - 61 + 2);
        lv_label_set_text(device_status_age, "0.4 s");
        lv_obj_set_style_text_font(device_status_age, value_font, 0);
        lv_obj_set_style_text_color(device_status_age, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_clickable(device_status_age, false);
        lv_obj_set_scrollable(device_status_age, false);
        lv_obj_set_width(device_status_age, 96);
        lv_obj_set_height(device_status_age, 13);
        lv_label_set_long_mode(device_status_age, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(device_status_age, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_set_name(device_status_age, "device_status_age");

        lv_obj_t * lv_label_4 = lv_label_create(lv_obj_1);
        lv_obj_set_x(lv_label_4, 122);
        lv_obj_set_y(lv_label_4, card_height - 43 + 1);
        lv_label_set_text(lv_label_4, "Meas. age");
        lv_obj_set_style_text_font(lv_label_4, body_font, 0);
        lv_obj_set_style_text_color(lv_label_4, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_clickable(lv_label_4, false);
        lv_obj_set_scrollable(lv_label_4, false);
        lv_obj_set_width(lv_label_4, 98);
        lv_obj_set_height(lv_label_4, 15);
        lv_label_set_long_mode(lv_label_4, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_4, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * device_measure_age = lv_label_create(lv_obj_1);
        lv_obj_set_x(device_measure_age, 125);
        lv_obj_set_y(device_measure_age, card_height - 43 + 2);
        lv_label_set_text(device_measure_age, "0.4 s");
        lv_obj_set_style_text_font(device_measure_age, value_font, 0);
        lv_obj_set_style_text_color(device_measure_age, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_clickable(device_measure_age, false);
        lv_obj_set_scrollable(device_measure_age, false);
        lv_obj_set_width(device_measure_age, 96);
        lv_obj_set_height(device_measure_age, 13);
        lv_label_set_long_mode(device_measure_age, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(device_measure_age, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_set_name(device_measure_age, "device_measure_age");

        lv_obj_t * lv_label_5 = lv_label_create(lv_obj_1);
        lv_obj_set_x(lv_label_5, 122);
        lv_obj_set_y(lv_label_5, card_height - 25 + 1);
        lv_label_set_text(lv_label_5, "Fault age");
        lv_obj_set_style_text_font(lv_label_5, body_font, 0);
        lv_obj_set_style_text_color(lv_label_5, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_clickable(lv_label_5, false);
        lv_obj_set_scrollable(lv_label_5, false);
        lv_obj_set_width(lv_label_5, 98);
        lv_obj_set_height(lv_label_5, 15);
        lv_label_set_long_mode(lv_label_5, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_5, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * device_fault_age = lv_label_create(lv_obj_1);
        lv_obj_set_x(device_fault_age, 125);
        lv_obj_set_y(device_fault_age, card_height - 25 + 2);
        lv_label_set_text(device_fault_age, "0.8 s");
        lv_obj_set_style_text_font(device_fault_age, value_font, 0);
        lv_obj_set_style_text_color(device_fault_age, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_clickable(device_fault_age, false);
        lv_obj_set_scrollable(device_fault_age, false);
        lv_obj_set_width(device_fault_age, 96);
        lv_obj_set_height(device_fault_age, 13);
        lv_label_set_long_mode(device_fault_age, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(device_fault_age, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_set_name(device_fault_age, "device_fault_age");

        lv_obj_t * lv_obj_2 = lv_obj_create(lv_obj_0);
        lv_obj_set_x(lv_obj_2, 241);
        lv_obj_set_y(lv_obj_2, 0);
        lv_obj_set_width(lv_obj_2, 167);
        lv_obj_set_height(lv_obj_2, card_height);
        lv_obj_set_clickable(lv_obj_2, false);
        lv_obj_set_scrollable(lv_obj_2, false);
        lv_obj_set_style_bg_color(lv_obj_2, lv_color_hex(0x111722), 0);
        lv_obj_set_style_bg_opa(lv_obj_2, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(lv_obj_2, 10, 0);
        lv_obj_set_style_border_width(lv_obj_2, 1, 0);
        lv_obj_set_style_outline_width(lv_obj_2, 0, 0);
        lv_obj_set_style_shadow_width(lv_obj_2, 0, 0);
        lv_obj_set_style_pad_all(lv_obj_2, -1, 0);
        lv_obj_set_style_border_color(lv_obj_2, lv_color_hex(0x1e293b), 0);
        lv_obj_set_style_border_opa(lv_obj_2, (255 * 100 / 100), 0);
        lv_obj_t * lv_label_6 = lv_label_create(lv_obj_2);
        lv_obj_set_x(lv_label_6, 13);
        lv_obj_set_y(lv_label_6, 8);
        lv_label_set_text(lv_label_6, "HMI PORT");
        lv_obj_set_style_text_font(lv_label_6, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_6, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_clickable(lv_label_6, false);
        lv_obj_set_scrollable(lv_label_6, false);
        lv_obj_set_style_text_letter_space(lv_label_6, 1, 0);

        lv_obj_t * lv_label_7 = lv_label_create(lv_obj_2);
        lv_obj_set_x(lv_label_7, 13);
        lv_obj_set_y(lv_label_7, 23 + 1 * ((card_height - 84) / 3) + 1);
        lv_label_set_text(lv_label_7, "Bus");
        lv_obj_set_style_text_font(lv_label_7, body_font, 0);
        lv_obj_set_style_text_color(lv_label_7, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_clickable(lv_label_7, false);
        lv_obj_set_scrollable(lv_label_7, false);
        lv_obj_set_width(lv_label_7, 141);
        lv_obj_set_height(lv_label_7, 15);
        lv_label_set_long_mode(lv_label_7, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_7, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * device_uart = lv_label_create(lv_obj_2);
        lv_obj_set_x(device_uart, 16);
        lv_obj_set_y(device_uart, 23 + 1 * ((card_height - 84) / 3) + 2);
        lv_label_set_text(device_uart, "UART1");
        lv_obj_set_style_text_font(device_uart, value_font, 0);
        lv_obj_set_style_text_color(device_uart, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_clickable(device_uart, false);
        lv_obj_set_scrollable(device_uart, false);
        lv_obj_set_width(device_uart, 139);
        lv_obj_set_height(device_uart, 13);
        lv_label_set_long_mode(device_uart, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(device_uart, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_set_name(device_uart, "device_uart");

        lv_obj_t * lv_label_8 = lv_label_create(lv_obj_2);
        lv_obj_set_x(lv_label_8, 13);
        lv_obj_set_y(lv_label_8, 41 + 2 * ((card_height - 84) / 3) + 1);
        lv_label_set_text(lv_label_8, "Baud");
        lv_obj_set_style_text_font(lv_label_8, body_font, 0);
        lv_obj_set_style_text_color(lv_label_8, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_clickable(lv_label_8, false);
        lv_obj_set_scrollable(lv_label_8, false);
        lv_obj_set_width(lv_label_8, 141);
        lv_obj_set_height(lv_label_8, 15);
        lv_label_set_long_mode(lv_label_8, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_8, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * device_baud = lv_label_create(lv_obj_2);
        lv_obj_set_x(device_baud, 16);
        lv_obj_set_y(device_baud, 41 + 2 * ((card_height - 84) / 3) + 2);
        lv_label_set_text(device_baud, "115200 8N1");
        lv_obj_set_style_text_font(device_baud, value_font, 0);
        lv_obj_set_style_text_color(device_baud, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_clickable(device_baud, false);
        lv_obj_set_scrollable(device_baud, false);
        lv_obj_set_width(device_baud, 139);
        lv_obj_set_height(device_baud, 13);
        lv_label_set_long_mode(device_baud, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(device_baud, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_set_name(device_baud, "device_baud");

        lv_obj_t * lv_label_9 = lv_label_create(lv_obj_2);
        lv_obj_set_x(lv_label_9, 13);
        lv_obj_set_y(lv_label_9, 59 + 3 * ((card_height - 84) / 3) + 1);
        lv_label_set_text(lv_label_9, "Pins");
        lv_obj_set_style_text_font(lv_label_9, body_font, 0);
        lv_obj_set_style_text_color(lv_label_9, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_clickable(lv_label_9, false);
        lv_obj_set_scrollable(lv_label_9, false);
        lv_obj_set_width(lv_label_9, 141);
        lv_obj_set_height(lv_label_9, 15);
        lv_label_set_long_mode(lv_label_9, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_9, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * device_pins = lv_label_create(lv_obj_2);
        lv_obj_set_x(device_pins, 16);
        lv_obj_set_y(device_pins, 59 + 3 * ((card_height - 84) / 3) + 2);
        lv_label_set_text(device_pins, "GPIO43 / 44");
        lv_obj_set_style_text_font(device_pins, value_font, 0);
        lv_obj_set_style_text_color(device_pins, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_clickable(device_pins, false);
        lv_obj_set_scrollable(device_pins, false);
        lv_obj_set_width(device_pins, 139);
        lv_obj_set_height(device_pins, 13);
        lv_label_set_long_mode(device_pins, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(device_pins, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_set_name(device_pins, "device_pins");

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

