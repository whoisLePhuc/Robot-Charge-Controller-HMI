/**
 * @file rcc_overview_system_gen.h
 */

#ifndef LVGL_PRO_RCC_OVERVIEW_SYSTEM_GEN_H
#define LVGL_PRO_RCC_OVERVIEW_SYSTEM_GEN_H

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

lv_obj_t * rcc_overview_system_create(lv_obj_t * parent, int32_t card_y, int32_t card_height, lv_font_t * small, lv_font_t * bold, const void * uart_icon, const void * fault_icon, const void * relay_icon, const void * session_icon);
lv_obj_t * rcc_overview_system_get_uart_hit(lv_obj_t *parent);
lv_obj_t * rcc_overview_system_get_fault_hit(lv_obj_t *parent);
lv_obj_t * rcc_overview_system_get_session_hit(lv_obj_t *parent);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_RCC_OVERVIEW_SYSTEM_GEN_H*/