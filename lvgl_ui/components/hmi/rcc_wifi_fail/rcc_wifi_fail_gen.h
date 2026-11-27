/**
 * @file rcc_wifi_fail_gen.h
 */

#ifndef LVGL_PRO_RCC_WIFI_FAIL_GEN_H
#define LVGL_PRO_RCC_WIFI_FAIL_GEN_H

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

lv_obj_t * rcc_wifi_fail_create(lv_obj_t * parent, const char * failure_title, const char * failure_text, const char * attempt_text, const char * signal_text, bool show_password_hint, lv_font_t * caption_font, lv_font_t * data_font, lv_font_t * title_font, const void * result_icon);
lv_obj_t * rcc_wifi_fail_get_choose_other(lv_obj_t *parent);
lv_obj_t * rcc_wifi_fail_get_retry(lv_obj_t *parent);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_RCC_WIFI_FAIL_GEN_H*/