/**
 * @file rcc_session_detail_gen.h
 */

#ifndef LVGL_PRO_RCC_SESSION_DETAIL_GEN_H
#define LVGL_PRO_RCC_SESSION_DETAIL_GEN_H

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

lv_obj_t * rcc_session_detail_create(lv_obj_t * parent, int32_t detail_x, int32_t detail_y, int32_t detail_width, const char * text, lv_font_t * small);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_RCC_SESSION_DETAIL_GEN_H*/