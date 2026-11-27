/**
 * @file rcc_overview_hero_gen.h
 */

#ifndef LVGL_PRO_RCC_OVERVIEW_HERO_GEN_H
#define LVGL_PRO_RCC_OVERVIEW_HERO_GEN_H

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

lv_obj_t * rcc_overview_hero_create(lv_obj_t * parent, int32_t card_height, lv_font_t * heading, lv_font_t * small, const char * state_text, const char * sub_text, int32_t summary_x);
lv_obj_t * rcc_overview_hero_get_steps(lv_obj_t *parent);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_RCC_OVERVIEW_HERO_GEN_H*/