#include "rcc_link.h"

#include <limits.h>
#include <string.h>

#include "driver/uart.h"
#include "esp_random.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "rcc_codec.h"

#define LINK_UART UART_NUM_0
#define LINK_BAUD 115200
#define CMD_PING 0x0001u
#define CMD_DEVICE 0x0002u
#define CMD_STATUS 0x0010u
#define CMD_MEASURE 0x0011u
#define CMD_FAULTS 0x0012u
#define CMD_EVENTS 0x0013u
#define CMD_START 0x0020u
#define CMD_STOP 0x0021u

typedef struct {
    uint32_t id;
    uint16_t command;
    uint8_t wire[RCC_CODEC_WIRE_MAX];
    size_t length;
    int64_t sent_us;
    int64_t deadline_us;
    uint8_t attempts;
    bool active;
    bool accepted;
} transaction_t;

static SemaphoreHandle_t s_model_lock;
static rcc_snapshot_t s_model;
static rcc_codec_t s_codec;
static rcc_message_t s_message;
static transaction_t s_query;
static transaction_t s_start;
static transaction_t s_stop;
static uint32_t s_next_id = 1u;
static uint32_t s_event_cursor;
static int64_t s_last_reply_us;
static int64_t s_last_status_us;
static int64_t s_last_measure_us;
static int64_t s_last_faults_us;
static int64_t s_last_command_us;
static int64_t s_last_frame_us;
static bool s_start_requested;
static bool s_stop_requested;

static uint16_t u16(const uint8_t *p)
{
    return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

static uint32_t u32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static uint64_t u64(const uint8_t *p)
{
    return (uint64_t)u32(p) | ((uint64_t)u32(p + 4) << 32);
}

static uint32_t elapsed_ms(int64_t then_us, int64_t now_us)
{
    if (then_us == 0 || now_us < then_us) return UINT32_MAX;
    const uint64_t age = (uint64_t)(now_us - then_us) / 1000u;
    return age > UINT32_MAX ? UINT32_MAX : (uint32_t)age;
}

static uint32_t new_id(void)
{
    ++s_next_id;
    if (s_next_id == 0u) ++s_next_id;
    return s_next_id;
}

static bool send_transaction(transaction_t *tx, uint16_t command,
                             const uint8_t *payload, uint16_t payload_length)
{
    memset(tx, 0, sizeof(*tx));
    tx->id = new_id();
    tx->command = command;
    if (!rcc_codec_request(command, tx->id, payload, payload_length,
                           tx->wire, &tx->length)) return false;
    tx->active = true;
    tx->attempts = 1u;
    tx->sent_us = esp_timer_get_time();
    tx->deadline_us = tx->sent_us + 50000;
    if (uart_write_bytes(LINK_UART, tx->wire, tx->length) != (int)tx->length) {
        tx->active = false;
        return false;
    }
    return true;
}

static void set_control_state(const transaction_t *tx, rcc_command_state_t state,
                              uint16_t reason)
{
    if (s_model.last_command == tx->command) {
        s_model.command_state = state;
        s_model.command_reason = reason;
    }
}

static void transaction_tick(transaction_t *tx, bool control, int64_t now)
{
    if (!tx->active || now < tx->deadline_us) return;
    if (!tx->accepted && tx->attempts < 3u) {
        ++tx->attempts;
        tx->sent_us = now;
        tx->deadline_us = now + 50000;
        (void)uart_write_bytes(LINK_UART, tx->wire, tx->length);
        return;
    }
    tx->active = false;
    ++s_model.timeouts;
    if (control) set_control_state(tx, RCC_COMMAND_UNCONFIRMED, 0u);
}

static void set_header(const uint8_t *p)
{
    s_model.top_state = p[4];
    s_model.operational_state = p[5];
    s_model.primary_fault = u16(&p[6]);
    s_model.inhibit_mask = u32(&p[8]);
}

static void push_event(uint16_t code, uint32_t sequence, uint64_t us, const uint32_t args[4])
{
    enum { LAST = RCC_EVENT_SLOTS - 1 };
    if (s_model.event_count == RCC_EVENT_SLOTS) {
        memmove(s_model.event_codes, s_model.event_codes + 1, LAST * sizeof(uint16_t));
        memmove(s_model.event_sequences, s_model.event_sequences + 1, LAST * sizeof(uint32_t));
        memmove(s_model.event_us, s_model.event_us + 1, LAST * sizeof(uint64_t));
        memmove(s_model.event_args, s_model.event_args + 1, LAST * sizeof(s_model.event_args[0]));
        s_model.event_count = LAST;
    }
    const uint8_t n = s_model.event_count++;
    s_model.event_codes[n] = code;
    s_model.event_sequences[n] = sequence;
    s_model.event_us[n] = us;
    memcpy(s_model.event_args[n], args, 4u * sizeof(uint32_t));
}

static void parse_events(const uint8_t *data, uint16_t length)
{
    if (length < 16u) return;
    s_event_cursor = u32(data);
    s_model.event_missed = u32(&data[8]);
    const uint8_t count = data[12];
    uint16_t offset = 16u;
    for (uint8_t i = 0u; i < count && offset < length; ++i) {
        const uint8_t entry_length = data[offset++];
        if (entry_length < 28u || entry_length > length - offset) break;
        const uint32_t sequence = u32(&data[offset + 4u]);
        if (s_model.event_count == 0u ||
            s_model.event_sequences[s_model.event_count - 1u] != sequence) {
            uint32_t args[4] = {0u, 0u, 0u, 0u};
            for (uint8_t w = 0u; w < 4u && 28u + 4u * (w + 1u) <= entry_length; ++w)
                args[w] = u32(&data[offset + 28u + 4u * w]);
            const uint16_t code = u16(&data[offset]);
            push_event(code, sequence, u64(&data[offset + 16u]), args);
            if (code == 0x0015u || code == 0x0016u) {   /* SESSION_COMPLETED / ABORTED */
                s_model.end_valid = true;
                s_model.end_session_id = args[0];
                s_model.end_reason = (uint8_t)args[1];
                s_model.end_duration_ms = args[2];
            }
        }
        offset += entry_length;
    }
}

static void parse_reply_data(uint16_t command, const uint8_t *data,
                             uint16_t length, int64_t now)
{
    if (command == CMD_DEVICE && length == 40u && u16(data) == 1u && data[2] == 1u) {
        const uint64_t boot = u64(&data[20]);
        if (s_model.device_valid && s_model.boot_id != boot) {
            s_model.status_valid = false;
            s_model.measurement_valid = false;
            s_model.faults_valid = false;
            s_model.event_count = 0u;
            s_model.end_valid = false;
            s_event_cursor = 0u;
        }
        s_model.boot_id = boot;
        s_model.product_id = u16(data);
        s_model.protocol_version = data[2];
        memcpy(s_model.image_id, &data[12], 8u);
        s_model.running_links = u32(&data[28]);
        s_model.build_flags = data[3];
        s_model.hardware_revision = u32(&data[4]);
        s_model.firmware_revision = u32(&data[8]);
        s_model.device_valid = true;
    } else if (command == CMD_STATUS && length == 48u) {
        const uint64_t boot = u64(&data[4]);
        if (s_model.device_valid && s_model.boot_id != boot) {
            s_model.device_valid = false;
            s_model.measurement_valid = false;
            s_model.faults_valid = false;
            s_model.event_count = 0u;
            s_model.end_valid = false;
            s_event_cursor = 0u;
            if (s_start.active) {
                s_start.active = false;
                set_control_state(&s_start, RCC_COMMAND_UNCONFIRMED, 0u);
            }
            if (s_stop.active) {
                s_stop.active = false;
                set_control_state(&s_stop, RCC_COMMAND_UNCONFIRMED, 0u);
            }
        }
        s_model.boot_id = boot;
        s_model.active_fault_mask = u32(&data[12]);
        s_model.relay_command = data[16];
        s_model.relay_feedback = data[17];
        s_model.charging_established = (data[18] & 1u) != 0u;
        s_model.session_id = u32(&data[20]);
        s_model.status_valid = true;
        s_last_status_us = now;
    } else if (command == CMD_MEASURE && length == 32u) {
        s_model.measurement_sample_age_ms = u32(&data[4]);
        s_model.vout_mv = u32(&data[8]);
        s_model.iout_ma = (int32_t)u32(&data[12]);
        s_model.measurement_sequence = u32(data);
        s_model.vout_filtered_mv = u32(&data[16]);
        s_model.iout_filtered_ma = (int32_t)u32(&data[20]);
        s_model.vout_status = u16(&data[24]);
        s_model.iout_status = u16(&data[26]);
        s_model.measurement_valid = true;
        s_last_measure_us = now;
    } else if (command == CMD_FAULTS && length == 12u) {
        s_model.active_fault_mask = u32(data);
        s_model.inhibit_mask = u32(&data[4]);
        s_model.primary_fault = u16(&data[8]);
        s_model.faults_valid = true;
        s_last_faults_us = now;
    } else if (command == CMD_EVENTS) {
        parse_events(data, length);
    }
}

static void handle_result(const rcc_message_t *message, int64_t now)
{
    if (message->message_type != 2u || message->payload_length < 12u) return;
    transaction_t *tx = NULL;
    bool control = false;
    if (s_stop.active && s_stop.id == message->request_id) {
        tx = &s_stop; control = true;
    } else if (s_start.active && s_start.id == message->request_id) {
        tx = &s_start; control = true;
    } else if (s_query.active && s_query.id == message->request_id) {
        tx = &s_query;
    }
    if (tx == NULL || tx->command != message->command) return;

    s_last_reply_us = now;
    s_model.online = true;
    const uint8_t *p = message->payload;
    const uint8_t result = p[0];
    const uint16_t reason = u16(&p[2]);
    set_header(p);
    if (control) s_last_command_us = now;
    if (result == 0u) {
        tx->accepted = true;
        tx->deadline_us = now + 15000000;
        if (control) set_control_state(tx, RCC_COMMAND_ACCEPTED, reason);
        return;
    }
    tx->active = false;
    if (result == 2u) {
        parse_reply_data(tx->command, p + 12u, message->payload_length - 12u, now);
    }
    if (control) {
        const rcc_command_state_t state = result == 1u ? RCC_COMMAND_REJECTED :
                                           result == 2u ? RCC_COMMAND_COMPLETED :
                                           result == 3u ? RCC_COMMAND_FAILED :
                                           RCC_COMMAND_UNCONFIRMED;
        set_control_state(tx, state, reason);
    }
}

static void send_next_query(int64_t now)
{
    static uint8_t phase;
    static int64_t last_query_us;
    if (s_query.active || now - last_query_us < 170000) return;
    const uint16_t commands[] = {CMD_PING, CMD_STATUS, CMD_MEASURE,
                                 CMD_STATUS, CMD_FAULTS, CMD_EVENTS,
                                 CMD_STATUS, CMD_MEASURE, CMD_DEVICE};
    const uint16_t command = commands[phase];
    phase = (phase + 1u) % (sizeof(commands) / sizeof(commands[0]));
    uint8_t event_payload[8] = {0};
    if (command == CMD_EVENTS) {
        event_payload[0] = (uint8_t)s_event_cursor;
        event_payload[1] = (uint8_t)(s_event_cursor >> 8);
        event_payload[2] = (uint8_t)(s_event_cursor >> 16);
        event_payload[3] = (uint8_t)(s_event_cursor >> 24);
        event_payload[4] = 4u;
    }
    (void)send_transaction(&s_query, command,
                           command == CMD_EVENTS ? event_payload : NULL,
                           command == CMD_EVENTS ? sizeof(event_payload) : 0u);
    last_query_us = now;
}

static void link_task(void *argument)
{
    (void)argument;
    uint8_t bytes[96];
    for (;;) {
        const int64_t now = esp_timer_get_time();
        const int read = uart_read_bytes(LINK_UART, bytes, sizeof(bytes), pdMS_TO_TICKS(10));
        if (read > 0) {
            for (int i = 0; i < read; ++i) {
                s_last_frame_us = now;
                if (rcc_codec_feed(&s_codec, bytes[i], &s_message)) {
                    if (xSemaphoreTake(s_model_lock, portMAX_DELAY) == pdTRUE) {
                        handle_result(&s_message, now);
                        xSemaphoreGive(s_model_lock);
                    }
                }
            }
        }
        if (s_codec.object_length && now - s_last_frame_us > 2000000) {
            rcc_codec_reset_partial(&s_codec);
        }
        if (xSemaphoreTake(s_model_lock, portMAX_DELAY) != pdTRUE) continue;
        s_model.rx_bad_frames = s_codec.invalid_frames;
        if (elapsed_ms(s_last_reply_us, now) > 2000u) s_model.online = false;
        transaction_tick(&s_query, false, now);
        transaction_tick(&s_start, true, now);
        transaction_tick(&s_stop, true, now);
        if (s_stop_requested && !s_stop.active) {
            s_stop_requested = false;
            s_model.last_command = CMD_STOP;
            s_model.command_state = RCC_COMMAND_SENDING;
            s_model.command_reason = 0u;
            if (!send_transaction(&s_stop, CMD_STOP, NULL, 0u)) {
                s_model.command_state = RCC_COMMAND_UNCONFIRMED;
            }
        } else if (s_start_requested && !s_start.active && !s_stop.active) {
            s_start_requested = false;
            s_model.last_command = CMD_START;
            s_model.command_state = RCC_COMMAND_SENDING;
            s_model.command_reason = 0u;
            if (!send_transaction(&s_start, CMD_START, NULL, 0u)) {
                s_model.command_state = RCC_COMMAND_UNCONFIRMED;
            }
        }
        send_next_query(now);
        xSemaphoreGive(s_model_lock);
    }
}

esp_err_t rcc_link_start(void)
{
    if (s_model_lock != NULL) return ESP_ERR_INVALID_STATE;
    s_model_lock = xSemaphoreCreateMutex();
    if (s_model_lock == NULL) return ESP_ERR_NO_MEM;
    /* The controller retains recent request IDs across a quick HMI reboot. */
    s_next_id = esp_random();
    rcc_codec_init(&s_codec);
    const uart_config_t config = {
        .baud_rate = LINK_BAUD,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    esp_err_t err = uart_param_config(LINK_UART, &config);
    if (err == ESP_OK) err = uart_set_pin(LINK_UART, 1, 3, UART_PIN_NO_CHANGE,
                                         UART_PIN_NO_CHANGE);
    if (err == ESP_OK) err = uart_driver_install(LINK_UART, 2048, 2048, 0, NULL, 0);
    if (err == ESP_OK && xTaskCreate(link_task, "rcc_link", 6144, NULL, 5, NULL) != pdPASS) {
        err = ESP_ERR_NO_MEM;
    }
    if (err != ESP_OK) {
        (void)uart_driver_delete(LINK_UART);
        vSemaphoreDelete(s_model_lock);
        s_model_lock = NULL;
    }
    return err;
}

void rcc_link_snapshot(rcc_snapshot_t *out)
{
    if (out == NULL) return;
    memset(out, 0, sizeof(*out));
    if (s_model_lock == NULL || xSemaphoreTake(s_model_lock, pdMS_TO_TICKS(20)) != pdTRUE)
        return;
    *out = s_model;
    const int64_t now = esp_timer_get_time();
    out->reply_age_ms = elapsed_ms(s_last_reply_us, now);
    out->status_age_ms = elapsed_ms(s_last_status_us, now);
    out->measurement_age_ms = elapsed_ms(s_last_measure_us, now);
    out->faults_age_ms = elapsed_ms(s_last_faults_us, now);
    out->command_age_ms = elapsed_ms(s_last_command_us, now);
    out->online = out->reply_age_ms <= 2000u;
    xSemaphoreGive(s_model_lock);
}

bool rcc_link_request_start(void)
{
    if (s_model_lock == NULL || xSemaphoreTake(s_model_lock, pdMS_TO_TICKS(20)) != pdTRUE)
        return false;
    const bool allowed = s_model.online && s_model.status_valid &&
                         elapsed_ms(s_last_status_us, esp_timer_get_time()) < 1500u &&
                         !s_start.active && !s_stop.active && !s_start_requested &&
                         !s_stop_requested;
    if (allowed) s_start_requested = true;
    xSemaphoreGive(s_model_lock);
    return allowed;
}

bool rcc_link_request_stop(void)
{
    if (s_model_lock == NULL || xSemaphoreTake(s_model_lock, pdMS_TO_TICKS(20)) != pdTRUE)
        return false;
    const bool allowed = !s_stop.active && !s_stop_requested;
    if (allowed) s_stop_requested = true;
    xSemaphoreGive(s_model_lock);
    return allowed;
}
