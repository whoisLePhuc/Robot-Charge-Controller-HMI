/*
 * Board support for the ESP32-3248S035R: ST7796 LCD, XPT2046 resistive touch, LVGL port.
 * Pins and measurements: docs/hmi-hardware.md. Decisions: docs/decisions.md (D03-D06).
 */
#pragma once

#include "esp_err.h"
#include "lvgl.h"

/* Landscape resolution (HMI-D04). */
#define HMI_BOARD_HRES 480
#define HMI_BOARD_VRES 320

typedef struct {
    esp_err_t display;   /* SPI bus, LCD panel, LVGL display */
    esp_err_t touch;     /* XPT2046 and LVGL input device */
    lv_display_t *lvgl_display;
    lv_indev_t *lvgl_touch;
} hmi_board_status_t;

/*
 * Brings up the LCD, touch and LVGL. The backlight is switched on once the display is
 * ready. Logging is disabled (UART0 carries the controller link, HMI-D03), so failures
 * are reported through the returned status only.
 */
hmi_board_status_t hmi_board_init(void);
