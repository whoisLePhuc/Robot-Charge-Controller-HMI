/**
 * @file rcc_overview_connector_gen.h
 */

#ifndef LVGL_PRO_RCC_OVERVIEW_CONNECTOR_GEN_H
#define LVGL_PRO_RCC_OVERVIEW_CONNECTOR_GEN_H

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

lv_obj_t * rcc_overview_connector_create(lv_obj_t * parent, int32_t line_x, int32_t line_width, lv_color_t line_color);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_RCC_OVERVIEW_CONNECTOR_GEN_H*/