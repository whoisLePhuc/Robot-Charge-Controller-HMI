/* Elecrow DLE06235B / ES3C35P pin allocation, not the older CrowPanel Terminal. Private to the BSP.
 * The controller UART pins are in hmi_config.h (hmi_common), because the Device page shows them. */
#pragma once

enum {
    BSP_LCD_CS = 10, BSP_LCD_CLK = 12,
    BSP_LCD_D0 = 11, BSP_LCD_D1 = 13, BSP_LCD_D2 = 14, BSP_LCD_D3 = 9,
    BSP_BACKLIGHT_GPIO = 41,
    BSP_TOUCH_SDA = 38, BSP_TOUCH_SCL = 39, BSP_TOUCH_RST = 48, BSP_TOUCH_INT = 47,
};
