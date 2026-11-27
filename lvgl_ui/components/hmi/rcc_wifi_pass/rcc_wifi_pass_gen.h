/**
 * @file rcc_wifi_pass_gen.h
 */

#ifndef LVGL_PRO_RCC_WIFI_PASS_GEN_H
#define LVGL_PRO_RCC_WIFI_PASS_GEN_H

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

lv_obj_t * rcc_wifi_pass_create(lv_obj_t * parent, bool symbols, bool shifted, const char * count_text, const char * toggle_text, lv_font_t * caption_font, lv_font_t * data_font, const void * eye_icon, const void * shift_icon, const void * erase_icon);
lv_obj_t * rcc_wifi_pass_get_password_field(lv_obj_t *parent);
lv_obj_t * rcc_wifi_pass_get_password_eye_icon(lv_obj_t *parent);
lv_obj_t * rcc_wifi_pass_get_key_symbols(lv_obj_t *parent);
lv_obj_t * rcc_wifi_pass_get_key_go(lv_obj_t *parent);
lv_obj_t * rcc_wifi_pass_get_key_go_label(lv_obj_t *parent);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_RCC_WIFI_PASS_GEN_H*/