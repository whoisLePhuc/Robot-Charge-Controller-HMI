/**
 * @file rcc_telemetry_card_gen.h
 */

#ifndef LVGL_PRO_RCC_TELEMETRY_CARD_GEN_H
#define LVGL_PRO_RCC_TELEMETRY_CARD_GEN_H

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

lv_obj_t * rcc_telemetry_card_create(lv_obj_t * parent, int32_t card_x, int32_t card_y, int32_t card_height, const char * title, const char * unit, lv_color_t accent, const char * reading, const char * summary, int32_t fill_width, int32_t unit_x, lv_color_t value_color, bool hide_unit, bool hide_fill, lv_font_t * caption_font, lv_font_t * value_font, lv_font_t * unit_font, const void * card_icon);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_RCC_TELEMETRY_CARD_GEN_H*/