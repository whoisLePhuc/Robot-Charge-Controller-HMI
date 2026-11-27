/**
 * @file rcc_shell_header_fault_gen.h
 */

#ifndef LVGL_PRO_RCC_SHELL_HEADER_FAULT_GEN_H
#define LVGL_PRO_RCC_SHELL_HEADER_FAULT_GEN_H

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

lv_obj_t * rcc_shell_header_fault_create(lv_obj_t * parent, const void * glyph);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_RCC_SHELL_HEADER_FAULT_GEN_H*/