/**
 * @file rcc_session_events_gen.h
 */

#ifndef LVGL_PRO_RCC_SESSION_EVENTS_GEN_H
#define LVGL_PRO_RCC_SESSION_EVENTS_GEN_H

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

lv_obj_t * rcc_session_events_create(lv_obj_t * parent, int32_t event_height, lv_font_t * small, lv_font_t * bold, lv_font_t * body_font, const void * sync_icon, const void * offline_icon, int32_t sync_icon_x, int32_t sync_text_x);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_RCC_SESSION_EVENTS_GEN_H*/