/**
 * @file rcc_shell_confirm_gen.h
 */

#ifndef LVGL_PRO_RCC_SHELL_CONFIRM_GEN_H
#define LVGL_PRO_RCC_SHELL_CONFIRM_GEN_H

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

lv_obj_t * rcc_shell_confirm_create(lv_obj_t * parent, const char * title, const char * controller_state, const char * link_status, const char * measurement_label, const char * measurement_status, const char * fault_status, lv_color_t fault_color, const char * footnote, lv_font_t * title_font, lv_font_t * row_font, lv_font_t * button_font, lv_font_t * body_font);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_RCC_SHELL_CONFIRM_GEN_H*/