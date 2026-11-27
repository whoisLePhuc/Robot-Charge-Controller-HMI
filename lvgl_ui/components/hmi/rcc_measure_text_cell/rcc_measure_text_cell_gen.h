/**
 * @file rcc_measure_text_cell_gen.h
 */

#ifndef LVGL_PRO_RCC_MEASURE_TEXT_CELL_GEN_H
#define LVGL_PRO_RCC_MEASURE_TEXT_CELL_GEN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
#else
    #include "lvgl/lvgl.h"
#endif

#if defined(LV_USE_XML) && LV_USE_XML
    #include "lv_xml/lv_xml.h"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_obj_t * rcc_measure_text_cell_create(lv_obj_t * parent, int32_t cell_x, int32_t cell_y, int32_t cell_width, const char * text, lv_font_t * font, lv_color_t text_color);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_RCC_MEASURE_TEXT_CELL_GEN_H*/