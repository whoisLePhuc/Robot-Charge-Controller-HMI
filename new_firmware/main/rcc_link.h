#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"

#include "rcc_types.h"

/* Controller transport over UART1 (TX GPIO43 / RX GPIO44, 115200 8N1). */
esp_err_t rcc_link_start(void);
/* Copies a coherent snapshot. May be called from the LVGL task. */
void rcc_link_snapshot(rcc_snapshot_t *out);
/* Nonblocking requests; STOP is accepted even when the link is offline. */
bool rcc_link_request_start(void);
bool rcc_link_request_stop(void);
