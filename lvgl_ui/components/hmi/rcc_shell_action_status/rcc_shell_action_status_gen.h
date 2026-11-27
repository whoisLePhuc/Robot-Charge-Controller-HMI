/**
 * @file rcc_shell_action_status_gen.h
 */

#ifndef LVGL_PRO_RCC_SHELL_ACTION_STATUS_GEN_H
#define LVGL_PRO_RCC_SHELL_ACTION_STATUS_GEN_H

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

lv_obj_t * rcc_shell_action_status_create(lv_obj_t * parent, int32_t message_width, const char * title, const char * subtitle, lv_color_t title_color, lv_font_t * title_font, lv_font_t * body_font);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_RCC_SHELL_ACTION_STATUS_GEN_H*/