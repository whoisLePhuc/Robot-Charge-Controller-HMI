/**
 * @file rcc_measure_matrix_gen.h
 */

#ifndef LVGL_PRO_RCC_MEASURE_MATRIX_GEN_H
#define LVGL_PRO_RCC_MEASURE_MATRIX_GEN_H

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

lv_obj_t * rcc_measure_matrix_create(lv_obj_t * parent, int32_t matrix_height, lv_font_t * small, int32_t seq_x);
lv_obj_t * rcc_measure_matrix_get_matrix_cells(lv_obj_t *parent);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_RCC_MEASURE_MATRIX_GEN_H*/