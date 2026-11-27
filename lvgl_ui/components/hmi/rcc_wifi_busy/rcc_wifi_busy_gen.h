/**
 * @file rcc_wifi_busy_gen.h
 */

#ifndef LVGL_PRO_RCC_WIFI_BUSY_GEN_H
#define LVGL_PRO_RCC_WIFI_BUSY_GEN_H

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

lv_obj_t * rcc_wifi_busy_create(lv_obj_t * parent, lv_font_t * title_font, lv_font_t * caption_font, lv_font_t * data_font, lv_font_t * active_font, const void * spinner_large, const void * spinner_small, const void * check_icon);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_RCC_WIFI_BUSY_GEN_H*/