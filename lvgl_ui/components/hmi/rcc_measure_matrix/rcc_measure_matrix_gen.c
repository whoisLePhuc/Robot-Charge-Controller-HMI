/**
 * @file rcc_measure_matrix_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "rcc_measure_matrix_gen.h"
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

lv_obj_t * rcc_measure_matrix_create(lv_obj_t * parent, int32_t matrix_height, lv_font_t * small, int32_t seq_x)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "rcc_measure_matrix_#");
        lv_obj_set_scrollable(lv_obj_0, false);
        lv_obj_set_clickable(lv_obj_0, false);
        lv_obj_set_x(lv_obj_0, 0);
        lv_obj_set_y(lv_obj_0, 82);
        lv_obj_set_width(lv_obj_0, 408);
        lv_obj_set_height(lv_obj_0, matrix_height);
        lv_obj_set_style_bg_color(lv_obj_0, lv_color_hex(0x111722), 0);
        lv_obj_set_style_bg_opa(lv_obj_0, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(lv_obj_0, 10, 0);
        lv_obj_set_style_border_width(lv_obj_0, 1, 0);
        lv_obj_set_style_border_color(lv_obj_0, lv_color_hex(0x1e293b), 0);
        lv_obj_set_style_border_opa(lv_obj_0, (255 * 100 / 100), 0);
        lv_obj_set_style_outline_width(lv_obj_0, 0, 0);
        lv_obj_set_style_shadow_width(lv_obj_0, 0, 0);
        lv_obj_set_style_pad_all(lv_obj_0, -1, 0);

        lv_obj_t * matrix_title = lv_label_create(lv_obj_0);
        lv_obj_set_x(matrix_title, 13);
        lv_obj_set_y(matrix_title, 8);
        lv_label_set_text(matrix_title, "ADC QUALITY MATRIX");
        lv_obj_set_scrollable(matrix_title, false);
        lv_obj_set_clickable(matrix_title, false);
        lv_obj_set_style_text_font(matrix_title, small, 0);
        lv_obj_set_style_text_color(matrix_title, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_style_text_letter_space(matrix_title, 1, 0);
        lv_obj_set_name(matrix_title, "matrix_title");

        lv_obj_t * matrix_seq = lv_label_create(lv_obj_0);
        lv_obj_set_x(matrix_seq, seq_x);
        lv_obj_set_y(matrix_seq, 8);
        lv_label_set_text(matrix_seq, "SEQ 48213 · AGE 12 ms");
        lv_obj_set_scrollable(matrix_seq, false);
        lv_obj_set_clickable(matrix_seq, false);
        lv_obj_set_style_text_font(matrix_seq, small, 0);
        lv_obj_set_style_text_color(matrix_seq, lv_color_hex(0xcbd5e1), 0);
        lv_obj_set_width(matrix_seq, 395 - seq_x);
        lv_obj_set_height(matrix_seq, 13);
        lv_label_set_long_mode(matrix_seq, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(matrix_seq, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_set_name(matrix_seq, "matrix_seq");

        lv_obj_t * matrix_header_0 = lv_label_create(lv_obj_0);
        lv_obj_set_x(matrix_header_0, 59);
        lv_obj_set_y(matrix_header_0, matrix_height - 67);
        lv_label_set_text(matrix_header_0, "PRES");
        lv_obj_set_scrollable(matrix_header_0, false);
        lv_obj_set_clickable(matrix_header_0, false);
        lv_obj_set_style_text_font(matrix_header_0, small, 0);
        lv_obj_set_style_text_color(matrix_header_0, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_width(matrix_header_0, 47);
        lv_obj_set_height(matrix_header_0, 13);
        lv_label_set_long_mode(matrix_header_0, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(matrix_header_0, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_name(matrix_header_0, "matrix_header_0");

        lv_obj_t * matrix_header_1 = lv_label_create(lv_obj_0);
        lv_obj_set_x(matrix_header_1, 106);
        lv_obj_set_y(matrix_header_1, matrix_height - 67);
        lv_label_set_text(matrix_header_1, "FRESH");
        lv_obj_set_scrollable(matrix_header_1, false);
        lv_obj_set_clickable(matrix_header_1, false);
        lv_obj_set_style_text_font(matrix_header_1, small, 0);
        lv_obj_set_style_text_color(matrix_header_1, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_width(matrix_header_1, 47);
        lv_obj_set_height(matrix_header_1, 13);
        lv_label_set_long_mode(matrix_header_1, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(matrix_header_1, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_name(matrix_header_1, "matrix_header_1");

        lv_obj_t * matrix_header_2 = lv_label_create(lv_obj_0);
        lv_obj_set_x(matrix_header_2, 154);
        lv_obj_set_y(matrix_header_2, matrix_height - 67);
        lv_label_set_text(matrix_header_2, "CAL");
        lv_obj_set_scrollable(matrix_header_2, false);
        lv_obj_set_clickable(matrix_header_2, false);
        lv_obj_set_style_text_font(matrix_header_2, small, 0);
        lv_obj_set_style_text_color(matrix_header_2, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_width(matrix_header_2, 47);
        lv_obj_set_height(matrix_header_2, 13);
        lv_label_set_long_mode(matrix_header_2, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(matrix_header_2, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_name(matrix_header_2, "matrix_header_2");

        lv_obj_t * matrix_header_3 = lv_label_create(lv_obj_0);
        lv_obj_set_x(matrix_header_3, 201);
        lv_obj_set_y(matrix_header_3, matrix_height - 67);
        lv_label_set_text(matrix_header_3, "RANGE");
        lv_obj_set_scrollable(matrix_header_3, false);
        lv_obj_set_clickable(matrix_header_3, false);
        lv_obj_set_style_text_font(matrix_header_3, small, 0);
        lv_obj_set_style_text_color(matrix_header_3, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_width(matrix_header_3, 47);
        lv_obj_set_height(matrix_header_3, 13);
        lv_label_set_long_mode(matrix_header_3, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(matrix_header_3, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_name(matrix_header_3, "matrix_header_3");

        lv_obj_t * matrix_header_4 = lv_label_create(lv_obj_0);
        lv_obj_set_x(matrix_header_4, 249);
        lv_obj_set_y(matrix_header_4, matrix_height - 67);
        lv_label_set_text(matrix_header_4, "SAT");
        lv_obj_set_scrollable(matrix_header_4, false);
        lv_obj_set_clickable(matrix_header_4, false);
        lv_obj_set_style_text_font(matrix_header_4, small, 0);
        lv_obj_set_style_text_color(matrix_header_4, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_width(matrix_header_4, 47);
        lv_obj_set_height(matrix_header_4, 13);
        lv_label_set_long_mode(matrix_header_4, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(matrix_header_4, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_name(matrix_header_4, "matrix_header_4");

        lv_obj_t * matrix_header_5 = lv_label_create(lv_obj_0);
        lv_obj_set_x(matrix_header_5, 297);
        lv_obj_set_y(matrix_header_5, matrix_height - 67);
        lv_label_set_text(matrix_header_5, "PLAUS");
        lv_obj_set_scrollable(matrix_header_5, false);
        lv_obj_set_clickable(matrix_header_5, false);
        lv_obj_set_style_text_font(matrix_header_5, small, 0);
        lv_obj_set_style_text_color(matrix_header_5, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_width(matrix_header_5, 47);
        lv_obj_set_height(matrix_header_5, 13);
        lv_label_set_long_mode(matrix_header_5, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(matrix_header_5, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_name(matrix_header_5, "matrix_header_5");

        lv_obj_t * matrix_header_6 = lv_label_create(lv_obj_0);
        lv_obj_set_x(matrix_header_6, 347);
        lv_obj_set_y(matrix_header_6, matrix_height - 67);
        lv_label_set_text(matrix_header_6, "FLOOR");
        lv_obj_set_scrollable(matrix_header_6, false);
        lv_obj_set_clickable(matrix_header_6, false);
        lv_obj_set_style_text_font(matrix_header_6, small, 0);
        lv_obj_set_style_text_color(matrix_header_6, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_width(matrix_header_6, 47);
        lv_obj_set_height(matrix_header_6, 13);
        lv_label_set_long_mode(matrix_header_6, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_align(matrix_header_6, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_name(matrix_header_6, "matrix_header_6");

        lv_obj_t * lv_obj_1 = lv_obj_create(lv_obj_0);
        lv_obj_set_x(lv_obj_1, 344);
        lv_obj_set_y(lv_obj_1, matrix_height - 68);
        lv_obj_set_width(lv_obj_1, 1);
        lv_obj_set_height(lv_obj_1, 61);
        lv_obj_set_clickable(lv_obj_1, false);
        lv_obj_set_scrollable(lv_obj_1, false);
        lv_obj_set_style_bg_color(lv_obj_1, lv_color_hex(0x1e293b), 0);
        lv_obj_set_style_bg_opa(lv_obj_1, (255 * 100 / 100), 0);
        lv_obj_set_style_radius(lv_obj_1, 0, 0);
        lv_obj_set_style_border_width(lv_obj_1, 0, 0);
        lv_obj_set_style_outline_width(lv_obj_1, 0, 0);
        lv_obj_set_style_shadow_width(lv_obj_1, 0, 0);
        lv_obj_set_style_pad_all(lv_obj_1, 0, 0);

        lv_obj_t * matrix_cells = lv_obj_create(lv_obj_0);
        lv_obj_set_name(matrix_cells, "matrix_cells");
        lv_obj_set_x(matrix_cells, 0);
        lv_obj_set_y(matrix_cells, 0);
        lv_obj_set_width(matrix_cells, 408);
        lv_obj_set_height(matrix_cells, matrix_height);
        lv_obj_set_scrollable(matrix_cells, false);
        lv_obj_set_clickable(matrix_cells, false);
        lv_obj_set_style_bg_opa(matrix_cells, (255 * 0 / 100), 0);
        lv_obj_set_style_border_width(matrix_cells, 0, 0);
        lv_obj_set_style_outline_width(matrix_cells, 0, 0);
        lv_obj_set_style_shadow_width(matrix_cells, 0, 0);
        lv_obj_set_style_pad_all(matrix_cells, 0, 0);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

lv_obj_t * rcc_measure_matrix_get_matrix_cells(lv_obj_t *parent)
{
    return lv_obj_find_by_name(parent, "matrix_cells");
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

