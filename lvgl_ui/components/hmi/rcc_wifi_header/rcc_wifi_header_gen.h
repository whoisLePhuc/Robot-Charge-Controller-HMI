/**
 * @file rcc_wifi_header_gen.h
 */

#ifndef LVGL_PRO_RCC_WIFI_HEADER_GEN_H
#define LVGL_PRO_RCC_WIFI_HEADER_GEN_H

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

lv_obj_t * rcc_wifi_header_create(lv_obj_t * parent, const char * heading, const char * subtitle, lv_font_t * title_font, lv_font_t * caption_font, const void * back_icon);
lv_obj_t * rcc_wifi_header_get_back(lv_obj_t *parent);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_RCC_WIFI_HEADER_GEN_H*/