/**
 * @file rcc_overview_step_gen.h
 */

#ifndef LVGL_PRO_RCC_OVERVIEW_STEP_GEN_H
#define LVGL_PRO_RCC_OVERVIEW_STEP_GEN_H

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

lv_obj_t * rcc_overview_step_create(lv_obj_t * parent, int32_t step_x, int32_t step_width, int32_t offset, const char * text, lv_color_t text_color, lv_color_t dot_color, int32_t active_opa, lv_font_t * small);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_RCC_OVERVIEW_STEP_GEN_H*/