/**
 * @file rcc_shell_confirm_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "rcc_shell_confirm_gen.h"
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

lv_obj_t * rcc_shell_confirm_create(lv_obj_t * parent, const char * title, const char * controller_state, const char * link_status, const char * measurement_label, const char * measurement_status, const char * fault_status, lv_color_t fault_color, const char * footnote, lv_font_t * title_font, lv_font_t * row_font, lv_font_t * button_font, lv_font_t * body_font)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "rcc_shell_confirm_#");
        lv_obj_set_x(lv_obj_0, 56);
        lv_obj_set_y(lv_obj_0, 32);
        lv_obj_set_width(lv_obj_0, 424);
        lv_obj_set_height(lv_obj_0, 288);
        lv_obj_set_scrollable(lv_obj_0, false);
        lv_obj_set_clickable(lv_obj_0, true);
        lv_obj_set_style_pad_all(lv_obj_0, 0, 0);
        lv_obj_set_style_bg_color(lv_obj_0, lv_color_hex(0x06090e), 0);
        lv_obj_set_style_bg_opa(lv_obj_0, 209, 0);
        lv_obj_set_style_radius(lv_obj_0, 0, 0);
        lv_obj_set_style_border_width(lv_obj_0, 0, 0);
        lv_obj_set_style_outline_width(lv_obj_0, 0, 0);
        lv_obj_set_style_shadow_width(lv_obj_0, 0, 0);

        lv_obj_t * confirm_card = lv_obj_create(lv_obj_0);
        lv_obj_set_x(confirm_card, 36);
        lv_obj_set_y(confirm_card, 14);
        lv_obj_set_width(confirm_card, 352);
        lv_obj_set_height(confirm_card, 208);
        lv_obj_set_scrollable(confirm_card, false);
        lv_obj_set_clickable(confirm_card, true);
        lv_obj_set_style_pad_all(confirm_card, -1, 0);
        lv_obj_set_style_bg_color(confirm_card, lv_color_hex(0x111722), 0);
        lv_obj_set_style_bg_opa(confirm_card, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(confirm_card, 12, 0);
        lv_obj_set_style_border_width(confirm_card, 1, 0);
        lv_obj_set_style_outline_width(confirm_card, 0, 0);
        lv_obj_set_style_shadow_width(confirm_card, 0, 0);
        lv_obj_set_style_border_color(confirm_card, lv_color_hex(0x38bdf8), 0);
        lv_obj_set_name(confirm_card, "confirm_card");
        lv_obj_t * lv_label_0 = lv_label_create(confirm_card);
        lv_obj_set_x(lv_label_0, 17);
        lv_obj_set_y(lv_label_0, 13);
        lv_label_set_text(lv_label_0, title);
        lv_obj_set_scrollable(lv_label_0, false);
        lv_obj_set_clickable(lv_label_0, false);
        lv_obj_set_style_text_font(lv_label_0, title_font, 0);
        lv_obj_set_style_text_color(lv_label_0, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_letter_space(lv_label_0, 0, 0);
        lv_obj_set_width(lv_label_0, 318);
        lv_obj_set_height(lv_label_0, 17);
        lv_label_set_long_mode(lv_label_0, LV_LABEL_LONG_MODE_DOTS);

        lv_obj_t * lv_label_1 = lv_label_create(confirm_card);
        lv_obj_set_x(lv_label_1, 17);
        lv_obj_set_y(lv_label_1, 41);
        lv_label_set_text(lv_label_1, "Controller state");
        lv_obj_set_scrollable(lv_label_1, false);
        lv_obj_set_clickable(lv_label_1, false);
        lv_obj_set_style_text_font(lv_label_1, row_font, 0);
        lv_obj_set_style_text_color(lv_label_1, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_style_text_align(lv_label_1, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_letter_space(lv_label_1, 0, 0);
        lv_obj_set_width(lv_label_1, 318);
        lv_obj_set_height(lv_label_1, 15);
        lv_label_set_long_mode(lv_label_1, LV_LABEL_LONG_MODE_DOTS);

        lv_obj_t * lv_label_2 = lv_label_create(confirm_card);
        lv_obj_set_x(lv_label_2, 17);
        lv_obj_set_y(lv_label_2, 42);
        lv_label_set_text(lv_label_2, controller_state);
        lv_obj_set_scrollable(lv_label_2, false);
        lv_obj_set_clickable(lv_label_2, false);
        lv_obj_set_style_text_font(lv_label_2, button_font, 0);
        lv_obj_set_style_text_color(lv_label_2, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_style_text_align(lv_label_2, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_set_style_text_letter_space(lv_label_2, 0, 0);
        lv_obj_set_width(lv_label_2, 319);
        lv_obj_set_height(lv_label_2, 13);
        lv_label_set_long_mode(lv_label_2, LV_LABEL_LONG_MODE_DOTS);

        lv_obj_t * lv_label_3 = lv_label_create(confirm_card);
        lv_obj_set_x(lv_label_3, 17);
        lv_obj_set_y(lv_label_3, 62);
        lv_label_set_text(lv_label_3, "UART link");
        lv_obj_set_scrollable(lv_label_3, false);
        lv_obj_set_clickable(lv_label_3, false);
        lv_obj_set_style_text_font(lv_label_3, row_font, 0);
        lv_obj_set_style_text_color(lv_label_3, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_style_text_align(lv_label_3, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_letter_space(lv_label_3, 0, 0);
        lv_obj_set_width(lv_label_3, 318);
        lv_obj_set_height(lv_label_3, 15);
        lv_label_set_long_mode(lv_label_3, LV_LABEL_LONG_MODE_DOTS);

        lv_obj_t * lv_label_4 = lv_label_create(confirm_card);
        lv_obj_set_x(lv_label_4, 17);
        lv_obj_set_y(lv_label_4, 63);
        lv_label_set_text(lv_label_4, link_status);
        lv_obj_set_scrollable(lv_label_4, false);
        lv_obj_set_clickable(lv_label_4, false);
        lv_obj_set_style_text_font(lv_label_4, button_font, 0);
        lv_obj_set_style_text_color(lv_label_4, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_style_text_align(lv_label_4, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_set_style_text_letter_space(lv_label_4, 0, 0);
        lv_obj_set_width(lv_label_4, 319);
        lv_obj_set_height(lv_label_4, 13);
        lv_label_set_long_mode(lv_label_4, LV_LABEL_LONG_MODE_DOTS);

        lv_obj_t * lv_label_5 = lv_label_create(confirm_card);
        lv_obj_set_x(lv_label_5, 17);
        lv_obj_set_y(lv_label_5, 83);
        lv_label_set_text(lv_label_5, measurement_label);
        lv_obj_set_scrollable(lv_label_5, false);
        lv_obj_set_clickable(lv_label_5, false);
        lv_obj_set_style_text_font(lv_label_5, row_font, 0);
        lv_obj_set_style_text_color(lv_label_5, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_style_text_align(lv_label_5, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_letter_space(lv_label_5, 0, 0);
        lv_obj_set_width(lv_label_5, 318);
        lv_obj_set_height(lv_label_5, 15);
        lv_label_set_long_mode(lv_label_5, LV_LABEL_LONG_MODE_DOTS);

        lv_obj_t * lv_label_6 = lv_label_create(confirm_card);
        lv_obj_set_x(lv_label_6, 17);
        lv_obj_set_y(lv_label_6, 84);
        lv_label_set_text(lv_label_6, measurement_status);
        lv_obj_set_scrollable(lv_label_6, false);
        lv_obj_set_clickable(lv_label_6, false);
        lv_obj_set_style_text_font(lv_label_6, button_font, 0);
        lv_obj_set_style_text_color(lv_label_6, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_style_text_align(lv_label_6, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_set_style_text_letter_space(lv_label_6, 0, 0);
        lv_obj_set_width(lv_label_6, 319);
        lv_obj_set_height(lv_label_6, 13);
        lv_label_set_long_mode(lv_label_6, LV_LABEL_LONG_MODE_DOTS);

        lv_obj_t * lv_label_7 = lv_label_create(confirm_card);
        lv_obj_set_x(lv_label_7, 17);
        lv_obj_set_y(lv_label_7, 104);
        lv_label_set_text(lv_label_7, "Fault / Inhibit");
        lv_obj_set_scrollable(lv_label_7, false);
        lv_obj_set_clickable(lv_label_7, false);
        lv_obj_set_style_text_font(lv_label_7, row_font, 0);
        lv_obj_set_style_text_color(lv_label_7, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_style_text_align(lv_label_7, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_letter_space(lv_label_7, 0, 0);
        lv_obj_set_width(lv_label_7, 318);
        lv_obj_set_height(lv_label_7, 15);
        lv_label_set_long_mode(lv_label_7, LV_LABEL_LONG_MODE_DOTS);

        lv_obj_t * lv_label_8 = lv_label_create(confirm_card);
        lv_obj_set_x(lv_label_8, 17);
        lv_obj_set_y(lv_label_8, 105);
        lv_label_set_text(lv_label_8, fault_status);
        lv_obj_set_scrollable(lv_label_8, false);
        lv_obj_set_clickable(lv_label_8, false);
        lv_obj_set_style_text_font(lv_label_8, button_font, 0);
        lv_obj_set_style_text_color(lv_label_8, fault_color, 0);
        lv_obj_set_style_text_align(lv_label_8, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_set_style_text_letter_space(lv_label_8, 0, 0);
        lv_obj_set_width(lv_label_8, 319);
        lv_obj_set_height(lv_label_8, 13);
        lv_label_set_long_mode(lv_label_8, LV_LABEL_LONG_MODE_DOTS);

        lv_obj_t * lv_label_9 = lv_label_create(confirm_card);
        lv_obj_set_x(lv_label_9, 17);
        lv_obj_set_y(lv_label_9, 128);
        lv_label_set_text(lv_label_9, footnote);
        lv_obj_set_scrollable(lv_label_9, false);
        lv_obj_set_clickable(lv_label_9, false);
        lv_obj_set_style_text_font(lv_label_9, body_font, 0);
        lv_obj_set_style_text_color(lv_label_9, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_style_text_align(lv_label_9, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_letter_space(lv_label_9, 0, 0);
        lv_obj_set_width(lv_label_9, 318);
        lv_obj_set_height(lv_label_9, 13);
        lv_label_set_long_mode(lv_label_9, LV_LABEL_LONG_MODE_DOTS);

        lv_obj_t * confirm_cancel = lv_button_create(confirm_card);
        lv_obj_set_x(confirm_cancel, 17);
        lv_obj_set_y(confirm_cancel, 157);
        lv_obj_set_width(confirm_cancel, 154);
        lv_obj_set_height(confirm_cancel, 38);
        lv_obj_set_scrollable(confirm_cancel, false);
        lv_obj_set_clickable(confirm_cancel, true);
        lv_obj_set_style_pad_all(confirm_cancel, 0, 0);
        lv_obj_set_style_bg_color(confirm_cancel, lv_color_hex(0x172030), 0);
        lv_obj_set_style_bg_opa(confirm_cancel, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(confirm_cancel, 10, 0);
        lv_obj_set_style_border_width(confirm_cancel, 0, 0);
        lv_obj_set_style_outline_width(confirm_cancel, 0, 0);
        lv_obj_set_style_shadow_width(confirm_cancel, 0, 0);
        lv_obj_set_style_bg_opa(confirm_cancel, 178, LV_STATE_PRESSED);
        lv_obj_set_name(confirm_cancel, "confirm_cancel");
        lv_obj_t * lv_label_10 = lv_label_create(confirm_cancel);
        lv_obj_set_x(lv_label_10, 0);
        lv_obj_set_y(lv_label_10, 12);
        lv_label_set_text(lv_label_10, "CANCEL");
        lv_obj_set_scrollable(lv_label_10, false);
        lv_obj_set_clickable(lv_label_10, false);
        lv_obj_set_style_text_font(lv_label_10, button_font, 0);
        lv_obj_set_style_text_color(lv_label_10, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_style_text_align(lv_label_10, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_letter_space(lv_label_10, 1, 0);
        lv_obj_set_width(lv_label_10, 154);
        lv_obj_set_height(lv_label_10, 13);
        lv_label_set_long_mode(lv_label_10, LV_LABEL_LONG_MODE_DOTS);

        lv_obj_t * confirm_send = lv_button_create(confirm_card);
        lv_obj_set_x(confirm_send, 181);
        lv_obj_set_y(confirm_send, 157);
        lv_obj_set_width(confirm_send, 154);
        lv_obj_set_height(confirm_send, 38);
        lv_obj_set_scrollable(confirm_send, false);
        lv_obj_set_clickable(confirm_send, true);
        lv_obj_set_style_pad_all(confirm_send, 0, 0);
        lv_obj_set_style_bg_color(confirm_send, lv_color_hex(0x0ea5e9), 0);
        lv_obj_set_style_bg_opa(confirm_send, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(confirm_send, 10, 0);
        lv_obj_set_style_border_width(confirm_send, 0, 0);
        lv_obj_set_style_outline_width(confirm_send, 0, 0);
        lv_obj_set_style_shadow_width(confirm_send, 0, 0);
        lv_obj_set_style_bg_opa(confirm_send, 178, LV_STATE_PRESSED);
        lv_obj_set_name(confirm_send, "confirm_send");
        lv_obj_t * lv_label_11 = lv_label_create(confirm_send);
        lv_obj_set_x(lv_label_11, 0);
        lv_obj_set_y(lv_label_11, 12);
        lv_label_set_text(lv_label_11, "SEND START");
        lv_obj_set_scrollable(lv_label_11, false);
        lv_obj_set_clickable(lv_label_11, false);
        lv_obj_set_style_text_font(lv_label_11, button_font, 0);
        lv_obj_set_style_text_color(lv_label_11, lv_color_hex(0x06090e), 0);
        lv_obj_set_style_text_align(lv_label_11, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_letter_space(lv_label_11, 1, 0);
        lv_obj_set_width(lv_label_11, 154);
        lv_obj_set_height(lv_label_11, 13);
        lv_label_set_long_mode(lv_label_11, LV_LABEL_LONG_MODE_DOTS);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

