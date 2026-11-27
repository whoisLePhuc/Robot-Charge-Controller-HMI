/**
 * @file rcc_wifi_row_gen.h
 */

#ifndef LVGL_PRO_RCC_WIFI_ROW_GEN_H
#define LVGL_PRO_RCC_WIFI_ROW_GEN_H

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

lv_obj_t * rcc_wifi_row_create(lv_obj_t * parent, int32_t row_index, const char * ssid, const char * details, const char * signal, bool secured, int32_t title_width, lv_color_t background, lv_color_t border, lv_color_t details_color, lv_color_t bar_short, lv_color_t bar_low, lv_color_t bar_high, lv_color_t bar_full, lv_font_t * caption_font, lv_font_t * data_font, const void * lock_icon);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_RCC_WIFI_ROW_GEN_H*/