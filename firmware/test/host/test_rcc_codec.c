/* Host test for src/rcc_codec.c (no ESP-IDF). Build and run: ./run.sh */
#include <stdio.h>
#include <string.h>
#include "rcc_codec.h"

static int failures;
#define CHECK(c) do { if (!(c)) { printf("FAIL %s:%d %s\n", __FILE__, __LINE__, #c); ++failures; } } while (0)

/* Independent reference encoder (controller -> HMI direction), written for the test only. */
static uint16_t crc16(const uint8_t *d, size_t n)
{ uint16_t c = 0xffff; while (n--) { c ^= (uint16_t)*d++ << 8; for (int i = 0; i < 8; ++i) c = c & 0x8000 ? (c << 1) ^ 0x1021 : c << 1; } return c; }
static uint32_t crc32(const uint8_t *d, size_t n)
{ uint32_t c = 0xffffffffu; while (n--) { c ^= *d++; for (int i = 0; i < 8; ++i) c = c & 1 ? (c >> 1) ^ 0xedb88320u : c >> 1; } return ~c; }
static size_t cobs(const uint8_t *s, size_t n, uint8_t *d)
{
    size_t ci = 0, w = 1; uint8_t code = 1;
    for (size_t i = 0; i < n; ++i) {
        if (s[i] == 0) { d[ci] = code; ci = w++; code = 1; }
        else { d[w++] = s[i]; if (++code == 0xff) { d[ci] = code; ci = w++; code = 1; } }
    }
    d[ci] = code; return w;
}
static void p16(uint8_t *p, uint16_t v) { p[0] = v; p[1] = v >> 8; }
static void p32(uint8_t *p, uint32_t v) { for (int i = 0; i < 4; ++i) p[i] = v >> (8 * i); }

/* Builds a (multi-)frame stream carrying a RESULT/EVENT object. Returns total length. */
static size_t build(uint8_t type, uint16_t cmd, uint32_t id, const uint8_t *payload,
                    uint16_t plen, uint8_t *out, int corrupt)
{
    uint8_t obj[512]; uint16_t olen = 16 + plen;
    obj[0] = 1; obj[1] = type; p16(&obj[2], 0); p16(&obj[4], cmd); p32(&obj[6], id);
    p16(&obj[10], plen); memcpy(&obj[12], payload, plen);
    p32(&obj[12 + plen], crc32(obj, 12 + plen));
    unsigned frames = (olen + 127) / 128; size_t total = 0;
    for (unsigned f = 0; f < frames; ++f) {
        uint16_t off = f * 128, fl = olen - off > 128 ? 128 : olen - off;
        uint8_t raw[12 + 128 + 2];
        raw[0] = 1; raw[1] = frames == 1 ? 1 : f == 0 ? 2 : 3; raw[2] = 2; raw[3] = 1;
        p32(&raw[4], id); p16(&raw[8], olen); raw[10] = f; raw[11] = 0;
        memcpy(&raw[12], obj + off, fl);
        p16(&raw[12 + fl], crc16(raw, 12 + fl));
        if (corrupt && f == 0) raw[13] ^= 0x55;
        out[total++] = 0; total += cobs(raw, 14 + fl, out + total); out[total++] = 0;
    }
    return total;
}

static int feed_all(rcc_codec_t *c, const uint8_t *w, size_t n, rcc_message_t *m)
{ int got = 0; for (size_t i = 0; i < n; ++i) if (rcc_codec_feed(c, w[i], m)) ++got; return got; }

int main(void)
{
    rcc_codec_t c; rcc_message_t m; uint8_t wire[1024];
    CHECK(rcc_codec_self_test());                      /* golden request vector */

    uint8_t pl[12 + 48]; memset(pl, 0, sizeof pl); pl[0] = 2; pl[4] = 4; pl[5] = 6;
    for (int i = 12; i < 60; ++i) pl[i] = (i % 7 == 0) ? 0 : i;   /* includes zero bytes */
    rcc_codec_init(&c);
    size_t n = build(2, 0x0010, 0xCAFE0001u, pl, sizeof pl, wire, 0);
    CHECK(feed_all(&c, wire, n, &m) == 1);
    CHECK(m.message_type == 2 && m.command == 0x0010 && m.request_id == 0xCAFE0001u);
    CHECK(m.payload_length == sizeof pl && memcmp(m.payload, pl, sizeof pl) == 0);

    /* Three-frame object (GET_EVENT_LOG page style). */
    uint8_t big[300]; for (int i = 0; i < 300; ++i) big[i] = (uint8_t)(i * 5 + 1);
    n = build(2, 0x0013, 0x01020304u, big, sizeof big, wire, 0);
    CHECK(feed_all(&c, wire, n, &m) == 1 && m.payload_length == 300 &&
          memcmp(m.payload, big, 300) == 0);

    /* Corrupt CRC is rejected; the parser recovers on the next good frame. */
    n = build(2, 0x0010, 5, pl, sizeof pl, wire, 1);
    CHECK(feed_all(&c, wire, n, &m) == 0);
    n = build(2, 0x0010, 6, pl, sizeof pl, wire, 0);
    CHECK(feed_all(&c, wire, n, &m) == 1 && m.request_id == 6);

    /* Garbage before a frame, overlong run without delimiter. */
    uint8_t junk[300]; memset(junk, 0x41, sizeof junk);
    CHECK(feed_all(&c, junk, sizeof junk, &m) == 0);
    n = build(2, 0x0001, 7, pl, 12, wire, 0);
    CHECK(feed_all(&c, wire, n, &m) == 1 && m.request_id == 7);

    /* Unsolicited EVENT: id 0, command 0. */
    n = build(3, 0, 0, pl, 12, wire, 0);
    CHECK(feed_all(&c, wire, n, &m) == 1 && m.message_type == 3);

    /* Encoder limits. */
    uint8_t w[RCC_CODEC_WIRE_MAX]; size_t wl;
    CHECK(!rcc_codec_request(0x10, 0, NULL, 0, w, &wl));        /* id 0 invalid */
    CHECK(rcc_codec_request(0x10, 1, NULL, 0, w, &wl) && w[0] == 0 && w[wl - 1] == 0);

    printf(failures ? "FAILED (%d)\n" : "ALL PASSED\n", failures);
    return failures != 0;
}
