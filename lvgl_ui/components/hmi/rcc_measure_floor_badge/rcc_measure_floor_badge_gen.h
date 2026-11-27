/**
 * @file rcc_measure_floor_badge_gen.h
 */

#ifndef LVGL_PRO_RCC_MEASURE_FLOOR_BADGE_GEN_H
#define LVGL_PRO_RCC_MEASURE_FLOOR_BADGE_GEN_H

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

lv_obj_t * rcc_measure_floor_badge_create(lv_obj_t * parent, int32_t badge_x, int32_t badge_y, int32_t badge_width, lv_font_t * small);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_RCC_MEASURE_FLOOR_BADGE_GEN_H*/