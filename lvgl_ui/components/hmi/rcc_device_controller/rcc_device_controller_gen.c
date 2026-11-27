/**
 * @file rcc_device_controller_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "rcc_device_controller_gen.h"
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

lv_obj_t * rcc_device_controller_create(lv_obj_t * parent, int32_t card_height, lv_font_t * caption_font, lv_font_t * body_font, lv_font_t * value_font)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "rcc_device_controller_#");
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
        lv_obj_set_width(lv_obj_1, 200);
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
        lv_obj_t * lv_label_0 = lv_label_create(lv_obj_1);
        lv_obj_set_x(lv_label_0, 13);
        lv_obj_set_y(lv_label_0, 8);
        lv_label_set_text(lv_label_0, "FIRMWARE");
        lv_obj_set_style_text_font(lv_label_0, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_0, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_clickable(lv_label_0, false);
        lv_obj_set_scrollable(lv_label_0, false);
        lv_obj_set_style_text_letter_space(lv_label_0, 1, 0);

        lv_obj_t * device_compat_box = lv_obj_create(lv_obj_1);
        lv_obj_set_x(device_compat_box, 103);
        lv_obj_set_y(device_compat_box, 7);
        lv_obj_set_width(device_compat_box, 84);
        lv_obj_set_height(device_compat_box, 17);
        lv_obj_set_clickable(device_compat_box, false);
        lv_obj_set_scrollable(device_compat_box, false);
        lv_obj_set_style_bg_color(device_compat_box, lv_color_hex(0x0a2617), 0);
        lv_obj_set_style_bg_opa(device_compat_box, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(device_compat_box, 6, 0);
        lv_obj_set_style_border_width(device_compat_box, 0, 0);
        lv_obj_set_style_outline_width(device_compat_box, 0, 0);
        lv_obj_set_style_shadow_width(device_compat_box, 0, 0);
        lv_obj_set_style_pad_all(device_compat_box, 0, 0);
        lv_obj_set_name(device_compat_box, "device_compat_box");

        lv_obj_t * device_compat_text = lv_label_create(lv_obj_1);
        lv_obj_set_x(device_compat_text, 110);
        lv_obj_set_y(device_compat_text, 9);
        lv_label_set_text(device_compat_text, "COMPATIBLE");
        lv_obj_set_style_text_font(device_compat_text, caption_font, 0);
        lv_obj_set_style_text_color(device_compat_text, lv_color_hex(0x22c55e), 0);
        lv_obj_set_clickable(device_compat_text, false);
        lv_obj_set_scrollable(device_compat_text, false);
        lv_obj_set_name(device_compat_text, "device_compat_text");

        lv_obj_t * lv_label_1 = lv_label_create(lv_obj_1);
        lv_obj_set_x(lv_label_1, 13);
        lv_obj_set_y(lv_label_1, 23 + 1 * ((card_height - 102) / 4) + 1);
        lv_label_set_text(lv_label_1, "Product");
        lv_obj_set_style_text_font(lv_label_1, body_font, 0);
        lv_obj_set_style_text_color(lv_label_1, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_clickable(lv_label_1, false);
        lv_obj_set_scrollable(lv_label_1, false);
        lv_obj_set_width(lv_label_1, 174);
        lv_obj_set_height(lv_label_1, 15);
        lv_label_set_long_mode(lv_label_1, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_1, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * device_product = lv_label_create(lv_obj_1);
        lv_obj_set_x(device_product, 16);
        lv_obj_set_y(device_product, 23 + 1 * ((card_height - 102) / 4) + 2);
        lv_label_set_text(device_product, "0x0001 · v1");
        lv_obj_set_style_text_font(device_product, value_font, 0);
        lv_obj_set_style_text_color(device_product, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_clickable(device_product, false);
        lv_obj_set_scrollable(device_product, false);
        lv_obj_set_width(device_product, 172);
        lv_obj_set_height(device_product, 13);
        lv_label_set_long_mode(device_product, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(device_product, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_set_name(device_product, "device_product");

        lv_obj_t * lv_label_2 = lv_label_create(lv_obj_1);
        lv_obj_set_x(lv_label_2, 13);
        lv_obj_set_y(lv_label_2, 41 + 2 * ((card_height - 102) / 4) + 1);
        lv_label_set_text(lv_label_2, "HW / FW");
        lv_obj_set_style_text_font(lv_label_2, body_font, 0);
        lv_obj_set_style_text_color(lv_label_2, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_clickable(lv_label_2, false);
        lv_obj_set_scrollable(lv_label_2, false);
        lv_obj_set_width(lv_label_2, 174);
        lv_obj_set_height(lv_label_2, 15);
        lv_label_set_long_mode(lv_label_2, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_2, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * device_revision = lv_label_create(lv_obj_1);
        lv_obj_set_x(device_revision, 16);
        lv_obj_set_y(device_revision, 41 + 2 * ((card_height - 102) / 4) + 2);
        lv_label_set_text(device_revision, "0x01 / 0x07");
        lv_obj_set_style_text_font(device_revision, value_font, 0);
        lv_obj_set_style_text_color(device_revision, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_clickable(device_revision, false);
        lv_obj_set_scrollable(device_revision, false);
        lv_obj_set_width(device_revision, 172);
        lv_obj_set_height(device_revision, 13);
        lv_label_set_long_mode(device_revision, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(device_revision, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_set_name(device_revision, "device_revision");

        lv_obj_t * lv_label_3 = lv_label_create(lv_obj_1);
        lv_obj_set_x(lv_label_3, 13);
        lv_obj_set_y(lv_label_3, 59 + 3 * ((card_height - 102) / 4) + 1);
        lv_label_set_text(lv_label_3, "Image CRC");
        lv_obj_set_style_text_font(lv_label_3, body_font, 0);
        lv_obj_set_style_text_color(lv_label_3, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_clickable(lv_label_3, false);
        lv_obj_set_scrollable(lv_label_3, false);
        lv_obj_set_width(lv_label_3, 174);
        lv_obj_set_height(lv_label_3, 15);
        lv_label_set_long_mode(lv_label_3, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_3, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * device_image = lv_label_create(lv_obj_1);
        lv_obj_set_x(device_image, 16);
        lv_obj_set_y(device_image, 59 + 3 * ((card_height - 102) / 4) + 2);
        lv_label_set_text(device_image, "9F3A…E815");
        lv_obj_set_style_text_font(device_image, value_font, 0);
        lv_obj_set_style_text_color(device_image, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_clickable(device_image, false);
        lv_obj_set_scrollable(device_image, false);
        lv_obj_set_width(device_image, 172);
        lv_obj_set_height(device_image, 13);
        lv_label_set_long_mode(device_image, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(device_image, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_set_name(device_image, "device_image");

        lv_obj_t * lv_label_4 = lv_label_create(lv_obj_1);
        lv_obj_set_x(lv_label_4, 13);
        lv_obj_set_y(lv_label_4, 77 + 4 * ((card_height - 102) / 4) + 1);
        lv_label_set_text(lv_label_4, "Boot ID");
        lv_obj_set_style_text_font(lv_label_4, body_font, 0);
        lv_obj_set_style_text_color(lv_label_4, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_clickable(lv_label_4, false);
        lv_obj_set_scrollable(lv_label_4, false);
        lv_obj_set_width(lv_label_4, 174);
        lv_obj_set_height(lv_label_4, 15);
        lv_label_set_long_mode(lv_label_4, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_4, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * device_boot = lv_label_create(lv_obj_1);
        lv_obj_set_x(device_boot, 16);
        lv_obj_set_y(device_boot, 77 + 4 * ((card_height - 102) / 4) + 2);
        lv_label_set_text(device_boot, "…E4B2");
        lv_obj_set_style_text_font(device_boot, value_font, 0);
        lv_obj_set_style_text_color(device_boot, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_clickable(device_boot, false);
        lv_obj_set_scrollable(device_boot, false);
        lv_obj_set_width(device_boot, 172);
        lv_obj_set_height(device_boot, 13);
        lv_label_set_long_mode(device_boot, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(device_boot, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_set_name(device_boot, "device_boot");

        lv_obj_t * lv_obj_2 = lv_obj_create(lv_obj_0);
        lv_obj_set_x(lv_obj_2, 208);
        lv_obj_set_y(lv_obj_2, 0);
        lv_obj_set_width(lv_obj_2, 200);
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
        lv_obj_t * lv_label_5 = lv_label_create(lv_obj_2);
        lv_obj_set_x(lv_label_5, 13);
        lv_obj_set_y(lv_label_5, 8);
        lv_label_set_text(lv_label_5, "BUILD FLAGS & BUSES");
        lv_obj_set_style_text_font(lv_label_5, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_5, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_clickable(lv_label_5, false);
        lv_obj_set_scrollable(lv_label_5, false);
        lv_obj_set_style_text_letter_space(lv_label_5, 1, 0);

        lv_obj_t * lv_label_6 = lv_label_create(lv_obj_2);
        lv_obj_set_x(lv_label_6, 13);
        lv_obj_set_y(lv_label_6, 23 + 1 * ((card_height - 104) / 4) + 1);
        lv_label_set_text(lv_label_6, "Provisional");
        lv_obj_set_style_text_font(lv_label_6, body_font, 0);
        lv_obj_set_style_text_color(lv_label_6, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_clickable(lv_label_6, false);
        lv_obj_set_scrollable(lv_label_6, false);
        lv_obj_set_width(lv_label_6, 120);
        lv_obj_set_height(lv_label_6, 15);
        lv_label_set_long_mode(lv_label_6, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_6, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * device_flag0_unknown = lv_label_create(lv_obj_2);
        lv_obj_set_x(device_flag0_unknown, 13);
        lv_obj_set_y(device_flag0_unknown, 23 + 1 * ((card_height - 104) / 4) + 2);
        lv_label_set_text(device_flag0_unknown, "—");
        lv_obj_set_style_text_font(device_flag0_unknown, value_font, 0);
        lv_obj_set_style_text_color(device_flag0_unknown, lv_color_hex(0x64748b), 0);
        lv_obj_set_clickable(device_flag0_unknown, false);
        lv_obj_set_scrollable(device_flag0_unknown, false);
        lv_obj_set_width(device_flag0_unknown, 174);
        lv_obj_set_height(device_flag0_unknown, 13);
        lv_label_set_long_mode(device_flag0_unknown, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(device_flag0_unknown, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_set_name(device_flag0_unknown, "device_flag0_unknown");
        lv_obj_set_hidden(device_flag0_unknown, true);

        lv_obj_t * device_flag0_box = lv_obj_create(lv_obj_2);
        lv_obj_set_x(device_flag0_box, 157);
        lv_obj_set_y(device_flag0_box, 23 + 1 * ((card_height - 104) / 4));
        lv_obj_set_width(device_flag0_box, 30);
        lv_obj_set_height(device_flag0_box, 17);
        lv_obj_set_clickable(device_flag0_box, false);
        lv_obj_set_scrollable(device_flag0_box, false);
        lv_obj_set_style_bg_color(device_flag0_box, lv_color_hex(0x2b2309), 0);
        lv_obj_set_style_bg_opa(device_flag0_box, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(device_flag0_box, 6, 0);
        lv_obj_set_style_border_width(device_flag0_box, 0, 0);
        lv_obj_set_style_outline_width(device_flag0_box, 0, 0);
        lv_obj_set_style_shadow_width(device_flag0_box, 0, 0);
        lv_obj_set_style_pad_all(device_flag0_box, 0, 0);
        lv_obj_set_name(device_flag0_box, "device_flag0_box");

        lv_obj_t * device_flag0_text = lv_label_create(lv_obj_2);
        lv_obj_set_x(device_flag0_text, 164);
        lv_obj_set_y(device_flag0_text, 23 + 1 * ((card_height - 104) / 4) + 2);
        lv_label_set_text(device_flag0_text, "ON");
        lv_obj_set_style_text_font(device_flag0_text, caption_font, 0);
        lv_obj_set_style_text_color(device_flag0_text, lv_color_hex(0xfacc15), 0);
        lv_obj_set_clickable(device_flag0_text, false);
        lv_obj_set_scrollable(device_flag0_text, false);
        lv_obj_set_name(device_flag0_text, "device_flag0_text");

        lv_obj_t * lv_label_7 = lv_label_create(lv_obj_2);
        lv_obj_set_x(lv_label_7, 13);
        lv_obj_set_y(lv_label_7, 41 + 2 * ((card_height - 104) / 4) + 1);
        lv_label_set_text(lv_label_7, "Relay enable");
        lv_obj_set_style_text_font(lv_label_7, body_font, 0);
        lv_obj_set_style_text_color(lv_label_7, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_clickable(lv_label_7, false);
        lv_obj_set_scrollable(lv_label_7, false);
        lv_obj_set_width(lv_label_7, 120);
        lv_obj_set_height(lv_label_7, 15);
        lv_label_set_long_mode(lv_label_7, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_7, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * device_flag1_unknown = lv_label_create(lv_obj_2);
        lv_obj_set_x(device_flag1_unknown, 13);
        lv_obj_set_y(device_flag1_unknown, 41 + 2 * ((card_height - 104) / 4) + 2);
        lv_label_set_text(device_flag1_unknown, "—");
        lv_obj_set_style_text_font(device_flag1_unknown, value_font, 0);
        lv_obj_set_style_text_color(device_flag1_unknown, lv_color_hex(0x64748b), 0);
        lv_obj_set_clickable(device_flag1_unknown, false);
        lv_obj_set_scrollable(device_flag1_unknown, false);
        lv_obj_set_width(device_flag1_unknown, 174);
        lv_obj_set_height(device_flag1_unknown, 13);
        lv_label_set_long_mode(device_flag1_unknown, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(device_flag1_unknown, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_set_name(device_flag1_unknown, "device_flag1_unknown");
        lv_obj_set_hidden(device_flag1_unknown, true);

        lv_obj_t * device_flag1_box = lv_obj_create(lv_obj_2);
        lv_obj_set_x(device_flag1_box, 157);
        lv_obj_set_y(device_flag1_box, 41 + 2 * ((card_height - 104) / 4));
        lv_obj_set_width(device_flag1_box, 30);
        lv_obj_set_height(device_flag1_box, 17);
        lv_obj_set_clickable(device_flag1_box, false);
        lv_obj_set_scrollable(device_flag1_box, false);
        lv_obj_set_style_bg_color(device_flag1_box, lv_color_hex(0x2b2309), 0);
        lv_obj_set_style_bg_opa(device_flag1_box, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(device_flag1_box, 6, 0);
        lv_obj_set_style_border_width(device_flag1_box, 0, 0);
        lv_obj_set_style_outline_width(device_flag1_box, 0, 0);
        lv_obj_set_style_shadow_width(device_flag1_box, 0, 0);
        lv_obj_set_style_pad_all(device_flag1_box, 0, 0);
        lv_obj_set_name(device_flag1_box, "device_flag1_box");

        lv_obj_t * device_flag1_text = lv_label_create(lv_obj_2);
        lv_obj_set_x(device_flag1_text, 164);
        lv_obj_set_y(device_flag1_text, 41 + 2 * ((card_height - 104) / 4) + 2);
        lv_label_set_text(device_flag1_text, "ON");
        lv_obj_set_style_text_font(device_flag1_text, caption_font, 0);
        lv_obj_set_style_text_color(device_flag1_text, lv_color_hex(0xfacc15), 0);
        lv_obj_set_clickable(device_flag1_text, false);
        lv_obj_set_scrollable(device_flag1_text, false);
        lv_obj_set_name(device_flag1_text, "device_flag1_text");

        lv_obj_t * lv_label_8 = lv_label_create(lv_obj_2);
        lv_obj_set_x(lv_label_8, 13);
        lv_obj_set_y(lv_label_8, 59 + 3 * ((card_height - 104) / 4) + 1);
        lv_label_set_text(lv_label_8, "Control");
        lv_obj_set_style_text_font(lv_label_8, body_font, 0);
        lv_obj_set_style_text_color(lv_label_8, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_clickable(lv_label_8, false);
        lv_obj_set_scrollable(lv_label_8, false);
        lv_obj_set_width(lv_label_8, 120);
        lv_obj_set_height(lv_label_8, 15);
        lv_label_set_long_mode(lv_label_8, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(lv_label_8, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * device_flag2_unknown = lv_label_create(lv_obj_2);
        lv_obj_set_x(device_flag2_unknown, 13);
        lv_obj_set_y(device_flag2_unknown, 59 + 3 * ((card_height - 104) / 4) + 2);
        lv_label_set_text(device_flag2_unknown, "—");
        lv_obj_set_style_text_font(device_flag2_unknown, value_font, 0);
        lv_obj_set_style_text_color(device_flag2_unknown, lv_color_hex(0x64748b), 0);
        lv_obj_set_clickable(device_flag2_unknown, false);
        lv_obj_set_scrollable(device_flag2_unknown, false);
        lv_obj_set_width(device_flag2_unknown, 174);
        lv_obj_set_height(device_flag2_unknown, 13);
        lv_label_set_long_mode(device_flag2_unknown, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(device_flag2_unknown, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_set_name(device_flag2_unknown, "device_flag2_unknown");
        lv_obj_set_hidden(device_flag2_unknown, true);

        lv_obj_t * device_flag2_box = lv_obj_create(lv_obj_2);
        lv_obj_set_x(device_flag2_box, 142);
        lv_obj_set_y(device_flag2_box, 59 + 3 * ((card_height - 104) / 4));
        lv_obj_set_width(device_flag2_box, 45);
        lv_obj_set_height(device_flag2_box, 17);
        lv_obj_set_clickable(device_flag2_box, false);
        lv_obj_set_scrollable(device_flag2_box, false);
        lv_obj_set_style_bg_color(device_flag2_box, lv_color_hex(0x172030), 0);
        lv_obj_set_style_bg_opa(device_flag2_box, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(device_flag2_box, 6, 0);
        lv_obj_set_style_border_width(device_flag2_box, 0, 0);
        lv_obj_set_style_outline_width(device_flag2_box, 0, 0);
        lv_obj_set_style_shadow_width(device_flag2_box, 0, 0);
        lv_obj_set_style_pad_all(device_flag2_box, 0, 0);
        lv_obj_set_name(device_flag2_box, "device_flag2_box");

        lv_obj_t * device_flag2_text = lv_label_create(lv_obj_2);
        lv_obj_set_x(device_flag2_text, 149);
        lv_obj_set_y(device_flag2_text, 59 + 3 * ((card_height - 104) / 4) + 2);
        lv_label_set_text(device_flag2_text, "NONE");
        lv_obj_set_style_text_font(device_flag2_text, caption_font, 0);
        lv_obj_set_style_text_color(device_flag2_text, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_clickable(device_flag2_text, false);
        lv_obj_set_scrollable(device_flag2_text, false);
        lv_obj_set_name(device_flag2_text, "device_flag2_text");

        lv_obj_t * device_bus0_box = lv_obj_create(lv_obj_2);
        lv_obj_set_x(device_bus0_box, 13);
        lv_obj_set_y(device_bus0_box, 78 + 4 * ((card_height - 104) / 4));
        lv_obj_set_width(device_bus0_box, 37);
        lv_obj_set_height(device_bus0_box, 17);
        lv_obj_set_clickable(device_bus0_box, false);
        lv_obj_set_scrollable(device_bus0_box, false);
        lv_obj_set_style_bg_color(device_bus0_box, lv_color_hex(0x0c2538), 0);
        lv_obj_set_style_bg_opa(device_bus0_box, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(device_bus0_box, 6, 0);
        lv_obj_set_style_border_width(device_bus0_box, 0, 0);
        lv_obj_set_style_outline_width(device_bus0_box, 0, 0);
        lv_obj_set_style_shadow_width(device_bus0_box, 0, 0);
        lv_obj_set_style_pad_all(device_bus0_box, 0, 0);
        lv_obj_set_name(device_bus0_box, "device_bus0_box");

        lv_obj_t * device_bus0_text = lv_label_create(lv_obj_2);
        lv_obj_set_x(device_bus0_text, 20);
        lv_obj_set_y(device_bus0_text, 78 + 4 * ((card_height - 104) / 4) + 2);
        lv_label_set_text(device_bus0_text, "CAN");
        lv_obj_set_style_text_font(device_bus0_text, caption_font, 0);
        lv_obj_set_style_text_color(device_bus0_text, lv_color_hex(0x38bdf8), 0);
        lv_obj_set_clickable(device_bus0_text, false);
        lv_obj_set_scrollable(device_bus0_text, false);
        lv_obj_set_name(device_bus0_text, "device_bus0_text");

        lv_obj_t * device_bus1_box = lv_obj_create(lv_obj_2);
        lv_obj_set_x(device_bus1_box, 54);
        lv_obj_set_y(device_bus1_box, 78 + 4 * ((card_height - 104) / 4));
        lv_obj_set_width(device_bus1_box, 47);
        lv_obj_set_height(device_bus1_box, 17);
        lv_obj_set_clickable(device_bus1_box, false);
        lv_obj_set_scrollable(device_bus1_box, false);
        lv_obj_set_style_bg_color(device_bus1_box, lv_color_hex(0x0c2538), 0);
        lv_obj_set_style_bg_opa(device_bus1_box, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(device_bus1_box, 6, 0);
        lv_obj_set_style_border_width(device_bus1_box, 0, 0);
        lv_obj_set_style_outline_width(device_bus1_box, 0, 0);
        lv_obj_set_style_shadow_width(device_bus1_box, 0, 0);
        lv_obj_set_style_pad_all(device_bus1_box, 0, 0);
        lv_obj_set_name(device_bus1_box, "device_bus1_box");

        lv_obj_t * device_bus1_text = lv_label_create(lv_obj_2);
        lv_obj_set_x(device_bus1_text, 61);
        lv_obj_set_y(device_bus1_text, 78 + 4 * ((card_height - 104) / 4) + 2);
        lv_label_set_text(device_bus1_text, "RS485");
        lv_obj_set_style_text_font(device_bus1_text, caption_font, 0);
        lv_obj_set_style_text_color(device_bus1_text, lv_color_hex(0x38bdf8), 0);
        lv_obj_set_clickable(device_bus1_text, false);
        lv_obj_set_scrollable(device_bus1_text, false);
        lv_obj_set_name(device_bus1_text, "device_bus1_text");

        lv_obj_t * device_bus2_box = lv_obj_create(lv_obj_2);
        lv_obj_set_x(device_bus2_box, 105);
        lv_obj_set_y(device_bus2_box, 78 + 4 * ((card_height - 104) / 4));
        lv_obj_set_width(device_bus2_box, 43);
        lv_obj_set_height(device_bus2_box, 17);
        lv_obj_set_clickable(device_bus2_box, false);
        lv_obj_set_scrollable(device_bus2_box, false);
        lv_obj_set_style_bg_color(device_bus2_box, lv_color_hex(0x0c2538), 0);
        lv_obj_set_style_bg_opa(device_bus2_box, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(device_bus2_box, 6, 0);
        lv_obj_set_style_border_width(device_bus2_box, 0, 0);
        lv_obj_set_style_outline_width(device_bus2_box, 0, 0);
        lv_obj_set_style_shadow_width(device_bus2_box, 0, 0);
        lv_obj_set_style_pad_all(device_bus2_box, 0, 0);
        lv_obj_set_name(device_bus2_box, "device_bus2_box");

        lv_obj_t * device_bus2_text = lv_label_create(lv_obj_2);
        lv_obj_set_x(device_bus2_text, 112);
        lv_obj_set_y(device_bus2_text, 78 + 4 * ((card_height - 104) / 4) + 2);
        lv_label_set_text(device_bus2_text, "UART");
        lv_obj_set_style_text_font(device_bus2_text, caption_font, 0);
        lv_obj_set_style_text_color(device_bus2_text, lv_color_hex(0x38bdf8), 0);
        lv_obj_set_clickable(device_bus2_text, false);
        lv_obj_set_scrollable(device_bus2_text, false);
        lv_obj_set_name(device_bus2_text, "device_bus2_text");

        lv_obj_t * device_bus3_box = lv_obj_create(lv_obj_2);
        lv_obj_set_x(device_bus3_box, 152);
        lv_obj_set_y(device_bus3_box, 78 + 4 * ((card_height - 104) / 4));
        lv_obj_set_width(device_bus3_box, 35);
        lv_obj_set_height(device_bus3_box, 17);
        lv_obj_set_clickable(device_bus3_box, false);
        lv_obj_set_scrollable(device_bus3_box, false);
        lv_obj_set_style_bg_color(device_bus3_box, lv_color_hex(0x0c2538), 0);
        lv_obj_set_style_bg_opa(device_bus3_box, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(device_bus3_box, 6, 0);
        lv_obj_set_style_border_width(device_bus3_box, 0, 0);
        lv_obj_set_style_outline_width(device_bus3_box, 0, 0);
        lv_obj_set_style_shadow_width(device_bus3_box, 0, 0);
        lv_obj_set_style_pad_all(device_bus3_box, 0, 0);
        lv_obj_set_name(device_bus3_box, "device_bus3_box");

        lv_obj_t * device_bus3_text = lv_label_create(lv_obj_2);
        lv_obj_set_x(device_bus3_text, 159);
        lv_obj_set_y(device_bus3_text, 78 + 4 * ((card_height - 104) / 4) + 2);
        lv_label_set_text(device_bus3_text, "SVC");
        lv_obj_set_style_text_font(device_bus3_text, caption_font, 0);
        lv_obj_set_style_text_color(device_bus3_text, lv_color_hex(0x38bdf8), 0);
        lv_obj_set_clickable(device_bus3_text, false);
        lv_obj_set_scrollable(device_bus3_text, false);
        lv_obj_set_name(device_bus3_text, "device_bus3_text");

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

