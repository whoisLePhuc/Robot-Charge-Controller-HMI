#include "rcc_codec.h"

#include <string.h>

#define FRAME_HEADER 12u
#define FRAME_CRC 2u
#define FRAGMENT_MAX 128u
#define RAW_MAX (FRAME_HEADER + FRAGMENT_MAX + FRAME_CRC)

static uint16_t get16(const uint8_t *p)
{
    return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

static uint32_t get32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static void put16(uint8_t *p, uint16_t value)
{
    p[0] = (uint8_t)value;
    p[1] = (uint8_t)(value >> 8);
}

static void put32(uint8_t *p, uint32_t value)
{
    for (unsigned i = 0; i < 4; ++i) {
        p[i] = (uint8_t)(value >> (8u * i));
    }
}

static uint16_t crc16(const uint8_t *data, size_t length)
{
    uint16_t crc = 0xffffu;
    for (size_t i = 0; i < length; ++i) {
        crc ^= (uint16_t)data[i] << 8;
        for (unsigned bit = 0; bit < 8; ++bit) {
            crc = (uint16_t)((crc & 0x8000u) ? (crc << 1) ^ 0x1021u : crc << 1);
        }
    }
    return crc;
}

static uint32_t crc32(const uint8_t *data, size_t length)
{
    uint32_t crc = 0xffffffffu;
    for (size_t i = 0; i < length; ++i) {
        crc ^= data[i];
        for (unsigned bit = 0; bit < 8; ++bit) {
            crc = (crc & 1u) ? (crc >> 1) ^ 0xedb88320u : crc >> 1;
        }
    }
    return crc ^ 0xffffffffu;
}

static bool cobs_encode(const uint8_t *src, size_t length, uint8_t *dst,
                        size_t capacity, size_t *out_length)
{
    if (capacity == 0u) return false;
    size_t code_at = 0u, written = 1u;
    uint8_t code = 1u;
    for (size_t i = 0; i < length; ++i) {
        if (src[i] == 0u) {
            dst[code_at] = code;
            if (written >= capacity) return false;
            code_at = written++;
            code = 1u;
        } else {
            if (written >= capacity) return false;
            dst[written++] = src[i];
            ++code;
            if (code == 0xffu) {
                dst[code_at] = code;
                if (written >= capacity) return false;
                code_at = written++;
                code = 1u;
            }
        }
    }
    dst[code_at] = code;
    *out_length = written;
    return true;
}

static bool cobs_decode(const uint8_t *src, size_t length, uint8_t *dst,
                        size_t capacity, size_t *out_length)
{
    size_t read = 0u, written = 0u;
    while (read < length) {
        const uint8_t code = src[read++];
        if (code == 0u || (size_t)(code - 1u) > length - read) return false;
        for (unsigned n = 1u; n < code; ++n) {
            if (written >= capacity || src[read] == 0u) return false;
            dst[written++] = src[read++];
        }
        if (code != 0xffu && read < length) {
            if (written >= capacity) return false;
            dst[written++] = 0u;
        }
    }
    *out_length = written;
    return true;
}

void rcc_codec_init(rcc_codec_t *codec)
{
    if (codec != NULL) memset(codec, 0, sizeof(*codec));
}

void rcc_codec_reset_partial(rcc_codec_t *codec)
{
    if (codec != NULL) {
        codec->object_length = 0u;
        codec->received = 0u;
        codec->body_length = 0u;
        codec->dropping = true;
    }
}

static bool object_ready(const uint8_t *object, uint16_t length,
                         uint32_t frame_id, rcc_message_t *message)
{
    if (length < 16u || object[0] != 1u || get32(&object[length - 4u]) !=
        crc32(object, length - 4u)) return false;
    const uint8_t type = object[1];
    const uint16_t payload_length = get16(&object[10]);
    const uint32_t id = get32(&object[6]);
    if (get16(&object[2]) != 0u || (uint32_t)payload_length + 16u != length ||
        id != frame_id || !((type == 2u && id != 0u) ||
                             (type == 3u && id == 0u && get16(&object[4]) == 0u))) {
        return false;
    }
    message->message_type = type;
    message->command = get16(&object[4]);
    message->request_id = id;
    message->payload_length = payload_length;
    if (payload_length != 0u) memcpy(message->payload, &object[12], payload_length);
    return true;
}

static bool accept_body(rcc_codec_t *codec, rcc_message_t *message)
{
    uint8_t raw[RAW_MAX];
    size_t length = 0u;
    if (!cobs_decode(codec->body, codec->body_length, raw, sizeof(raw), &length) ||
        length < 15u || get16(&raw[length - 2u]) != crc16(raw, length - 2u)) return false;
    const uint8_t type = raw[1], index = raw[10];
    const uint16_t object_length = get16(&raw[8]);
    const uint32_t id = get32(&raw[4]);
    if (raw[0] != 1u || raw[2] != 0x02u || raw[3] != 0x01u || raw[11] != 0u ||
        object_length < 16u || object_length > RCC_CODEC_OBJECT_MAX ||
        index >= (object_length + 127u) / 128u) return false;
    const uint16_t offset = (uint16_t)index * FRAGMENT_MAX;
    const uint16_t fragment_length = (uint16_t)(length - FRAME_HEADER - FRAME_CRC);
    const uint16_t required = object_length - offset > FRAGMENT_MAX ?
                              FRAGMENT_MAX : object_length - offset;
    if (fragment_length != required) return false;
    if (type == 1u) {
        codec->object_length = 0u;
        if (index != 0u || object_length > FRAGMENT_MAX) return false;
        return object_ready(&raw[12], object_length, id, message);
    }
    if (type == 2u) {
        if (index != 0u || object_length <= FRAGMENT_MAX) return false;
        codec->object_length = object_length;
        codec->received = fragment_length;
        codec->next_fragment = 1u;
        codec->assembling_id = id;
        memcpy(codec->object, &raw[12], fragment_length);
        return false;
    }
    if (type != 3u || index == 0u || codec->object_length != object_length ||
        codec->next_fragment != index || codec->assembling_id != id ||
        (uint32_t)codec->received + fragment_length > object_length) {
        codec->object_length = 0u;
        return false;
    }
    memcpy(&codec->object[codec->received], &raw[12], fragment_length);
    codec->received += fragment_length;
    ++codec->next_fragment;
    if (codec->received != object_length) return false;
    codec->object_length = 0u;
    return object_ready(codec->object, object_length, id, message);
}

bool rcc_codec_feed(rcc_codec_t *codec, uint8_t byte, rcc_message_t *message)
{
    if (codec == NULL || message == NULL) return false;
    if (byte == 0u) {
        const bool ready = !codec->dropping && codec->body_length != 0u &&
                           accept_body(codec, message);
        codec->body_length = 0u;
        codec->dropping = false;
        return ready;
    }
    if (codec->dropping) return false;
    if (codec->body_length >= sizeof(codec->body)) {
        codec->body_length = 0u;
        codec->dropping = true;
        ++codec->invalid_frames;
        return false;
    }
    codec->body[codec->body_length++] = byte;
    return false;
}

bool rcc_codec_request(uint16_t command, uint32_t request_id,
                       const uint8_t *payload, uint16_t payload_length,
                       uint8_t wire[RCC_CODEC_WIRE_MAX], size_t *wire_length)
{
    if (wire == NULL || wire_length == NULL || request_id == 0u ||
        payload_length > 32u || (payload_length && payload == NULL)) return false;
    uint8_t object[16u + 32u];
    const uint16_t object_length = 16u + payload_length;
    object[0] = 1u; object[1] = 1u;
    put16(&object[2], 0u);
    put16(&object[4], command);
    put32(&object[6], request_id);
    put16(&object[10], payload_length);
    if (payload_length) memcpy(&object[12], payload, payload_length);
    put32(&object[12u + payload_length], crc32(object, 12u + payload_length));

    uint8_t raw[FRAME_HEADER + sizeof(object) + FRAME_CRC];
    raw[0] = 1u; raw[1] = 1u; raw[2] = 1u; raw[3] = 2u;
    put32(&raw[4], request_id);
    put16(&raw[8], object_length);
    raw[10] = 0u; raw[11] = 0u;
    memcpy(&raw[12], object, object_length);
    const size_t covered = FRAME_HEADER + object_length;
    put16(&raw[covered], crc16(raw, covered));
    size_t encoded = 0u;
    wire[0] = 0u;
    if (!cobs_encode(raw, covered + FRAME_CRC, &wire[1],
                     RCC_CODEC_WIRE_MAX - 2u, &encoded)) return false;
    wire[encoded + 1u] = 0u;
    *wire_length = encoded + 2u;
    return true;
}

bool rcc_codec_self_test(void)
{
    /* firmware/tests/host/test_rcc_serial_frame.c in the controller repository. */
    static const uint8_t expected[] = {
        0x00, 0x0a, 0x01, 0x01, 0x01, 0x02, 0x78, 0x56, 0x34, 0x12,
        0x14, 0x01, 0x01, 0x03, 0x01, 0x01, 0x01, 0x02, 0x20, 0x06,
        0x78, 0x56, 0x34, 0x12, 0x04, 0x0b, 0xde, 0xad, 0xbe, 0xef,
        0x74, 0xc9, 0x40, 0x30, 0x26, 0xb1, 0x00,
    };
    static const uint8_t token[] = {0xde, 0xad, 0xbe, 0xef};
    uint8_t wire[RCC_CODEC_WIRE_MAX];
    size_t length = 0u;
    return rcc_codec_request(0x0020u, 0x12345678u, token, sizeof(token),
                             wire, &length) && length == sizeof(expected) &&
           memcmp(wire, expected, sizeof(expected)) == 0;
}
