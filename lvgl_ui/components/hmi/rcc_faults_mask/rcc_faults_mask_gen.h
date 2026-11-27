/**
 * @file rcc_faults_mask_gen.h
 */

#ifndef LVGL_PRO_RCC_FAULTS_MASK_GEN_H
#define LVGL_PRO_RCC_FAULTS_MASK_GEN_H

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

lv_obj_t * rcc_faults_mask_create(lv_obj_t * parent, int32_t card_x, int32_t card_y, int32_t card_height, const char * list_title, lv_font_t * caption_font, lv_font_t * body_font);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_RCC_FAULTS_MASK_GEN_H*/