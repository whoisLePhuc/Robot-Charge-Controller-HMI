#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HMI_OTA_IDLE = 0,
    HMI_OTA_RECEIVING,
    HMI_OTA_VERIFYING,
    HMI_OTA_SUCCESS,
    HMI_OTA_FAILED,
} hmi_ota_state_t;

typedef struct {
    hmi_ota_state_t state;
    uint8_t progress_pct;      /* 0..100 */
    size_t bytes_written;
    size_t total_bytes;
    char running_slot[32];     /* e.g. "ota_0" or "ota_1" */
    char next_slot[32];        /* target partition e.g. "ota_1" */
    char running_version[32];  /* IDF / Project version string */
    char error_msg[64];
    uint32_t version;          /* increments on change */
} hmi_ota_status_t;

/* Start/stop the Web OTA HTTP Server on port 80 */
esp_err_t hmi_ota_start_server(void);
void hmi_ota_stop_server(void);
bool hmi_ota_server_is_running(void);

/* Query current OTA status */
void hmi_ota_get_status(hmi_ota_status_t *out);

/* Mark the current running app partition as valid (cancels rollback).
 * Call after system successfully starts up and TWDT is active. */
esp_err_t hmi_ota_mark_valid(void);

#ifdef __cplusplus
}
#endif
