/**
 * @file rcc_wifi_info_gen.h
 */

#ifndef LVGL_PRO_RCC_WIFI_INFO_GEN_H
#define LVGL_PRO_RCC_WIFI_INFO_GEN_H

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

lv_obj_t * rcc_wifi_info_create(lv_obj_t * parent, const char * connected_ssid, const char * ip_address, const char * gateway, const char * signal_text, const char * mac_address, const char * ota_title, const char * ota_value, const char * slot_text, const char * ota_button_text, bool ota_armed, lv_font_t * caption_font, lv_font_t * data_font, lv_font_t * title_font, const void * result_icon);
lv_obj_t * rcc_wifi_info_get_done(lv_obj_t *parent);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_RCC_WIFI_INFO_GEN_H*/