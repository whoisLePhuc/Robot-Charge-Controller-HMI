/**
 * @file rcc_session_summary_gen.h
 */

#ifndef LVGL_PRO_RCC_SESSION_SUMMARY_GEN_H
#define LVGL_PRO_RCC_SESSION_SUMMARY_GEN_H

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

lv_obj_t * rcc_session_summary_create(lv_obj_t * parent, int32_t card_x, const char * title, const char * value, lv_font_t * small, lv_font_t * bold);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_RCC_SESSION_SUMMARY_GEN_H*/