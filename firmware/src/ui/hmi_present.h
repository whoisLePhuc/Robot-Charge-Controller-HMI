/*
 * Presentation logic for the HMI: names, derived state, formatters and banner/action-bar selection.
 * Pure C on top of rcc_snapshot_t, no LVGL; unit-tested on the host (test/host/test_hmi_present.c).
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "rcc_link.h"

#define HP_FAULT_COUNT 19
#define HP_INHIBIT_COUNT 3
#define HP_END_COUNT 10

/* Display tone; the theme maps it to a colour (hmi_theme.h ht_kind_color). */
typedef enum { HP_K_NORMAL, HP_K_WARN, HP_K_FAULT, HP_K_OFF, HP_K_ACCENT } hp_kind_t;
typedef enum { HP_FS_OK, HP_FS_FAULT, HP_FS_INHIBIT, HP_FS_UNKNOWN } hp_fault_state_t;
typedef enum { HP_B_NONE, HP_B_STOP, HP_B_START, HP_B_STOPPING } hp_bar_button_t;
typedef enum { HP_BAR_NONE, HP_BAR_FULL, HP_BAR_SPLIT, HP_BAR_MSG, HP_BAR_NOAUTH } hp_bar_kind_t;
typedef enum { HP_TGT_NONE = -1, HP_TGT_FAULTS, HP_TGT_LINK } hp_target_t;
typedef enum { HP_SB_NONE, HP_SB_LINK_STALE, HP_SB_FAULT_LATCHED } hp_start_block_t;   /* why START is not offered */
typedef enum { HP_FW_UNREAD, HP_FW_COMPATIBLE, HP_FW_INCOMPATIBLE } hp_fw_compat_t;

#define HP_PRODUCT_ID 1u         /* controller product / protocol version this HMI speaks */
#define HP_PROTOCOL_VERSION 1u
/* Full scale of the Overview gauges. */
#define HP_V_FULL_MV 60000
#define HP_I_FULL_MA 10000
#define HP_P_FULL_MW 600000
#define HP_V_FULL_TXT "60V"
#define HP_I_FULL_TXT "10A"
#define HP_P_FULL_TXT "600W"

extern const char *const hp_fault_names[HP_FAULT_COUNT];
extern const char *const hp_inhibit_names[HP_INHIBIT_COUNT];
extern const char *const hp_end_names[HP_END_COUNT];

const char *hp_fault_name(uint16_t id);
const char *hp_fault_short(uint16_t id);
const char *hp_fault_hint(uint16_t id);
const char *hp_state_name(uint8_t top, uint8_t op);
/* "A+B" names of the set bits, "bit N" past count, "none" for 0. */
void hp_mask_text(char *out, size_t cap, uint32_t mask, const char *const *names, int count);
const char *hp_event_name(uint16_t code);
void hp_event_detail(char *b, size_t n, uint16_t code, const uint32_t a[4]);
/* Controller reject reason; unknown codes are written to buf as hex. */
const char *hp_reason_text(uint16_t r, char *buf, size_t n);

bool hp_fresh(const rcc_snapshot_t *m);
bool hp_v_valid(const rcc_snapshot_t *m);
bool hp_v_floor(const rcc_snapshot_t *m);
bool hp_i_valid(const rcc_snapshot_t *m);
bool hp_meas_stale(const rcc_snapshot_t *m);
bool hp_in_session(const rcc_snapshot_t *m);
hp_fault_state_t hp_fault_state(const rcc_snapshot_t *m);
bool hp_start_allowed(const rcc_snapshot_t *m);
bool hp_no_authority(const rcc_snapshot_t *m);
/* Why START is blocked (stale link, latched fault), HP_SB_NONE when it is allowed or blocked for another reason. */
hp_start_block_t hp_start_block(const rcc_snapshot_t *m);
const char *hp_start_block_text(hp_start_block_t r);
hp_fw_compat_t hp_fw_compat(const rcc_snapshot_t *m);
const char *hp_fault_title(hp_fault_state_t fs);
/* Display tone of an event code (fault, warning, charge milestone, other = HP_K_OFF). */
hp_kind_t hp_event_kind(uint16_t code);
/* "STOP . OK" style summary of the last command, an em dash when none. */
void hp_command_text(char *b, size_t n, const rcc_snapshot_t *m);
/* Controller state for the start confirmation: "IDLE . Ready" when idle and operational, else the state name. */
void hp_controller_summary(char *b, size_t n, const rcc_snapshot_t *m);

/* Brightness presets of the Settings tab and whether preset i is the highlighted one for the stored percentage
 * (an off-preset value highlights the nearest end preset only past 75 or below 35). */
extern const uint8_t hp_brightness_presets[4];
bool hp_brightness_preset_selected(uint8_t pct, int i);

/* Gauge fill in permille of full scale (0..1000) and the power in whole watts. */
typedef struct {
    int v_pm, a_pm, w_pm;
    bool power_valid;
    int power_w;
} hp_gauge_t;
void hp_gauge(const rcc_snapshot_t *m, hp_gauge_t *g);

void hp_fmt_age(char *b, size_t n, uint32_t ms);
void hp_fmt_milli(char *b, size_t n, int32_t v, bool below);
void hp_fmt_duration(char *b, size_t n, uint32_t ms);
void hp_fmt_since_boot(char *b, size_t n, uint64_t us);
void hp_session_line(char *b, size_t n, const rcc_snapshot_t *m);

typedef struct {
    const char *name;
    char sub[64];
    hp_kind_t kind;
    int stage;   /* 0..4 on the stage strip, -1 none */
} hp_state_info_t;
void hp_state_info(const rcc_snapshot_t *m, hp_state_info_t *s);

typedef struct {
    hp_bar_kind_t kind;
    hp_bar_button_t button;
    char title[48];
    char sub[72];
    hp_kind_t title_kind;   /* HP_K_ACCENT by default */
    hp_start_block_t start_block;   /* set on the full-width STOP bar when START is blocked by link or fault */
} hp_bar_spec_t;
void hp_bar_spec(const rcc_snapshot_t *m, bool is_overview, hp_bar_spec_t *b);

typedef struct {
    const char *text;       /* NULL: no banner; may point into buf */
    char buf[128];
    hp_kind_t kind;
    hp_target_t target;
    bool two_line;
} hp_banner_t;
void hp_banner(const rcc_snapshot_t *m, bool link_started, hp_banner_t *b);
