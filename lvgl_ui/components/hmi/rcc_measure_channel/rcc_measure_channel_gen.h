/**
 * @file rcc_measure_channel_gen.h
 */

#ifndef LVGL_PRO_RCC_MEASURE_CHANNEL_GEN_H
#define LVGL_PRO_RCC_MEASURE_CHANNEL_GEN_H

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

lv_obj_t * rcc_measure_channel_create(lv_obj_t * parent, int32_t card_x, const char * title, const char * limit, const char * unit, lv_color_t hue, int32_t limit_x, const char * reading, const char * filtered, int32_t unit_x, lv_font_t * small, lv_font_t * reading_font, lv_font_t * unit_font);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_RCC_MEASURE_CHANNEL_GEN_H*/