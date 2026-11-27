/*
 * Board-level constants shared by the application, the controller link and the UI
 * (the Device page shows them). Header only, no ESP-IDF includes.
 */
#pragma once

#define HMI_TWDT_TIMEOUT_MS 5000   /* task watchdog timeout; the LVGL task feeds it from the 1 s refresh */

/* The HMI side of the controller UART; the Device page shows these. */
#define RCC_LINK_UART_PORT 1
#define RCC_LINK_TX_GPIO 43
#define RCC_LINK_RX_GPIO 44
#define RCC_LINK_BAUD 115200
