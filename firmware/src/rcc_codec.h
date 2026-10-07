#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define RCC_CODEC_OBJECT_MAX 512u
#define RCC_CODEC_PAYLOAD_MAX 496u
#define RCC_CODEC_WIRE_MAX 145u

typedef struct {
    uint8_t message_type;
    uint16_t command;
    uint32_t request_id;
    uint16_t payload_length;
    uint8_t payload[RCC_CODEC_PAYLOAD_MAX];
} rcc_message_t;

/* One parser instance owns the bounded frame and reassembly storage. Task context only. */
typedef struct {
    uint8_t body[RCC_CODEC_WIRE_MAX];
    uint16_t body_length;
    bool dropping;
    uint8_t object[RCC_CODEC_OBJECT_MAX];
    uint16_t object_length;
    uint16_t received;
    uint8_t next_fragment;
    uint32_t assembling_id;
    uint32_t invalid_frames;
} rcc_codec_t;

void rcc_codec_init(rcc_codec_t *codec);
void rcc_codec_reset_partial(rcc_codec_t *codec);
/* Returns true only for a complete, CRC-checked COMMAND_RESULT or EVENT. */
bool rcc_codec_feed(rcc_codec_t *codec, uint8_t byte, rcc_message_t *message);
/* Current HMI requests fit in a SINGLE frame. No allocation; false means invalid input. */
bool rcc_codec_request(uint16_t command, uint32_t request_id,
                       const uint8_t *payload, uint16_t payload_length,
                       uint8_t wire[RCC_CODEC_WIRE_MAX], size_t *wire_length);
/* Checks our encoder against the controller repository's independent wire vector. */
bool rcc_codec_self_test(void);
