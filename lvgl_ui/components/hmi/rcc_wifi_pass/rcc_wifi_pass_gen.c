/**
 * @file rcc_wifi_pass_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "rcc_wifi_pass_gen.h"
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

lv_obj_t * rcc_wifi_pass_create(lv_obj_t * parent, bool symbols, bool shifted, const char * count_text, const char * toggle_text, lv_font_t * caption_font, lv_font_t * data_font, const void * eye_icon, const void * shift_icon, const void * erase_icon)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "rcc_wifi_pass_#");
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

        lv_obj_t * lv_label_0 = lv_label_create(lv_obj_0);
        lv_obj_set_x(lv_label_0, 8);
        lv_obj_set_y(lv_label_0, 46);
        lv_obj_set_width(lv_label_0, 68);
        lv_obj_set_height(lv_label_0, 13);
        lv_obj_set_clickable(lv_label_0, false);
        lv_obj_set_style_text_font(lv_label_0, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_0, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_scrollable(lv_label_0, false);
        lv_label_set_text(lv_label_0, "PASSWORD");
        lv_label_set_long_mode(lv_label_0, LV_LABEL_LONG_MODE_WRAP);
        lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_letter_space(lv_label_0, 1, 0);

        lv_obj_t * password_count = lv_label_create(lv_obj_0);
        lv_obj_set_x(password_count, 372);
        lv_obj_set_y(password_count, 46);
        lv_obj_set_width(password_count, 100);
        lv_obj_set_height(password_count, 13);
        lv_obj_set_clickable(password_count, false);
        lv_obj_set_style_text_font(password_count, caption_font, 0);
        lv_obj_set_style_text_color(password_count, lv_color_hex(0x64748b), 0);
        lv_obj_set_scrollable(password_count, false);
        lv_label_set_text(password_count, count_text);
        lv_label_set_long_mode(password_count, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(password_count, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_set_name(password_count, "password_count");

        lv_obj_t * lv_obj_1 = lv_obj_create(lv_obj_0);
        lv_obj_set_x(lv_obj_1, 8);
        lv_obj_set_y(lv_obj_1, 62);
        lv_obj_set_width(lv_obj_1, 464);
        lv_obj_set_height(lv_obj_1, 34);
        lv_obj_set_clickable(lv_obj_1, false);
        lv_obj_set_style_radius(lv_obj_1, 10, 0);
        lv_obj_set_style_border_width(lv_obj_1, 1, 0);
        lv_obj_set_style_bg_opa(lv_obj_1, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(lv_obj_1, lv_color_hex(0x111722), 0);
        lv_obj_set_style_border_color(lv_obj_1, lv_color_hex(0x38bdf8), 0);
        lv_obj_set_style_pad_top(lv_obj_1, -1, 0);
        lv_obj_set_style_pad_bottom(lv_obj_1, -1, 0);
        lv_obj_set_style_pad_left(lv_obj_1, -1, 0);
        lv_obj_set_style_pad_right(lv_obj_1, -1, 0);
        lv_obj_set_scrollable(lv_obj_1, false);
        lv_obj_set_style_shadow_width(lv_obj_1, 0, 0);
        lv_obj_set_style_outline_width(lv_obj_1, 0, 0);
        lv_obj_t * password_field = lv_obj_create(lv_obj_1);
        lv_obj_set_x(password_field, 0);
        lv_obj_set_y(password_field, 0);
        lv_obj_set_width(password_field, 440);
        lv_obj_set_height(password_field, 34);
        lv_obj_set_clickable(password_field, false);
        lv_obj_set_style_radius(password_field, 0, 0);
        lv_obj_set_style_border_width(password_field, 0, 0);
        lv_obj_set_style_bg_opa(password_field, (255 * 0 / 100), 0);
        lv_obj_set_style_pad_top(password_field, 0, 0);
        lv_obj_set_style_pad_bottom(password_field, 0, 0);
        lv_obj_set_style_pad_left(password_field, 0, 0);
        lv_obj_set_style_pad_right(password_field, 0, 0);
        lv_obj_set_scrollable(password_field, false);
        lv_obj_set_style_shadow_width(password_field, 0, 0);
        lv_obj_set_style_outline_width(password_field, 0, 0);
        lv_obj_set_name(password_field, "password_field");

        lv_obj_t * password_eye = lv_button_create(lv_obj_1);
        lv_obj_set_x(password_eye, 420);
        lv_obj_set_y(password_eye, 0);
        lv_obj_set_width(password_eye, 44);
        lv_obj_set_height(password_eye, 34);
        lv_obj_set_clickable(password_eye, true);
        lv_obj_set_style_radius(password_eye, 10, 0);
        lv_obj_set_style_border_width(password_eye, 0, 0);
        lv_obj_set_style_bg_opa(password_eye, (255 * 0 / 100), 0);
        lv_obj_set_style_pad_top(password_eye, 0, 0);
        lv_obj_set_style_pad_bottom(password_eye, 0, 0);
        lv_obj_set_style_pad_left(password_eye, 0, 0);
        lv_obj_set_style_pad_right(password_eye, 0, 0);
        lv_obj_set_scrollable(password_eye, false);
        lv_obj_set_style_shadow_width(password_eye, 0, 0);
        lv_obj_set_style_outline_width(password_eye, 0, 0);
        lv_obj_set_name(password_eye, "password_eye");
        lv_obj_t * password_eye_icon = lv_image_create(password_eye);
        lv_obj_set_x(password_eye_icon, 15);
        lv_obj_set_y(password_eye_icon, 10);
        lv_obj_set_width(password_eye_icon, 14);
        lv_obj_set_height(password_eye_icon, 14);
        lv_obj_set_clickable(password_eye_icon, false);
        lv_obj_set_scrollable(password_eye_icon, false);
        lv_image_set_src(password_eye_icon, eye_icon);
        lv_obj_set_style_image_recolor(password_eye_icon, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_style_image_recolor_opa(password_eye_icon, (255 * 100 / 100), 0);
        lv_obj_set_name(password_eye_icon, "password_eye_icon");

        lv_obj_t * matrix_0 = lv_buttonmatrix_create(lv_obj_0);
        lv_obj_set_name(matrix_0, "matrix_0");
        lv_obj_set_x(matrix_0, 4);
        lv_obj_set_y(matrix_0, 112);
        lv_obj_set_width(matrix_0, 472);
        lv_obj_set_height(matrix_0, 38);
        static const char *matrix_0_map_0[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0", NULL};
        lv_buttonmatrix_set_map(matrix_0, matrix_0_map_0);
        lv_obj_set_scrollable(matrix_0, false);
        lv_obj_set_style_bg_opa(matrix_0, (255 * 0 / 100), 0);
        lv_obj_set_style_border_width(matrix_0, 0, 0);
        lv_obj_set_style_pad_all(matrix_0, 0, 0);
        lv_obj_set_style_pad_gap(matrix_0, 3, 0);
        lv_obj_set_style_radius(matrix_0, 0, 0);
        lv_obj_set_style_outline_width(matrix_0, 0, 0);
        lv_obj_set_style_shadow_width(matrix_0, 0, 0);
        lv_obj_set_style_bg_color(matrix_0, lv_color_hex(0x172030), LV_PART_ITEMS);
        lv_obj_set_style_bg_opa(matrix_0, (255 * 100 / 100), LV_PART_ITEMS);
        lv_obj_set_style_radius(matrix_0, 6, LV_PART_ITEMS);
        lv_obj_set_style_border_width(matrix_0, 1, LV_PART_ITEMS);
        lv_obj_set_style_border_color(matrix_0, lv_color_hex(0x1e293b), LV_PART_ITEMS);
        lv_obj_set_style_text_font(matrix_0, data_font, LV_PART_ITEMS);
        lv_obj_set_style_text_color(matrix_0, lv_color_hex(0xcbd5e1), LV_PART_ITEMS);
        lv_obj_set_style_bg_opa(matrix_0, (255 * 70 / 100), LV_PART_ITEMS | LV_STATE_PRESSED);
        lv_obj_set_style_shadow_width(matrix_0, 0, LV_PART_ITEMS);

        lv_obj_t * matrix_1 = lv_buttonmatrix_create(lv_obj_0);
        lv_obj_set_name(matrix_1, "matrix_1");
        lv_obj_set_x(matrix_1, 4);
        lv_obj_set_y(matrix_1, 153);
        lv_obj_set_width(matrix_1, 472);
        lv_obj_set_height(matrix_1, 38);
        static const char *matrix_1_map_0[] = {"q", "w", "e", "r", "t", "y", "u", "i", "o", "p", NULL};
        lv_buttonmatrix_set_map(matrix_1, matrix_1_map_0);
        lv_obj_set_scrollable(matrix_1, false);
        lv_obj_set_style_bg_opa(matrix_1, (255 * 0 / 100), 0);
        lv_obj_set_style_border_width(matrix_1, 0, 0);
        lv_obj_set_style_pad_all(matrix_1, 0, 0);
        lv_obj_set_style_pad_gap(matrix_1, 3, 0);
        lv_obj_set_style_radius(matrix_1, 0, 0);
        lv_obj_set_style_outline_width(matrix_1, 0, 0);
        lv_obj_set_style_shadow_width(matrix_1, 0, 0);
        lv_obj_set_style_bg_color(matrix_1, lv_color_hex(0x172030), LV_PART_ITEMS);
        lv_obj_set_style_bg_opa(matrix_1, (255 * 100 / 100), LV_PART_ITEMS);
        lv_obj_set_style_radius(matrix_1, 6, LV_PART_ITEMS);
        lv_obj_set_style_border_width(matrix_1, 1, LV_PART_ITEMS);
        lv_obj_set_style_border_color(matrix_1, lv_color_hex(0x1e293b), LV_PART_ITEMS);
        lv_obj_set_style_text_font(matrix_1, data_font, LV_PART_ITEMS);
        lv_obj_set_style_text_color(matrix_1, lv_color_hex(0xcbd5e1), LV_PART_ITEMS);
        lv_obj_set_style_bg_opa(matrix_1, (255 * 70 / 100), LV_PART_ITEMS | LV_STATE_PRESSED);
        lv_obj_set_style_shadow_width(matrix_1, 0, LV_PART_ITEMS);

        lv_obj_t * matrix_2 = lv_buttonmatrix_create(lv_obj_0);
        lv_obj_set_name(matrix_2, "matrix_2");
        lv_obj_set_x(matrix_2, 4);
        lv_obj_set_y(matrix_2, 194);
        lv_obj_set_width(matrix_2, 472);
        lv_obj_set_height(matrix_2, 38);
        static const char *matrix_2_map_0[] = {"a", "s", "d", "f", "g", "h", "j", "k", "l", NULL};
        lv_buttonmatrix_set_map(matrix_2, matrix_2_map_0);
        lv_obj_set_scrollable(matrix_2, false);
        lv_obj_set_style_bg_opa(matrix_2, (255 * 0 / 100), 0);
        lv_obj_set_style_border_width(matrix_2, 0, 0);
        lv_obj_set_style_pad_all(matrix_2, 0, 0);
        lv_obj_set_style_pad_gap(matrix_2, 3, 0);
        lv_obj_set_style_radius(matrix_2, 0, 0);
        lv_obj_set_style_outline_width(matrix_2, 0, 0);
        lv_obj_set_style_shadow_width(matrix_2, 0, 0);
        lv_obj_set_style_bg_color(matrix_2, lv_color_hex(0x172030), LV_PART_ITEMS);
        lv_obj_set_style_bg_opa(matrix_2, (255 * 100 / 100), LV_PART_ITEMS);
        lv_obj_set_style_radius(matrix_2, 6, LV_PART_ITEMS);
        lv_obj_set_style_border_width(matrix_2, 1, LV_PART_ITEMS);
        lv_obj_set_style_border_color(matrix_2, lv_color_hex(0x1e293b), LV_PART_ITEMS);
        lv_obj_set_style_text_font(matrix_2, data_font, LV_PART_ITEMS);
        lv_obj_set_style_text_color(matrix_2, lv_color_hex(0xcbd5e1), LV_PART_ITEMS);
        lv_obj_set_style_bg_opa(matrix_2, (255 * 70 / 100), LV_PART_ITEMS | LV_STATE_PRESSED);
        lv_obj_set_style_shadow_width(matrix_2, 0, LV_PART_ITEMS);

        lv_obj_t * matrix_3 = lv_buttonmatrix_create(lv_obj_0);
        lv_obj_set_name(matrix_3, "matrix_3");
        lv_obj_set_x(matrix_3, 74);
        lv_obj_set_y(matrix_3, 235);
        lv_obj_set_width(matrix_3, symbols ? 44 : 331);
        lv_obj_set_height(matrix_3, 38);
        static const char *matrix_3_map_0[] = {"z", "x", "c", "v", "b", "n", "m", NULL};
        lv_buttonmatrix_set_map(matrix_3, matrix_3_map_0);
        lv_obj_set_scrollable(matrix_3, false);
        lv_obj_set_style_bg_opa(matrix_3, (255 * 0 / 100), 0);
        lv_obj_set_style_border_width(matrix_3, 0, 0);
        lv_obj_set_style_pad_all(matrix_3, 0, 0);
        lv_obj_set_style_pad_gap(matrix_3, 3, 0);
        lv_obj_set_style_radius(matrix_3, 0, 0);
        lv_obj_set_style_outline_width(matrix_3, 0, 0);
        lv_obj_set_style_shadow_width(matrix_3, 0, 0);
        lv_obj_set_style_bg_color(matrix_3, lv_color_hex(0x172030), LV_PART_ITEMS);
        lv_obj_set_style_bg_opa(matrix_3, (255 * 100 / 100), LV_PART_ITEMS);
        lv_obj_set_style_radius(matrix_3, 6, LV_PART_ITEMS);
        lv_obj_set_style_border_width(matrix_3, 1, LV_PART_ITEMS);
        lv_obj_set_style_border_color(matrix_3, lv_color_hex(0x1e293b), LV_PART_ITEMS);
        lv_obj_set_style_text_font(matrix_3, data_font, LV_PART_ITEMS);
        lv_obj_set_style_text_color(matrix_3, lv_color_hex(0xcbd5e1), LV_PART_ITEMS);
        lv_obj_set_style_bg_opa(matrix_3, (255 * 70 / 100), LV_PART_ITEMS | LV_STATE_PRESSED);
        lv_obj_set_style_shadow_width(matrix_3, 0, LV_PART_ITEMS);

        lv_obj_t * key_shift = lv_button_create(lv_obj_0);
        lv_obj_set_x(key_shift, 4);
        lv_obj_set_y(key_shift, 235);
        lv_obj_set_width(key_shift, 67);
        lv_obj_set_height(key_shift, 38);
        lv_obj_set_clickable(key_shift, true);
        lv_obj_set_style_radius(key_shift, 6, 0);
        lv_obj_set_style_border_width(key_shift, 1, 0);
        lv_obj_set_style_bg_opa(key_shift, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(key_shift, shifted ? lv_color_hex(0x0c2538) : lv_color_hex(0x172030), 0);
        lv_obj_set_style_border_color(key_shift, shifted ? lv_color_hex(0x38bdf8) : lv_color_hex(0x1e293b), 0);
        lv_obj_set_style_pad_top(key_shift, -1, 0);
        lv_obj_set_style_pad_bottom(key_shift, -1, 0);
        lv_obj_set_style_pad_left(key_shift, -1, 0);
        lv_obj_set_style_pad_right(key_shift, -1, 0);
        lv_obj_set_scrollable(key_shift, false);
        lv_obj_set_style_shadow_width(key_shift, 0, 0);
        lv_obj_set_style_outline_width(key_shift, 0, 0);
        lv_obj_set_name(key_shift, "key_shift");
        lv_obj_set_style_bg_opa(key_shift, (255 * 70 / 100), LV_STATE_PRESSED);
        lv_obj_t * shift_image = lv_image_create(key_shift);
        lv_obj_set_x(shift_image, 26);
        lv_obj_set_y(shift_image, 12);
        lv_obj_set_width(shift_image, 14);
        lv_obj_set_height(shift_image, 14);
        lv_obj_set_clickable(shift_image, false);
        lv_obj_set_scrollable(shift_image, false);
        lv_image_set_src(shift_image, shift_icon);
        lv_obj_set_style_image_recolor(shift_image, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_style_image_recolor_opa(shift_image, (255 * 100 / 100), 0);
        lv_obj_set_name(shift_image, "shift_image");

        lv_obj_t * key_erase = lv_button_create(lv_obj_0);
        lv_obj_set_x(key_erase, 409);
        lv_obj_set_y(key_erase, 235);
        lv_obj_set_width(key_erase, 67);
        lv_obj_set_height(key_erase, 38);
        lv_obj_set_clickable(key_erase, true);
        lv_obj_set_style_radius(key_erase, 6, 0);
        lv_obj_set_style_border_width(key_erase, 1, 0);
        lv_obj_set_style_bg_opa(key_erase, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(key_erase, lv_color_hex(0x172030), 0);
        lv_obj_set_style_border_color(key_erase, lv_color_hex(0x1e293b), 0);
        lv_obj_set_style_pad_top(key_erase, -1, 0);
        lv_obj_set_style_pad_bottom(key_erase, -1, 0);
        lv_obj_set_style_pad_left(key_erase, -1, 0);
        lv_obj_set_style_pad_right(key_erase, -1, 0);
        lv_obj_set_scrollable(key_erase, false);
        lv_obj_set_style_shadow_width(key_erase, 0, 0);
        lv_obj_set_style_outline_width(key_erase, 0, 0);
        lv_obj_set_name(key_erase, "key_erase");
        lv_obj_set_style_bg_opa(key_erase, (255 * 70 / 100), LV_STATE_PRESSED);
        lv_obj_t * lv_image_0 = lv_image_create(key_erase);
        lv_obj_set_x(lv_image_0, 25);
        lv_obj_set_y(lv_image_0, 11);
        lv_obj_set_width(lv_image_0, 16);
        lv_obj_set_height(lv_image_0, 16);
        lv_obj_set_clickable(lv_image_0, false);
        lv_obj_set_scrollable(lv_image_0, false);
        lv_image_set_src(lv_image_0, erase_icon);
        lv_obj_set_style_image_recolor(lv_image_0, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_style_image_recolor_opa(lv_image_0, (255 * 100 / 100), 0);

        lv_obj_t * key_symbols = lv_button_create(lv_obj_0);
        lv_obj_set_x(key_symbols, 4);
        lv_obj_set_y(key_symbols, 276);
        lv_obj_set_width(key_symbols, 69);
        lv_obj_set_height(key_symbols, 38);
        lv_obj_set_clickable(key_symbols, true);
        lv_obj_set_style_radius(key_symbols, 6, 0);
        lv_obj_set_style_border_width(key_symbols, 1, 0);
        lv_obj_set_style_bg_opa(key_symbols, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(key_symbols, lv_color_hex(0x172030), 0);
        lv_obj_set_style_border_color(key_symbols, lv_color_hex(0x1e293b), 0);
        lv_obj_set_style_pad_top(key_symbols, -1, 0);
        lv_obj_set_style_pad_bottom(key_symbols, -1, 0);
        lv_obj_set_style_pad_left(key_symbols, -1, 0);
        lv_obj_set_style_pad_right(key_symbols, -1, 0);
        lv_obj_set_scrollable(key_symbols, false);
        lv_obj_set_style_shadow_width(key_symbols, 0, 0);
        lv_obj_set_style_outline_width(key_symbols, 0, 0);
        lv_obj_set_name(key_symbols, "key_symbols");
        lv_obj_set_style_bg_opa(key_symbols, (255 * 70 / 100), LV_STATE_PRESSED);
        lv_obj_t * lv_label_1 = lv_label_create(key_symbols);
        lv_obj_set_x(lv_label_1, 0);
        lv_obj_set_y(lv_label_1, 12);
        lv_obj_set_width(lv_label_1, 69);
        lv_obj_set_height(lv_label_1, 13);
        lv_obj_set_clickable(lv_label_1, false);
        lv_obj_set_style_text_font(lv_label_1, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_1, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_scrollable(lv_label_1, false);
        lv_label_set_text(lv_label_1, toggle_text);
        lv_label_set_long_mode(lv_label_1, LV_LABEL_LONG_MODE_WRAP);
        lv_obj_set_style_text_align(lv_label_1, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * key_comma = lv_button_create(lv_obj_0);
        lv_obj_set_x(key_comma, 76);
        lv_obj_set_y(key_comma, 276);
        lv_obj_set_width(key_comma, 43);
        lv_obj_set_height(key_comma, 38);
        lv_obj_set_clickable(key_comma, true);
        lv_obj_set_style_radius(key_comma, 6, 0);
        lv_obj_set_style_border_width(key_comma, 1, 0);
        lv_obj_set_style_bg_opa(key_comma, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(key_comma, lv_color_hex(0x172030), 0);
        lv_obj_set_style_border_color(key_comma, lv_color_hex(0x1e293b), 0);
        lv_obj_set_style_pad_top(key_comma, -1, 0);
        lv_obj_set_style_pad_bottom(key_comma, -1, 0);
        lv_obj_set_style_pad_left(key_comma, -1, 0);
        lv_obj_set_style_pad_right(key_comma, -1, 0);
        lv_obj_set_scrollable(key_comma, false);
        lv_obj_set_style_shadow_width(key_comma, 0, 0);
        lv_obj_set_style_outline_width(key_comma, 0, 0);
        lv_obj_set_name(key_comma, "key_comma");
        lv_obj_set_style_bg_opa(key_comma, (255 * 70 / 100), LV_STATE_PRESSED);
        lv_obj_t * lv_label_2 = lv_label_create(key_comma);
        lv_obj_set_x(lv_label_2, 0);
        lv_obj_set_y(lv_label_2, 12);
        lv_obj_set_width(lv_label_2, 43);
        lv_obj_set_height(lv_label_2, 13);
        lv_obj_set_clickable(lv_label_2, false);
        lv_obj_set_style_text_font(lv_label_2, data_font, 0);
        lv_obj_set_style_text_color(lv_label_2, lv_color_hex(0xcbd5e1), 0);
        lv_obj_set_scrollable(lv_label_2, false);
        lv_label_set_text(lv_label_2, ",");
        lv_label_set_long_mode(lv_label_2, LV_LABEL_LONG_MODE_WRAP);
        lv_obj_set_style_text_align(lv_label_2, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * key_space = lv_button_create(lv_obj_0);
        lv_obj_set_x(key_space, 122);
        lv_obj_set_y(key_space, 276);
        lv_obj_set_width(key_space, 200);
        lv_obj_set_height(key_space, 38);
        lv_obj_set_clickable(key_space, true);
        lv_obj_set_style_radius(key_space, 6, 0);
        lv_obj_set_style_border_width(key_space, 1, 0);
        lv_obj_set_style_bg_opa(key_space, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(key_space, lv_color_hex(0x172030), 0);
        lv_obj_set_style_border_color(key_space, lv_color_hex(0x1e293b), 0);
        lv_obj_set_style_pad_top(key_space, -1, 0);
        lv_obj_set_style_pad_bottom(key_space, -1, 0);
        lv_obj_set_style_pad_left(key_space, -1, 0);
        lv_obj_set_style_pad_right(key_space, -1, 0);
        lv_obj_set_scrollable(key_space, false);
        lv_obj_set_style_shadow_width(key_space, 0, 0);
        lv_obj_set_style_outline_width(key_space, 0, 0);
        lv_obj_set_name(key_space, "key_space");
        lv_obj_set_style_bg_opa(key_space, (255 * 70 / 100), LV_STATE_PRESSED);
        lv_obj_t * lv_label_3 = lv_label_create(key_space);
        lv_obj_set_x(lv_label_3, 0);
        lv_obj_set_y(lv_label_3, 12);
        lv_obj_set_width(lv_label_3, 200);
        lv_obj_set_height(lv_label_3, 13);
        lv_obj_set_clickable(lv_label_3, false);
        lv_obj_set_style_text_font(lv_label_3, caption_font, 0);
        lv_obj_set_style_text_color(lv_label_3, lv_color_hex(0x64748b), 0);
        lv_obj_set_scrollable(lv_label_3, false);
        lv_label_set_text(lv_label_3, "space");
        lv_label_set_long_mode(lv_label_3, LV_LABEL_LONG_MODE_WRAP);
        lv_obj_set_style_text_align(lv_label_3, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * key_period = lv_button_create(lv_obj_0);
        lv_obj_set_x(key_period, 325);
        lv_obj_set_y(key_period, 276);
        lv_obj_set_width(key_period, 43);
        lv_obj_set_height(key_period, 38);
        lv_obj_set_clickable(key_period, true);
        lv_obj_set_style_radius(key_period, 6, 0);
        lv_obj_set_style_border_width(key_period, 1, 0);
        lv_obj_set_style_bg_opa(key_period, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(key_period, lv_color_hex(0x172030), 0);
        lv_obj_set_style_border_color(key_period, lv_color_hex(0x1e293b), 0);
        lv_obj_set_style_pad_top(key_period, -1, 0);
        lv_obj_set_style_pad_bottom(key_period, -1, 0);
        lv_obj_set_style_pad_left(key_period, -1, 0);
        lv_obj_set_style_pad_right(key_period, -1, 0);
        lv_obj_set_scrollable(key_period, false);
        lv_obj_set_style_shadow_width(key_period, 0, 0);
        lv_obj_set_style_outline_width(key_period, 0, 0);
        lv_obj_set_name(key_period, "key_period");
        lv_obj_set_style_bg_opa(key_period, (255 * 70 / 100), LV_STATE_PRESSED);
        lv_obj_t * lv_label_4 = lv_label_create(key_period);
        lv_obj_set_x(lv_label_4, 0);
        lv_obj_set_y(lv_label_4, 12);
        lv_obj_set_width(lv_label_4, 43);
        lv_obj_set_height(lv_label_4, 13);
        lv_obj_set_clickable(lv_label_4, false);
        lv_obj_set_style_text_font(lv_label_4, data_font, 0);
        lv_obj_set_style_text_color(lv_label_4, lv_color_hex(0xcbd5e1), 0);
        lv_obj_set_scrollable(lv_label_4, false);
        lv_label_set_text(lv_label_4, ".");
        lv_label_set_long_mode(lv_label_4, LV_LABEL_LONG_MODE_WRAP);
        lv_obj_set_style_text_align(lv_label_4, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * key_go = lv_button_create(lv_obj_0);
        lv_obj_set_x(key_go, 371);
        lv_obj_set_y(key_go, 276);
        lv_obj_set_width(key_go, 105);
        lv_obj_set_height(key_go, 38);
        lv_obj_set_clickable(key_go, true);
        lv_obj_set_style_radius(key_go, 6, 0);
        lv_obj_set_style_border_width(key_go, 1, 0);
        lv_obj_set_style_bg_opa(key_go, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(key_go, lv_color_hex(0x172030), 0);
        lv_obj_set_style_border_color(key_go, lv_color_hex(0x1e293b), 0);
        lv_obj_set_style_pad_top(key_go, -1, 0);
        lv_obj_set_style_pad_bottom(key_go, -1, 0);
        lv_obj_set_style_pad_left(key_go, -1, 0);
        lv_obj_set_style_pad_right(key_go, -1, 0);
        lv_obj_set_scrollable(key_go, false);
        lv_obj_set_style_shadow_width(key_go, 0, 0);
        lv_obj_set_style_outline_width(key_go, 0, 0);
        lv_obj_set_name(key_go, "key_go");
        lv_obj_set_style_bg_opa(key_go, (255 * 70 / 100), LV_STATE_PRESSED);
        lv_obj_t * key_go_label = lv_label_create(key_go);
        lv_obj_set_x(key_go_label, 0);
        lv_obj_set_y(key_go_label, 12);
        lv_obj_set_width(key_go_label, 105);
        lv_obj_set_height(key_go_label, 13);
        lv_obj_set_clickable(key_go_label, false);
        lv_obj_set_style_text_font(key_go_label, data_font, 0);
        lv_obj_set_style_text_color(key_go_label, lv_color_hex(0x64748b), 0);
        lv_obj_set_scrollable(key_go_label, false);
        lv_label_set_text(key_go_label, "Connect");
        lv_label_set_long_mode(key_go_label, LV_LABEL_LONG_MODE_WRAP);
        lv_obj_set_style_text_align(key_go_label, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_name(key_go_label, "key_go_label");

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

lv_obj_t * rcc_wifi_pass_get_password_field(lv_obj_t *parent)
{
    return lv_obj_find_by_name(parent, "password_field");
}

lv_obj_t * rcc_wifi_pass_get_password_eye_icon(lv_obj_t *parent)
{
    return lv_obj_find_by_name(parent, "password_eye_icon");
}

lv_obj_t * rcc_wifi_pass_get_key_symbols(lv_obj_t *parent)
{
    return lv_obj_find_by_name(parent, "key_symbols");
}

lv_obj_t * rcc_wifi_pass_get_key_go(lv_obj_t *parent)
{
    return lv_obj_find_by_name(parent, "key_go");
}

lv_obj_t * rcc_wifi_pass_get_key_go_label(lv_obj_t *parent)
{
    return lv_obj_find_by_name(parent, "key_go_label");
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

