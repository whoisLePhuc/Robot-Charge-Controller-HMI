/* LVGL configuration for the PC simulator. Mirrors firmware/sdkconfig.esp32dev (fonts,
 * widgets, 16-bit colour, 64 KiB builtin heap) so the render matches the panel. */
#ifndef LV_CONF_H
#define LV_CONF_H
#define LV_COLOR_DEPTH 16
#define LV_USE_STDLIB_MALLOC LV_STDLIB_BUILTIN
#define LV_USE_STDLIB_STRING LV_STDLIB_BUILTIN
#define LV_USE_STDLIB_SPRINTF LV_STDLIB_BUILTIN
#define LV_MEM_SIZE (64 * 1024)
#define LV_USE_OS LV_OS_NONE
#define LV_USE_LOG 0
#define LV_USE_ASSERT_NULL 1
#define LV_FONT_MONTSERRAT_10 1
#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_20 1
#define LV_FONT_MONTSERRAT_22 1
#define LV_FONT_MONTSERRAT_28 1
#define LV_FONT_MONTSERRAT_40 1
#define LV_FONT_MONTSERRAT_48 1
#define LV_FONT_DEFAULT &lv_font_montserrat_14
#define LV_USE_FONT_PLACEHOLDER 1
#define LV_DEF_REFR_PERIOD 33
#endif
