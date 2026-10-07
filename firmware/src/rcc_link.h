#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"

#define RCC_EVENT_SLOTS 8

typedef enum {
    RCC_COMMAND_NONE,
    RCC_COMMAND_SENDING,
    RCC_COMMAND_ACCEPTED,
    RCC_COMMAND_COMPLETED,
    RCC_COMMAND_REJECTED,
    RCC_COMMAND_FAILED,
    RCC_COMMAND_UNCONFIRMED,
} rcc_command_state_t;

typedef struct {
    bool online;
    bool device_valid;
    bool status_valid;
    bool measurement_valid;
    bool faults_valid;
    uint32_t reply_age_ms;
    uint32_t status_age_ms;
    uint32_t measurement_age_ms;
    uint8_t top_state;
    uint8_t operational_state;
    uint8_t relay_command;
    uint8_t relay_feedback;
    bool charging_established;
    uint32_t session_id;
    uint32_t active_fault_mask;
    uint32_t inhibit_mask;
    uint16_t primary_fault;
    uint32_t vout_mv;
    int32_t iout_ma;
    uint16_t vout_status;
    uint16_t iout_status;
    uint32_t measurement_sample_age_ms;
    uint32_t firmware_revision;
    uint32_t hardware_revision;
    uint8_t build_flags;
    uint64_t boot_id;
    uint16_t last_command;
    rcc_command_state_t command_state;
    uint16_t command_reason;
    uint32_t rx_bad_frames;
    uint32_t timeouts;
    uint32_t event_missed;
    uint8_t event_count;                          /* oldest first */
    uint16_t event_codes[RCC_EVENT_SLOTS];
    uint32_t event_sequences[RCC_EVENT_SLOTS];
    uint64_t event_us[RCC_EVENT_SLOTS];           /* controller monotonic_us, since boot */
    uint32_t event_args[RCC_EVENT_SLOTS][4];      /* first 16 data bytes after the header */
    /* GET_MEASUREMENTS */
    uint32_t measurement_sequence;
    uint32_t vout_filtered_mv;
    int32_t iout_filtered_ma;
    /* GET_DEVICE_INFO */
    uint16_t product_id;
    uint8_t protocol_version;
    uint8_t image_id[8];
    uint32_t running_links;                       /* bit0 CAN, bit1 RS485, bit2 UART, bit3 SVC */
    /* Ages and last session end (SESSION_COMPLETED / SESSION_ABORTED events). */
    uint32_t faults_age_ms;
    uint32_t command_age_ms;
    bool end_valid;
    uint8_t end_reason;
    uint32_t end_session_id;
    uint32_t end_duration_ms;
} rcc_snapshot_t;

/* UART0 owner. Call once after board init. Does not send START or STOP on boot. */
esp_err_t rcc_link_start(void);
/* Copies a coherent snapshot. May be called from the LVGL task. */
void rcc_link_snapshot(rcc_snapshot_t *out);
/* Nonblocking requests; STOP is accepted even when the link is offline. */
bool rcc_link_request_start(void);
bool rcc_link_request_stop(void);
