/**
 * @file rcc_measure_row_heading_gen.h
 */

#ifndef LVGL_PRO_RCC_MEASURE_ROW_HEADING_GEN_H
#define LVGL_PRO_RCC_MEASURE_ROW_HEADING_GEN_H

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

lv_obj_t * rcc_measure_row_heading_create(lv_obj_t * parent, int32_t row_y, const char * text, lv_font_t * bold, lv_color_t color);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_RCC_MEASURE_ROW_HEADING_GEN_H*/