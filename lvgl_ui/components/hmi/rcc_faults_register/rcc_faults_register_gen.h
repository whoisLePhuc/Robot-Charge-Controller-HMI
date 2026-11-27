/**
 * @file rcc_faults_register_gen.h
 */

#ifndef LVGL_PRO_RCC_FAULTS_REGISTER_GEN_H
#define LVGL_PRO_RCC_FAULTS_REGISTER_GEN_H

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

lv_obj_t * rcc_faults_register_create(lv_obj_t * parent, int32_t card_x, int32_t card_height, const char * register_title, const char * register_mask, const char * register_value, const char * register_note, lv_color_t value_color, lv_font_t * caption_font, lv_font_t * title_font);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_RCC_FAULTS_REGISTER_GEN_H*/