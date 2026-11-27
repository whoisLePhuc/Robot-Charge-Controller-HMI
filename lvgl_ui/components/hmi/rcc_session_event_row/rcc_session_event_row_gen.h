/**
 * @file rcc_session_event_row_gen.h
 */

#ifndef LVGL_PRO_RCC_SESSION_EVENT_ROW_GEN_H
#define LVGL_PRO_RCC_SESSION_EVENT_ROW_GEN_H

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

lv_obj_t * rcc_session_event_row_create(lv_obj_t * parent, int32_t row_y, int32_t row_height, const char * since, const char * caption, lv_color_t event_color, lv_font_t * small, lv_font_t * bold, bool divider_visible);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_RCC_SESSION_EVENT_ROW_GEN_H*/