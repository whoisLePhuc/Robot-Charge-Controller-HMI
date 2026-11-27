/**
 * @file rcc_wifi_list_gen.h
 */

#ifndef LVGL_PRO_RCC_WIFI_LIST_GEN_H
#define LVGL_PRO_RCC_WIFI_LIST_GEN_H

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

lv_obj_t * rcc_wifi_list_create(lv_obj_t * parent, lv_font_t * caption_font, lv_font_t * detail_font, const void * refresh_icon);
lv_obj_t * rcc_wifi_list_get_networks(lv_obj_t *parent);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_RCC_WIFI_LIST_GEN_H*/