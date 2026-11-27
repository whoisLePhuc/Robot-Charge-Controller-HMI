/**
 * @file rcc_session_detail_icon_gen.h
 */

#ifndef LVGL_PRO_RCC_SESSION_DETAIL_ICON_GEN_H
#define LVGL_PRO_RCC_SESSION_DETAIL_ICON_GEN_H

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

lv_obj_t * rcc_session_detail_icon_create(lv_obj_t * parent, int32_t detail_x, int32_t detail_y, const void * mark);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_RCC_SESSION_DETAIL_ICON_GEN_H*/