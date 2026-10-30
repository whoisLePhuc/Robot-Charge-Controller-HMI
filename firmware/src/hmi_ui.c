/*
 * HMI pages. Modern Bento UI v3.2 (Zero-Overlap, Spacious, LVGL 9.6, 480x320 landscape).
 * Follows rcc_hmi_modern_bento_ui_v3_2_zero_overlap_spacious_lvgl_9_6_480_320.html.
 * Called only under the LVGL port lock.
 */
#include "hmi_ui.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "esp_task_wdt.h"
#include "hmi_backlight.h"
#include "hmi_icons.h"
#include "hmi_settings.h"
#include "hmi_wifi.h"
#include "hmi_ota.h"
#include "lvgl.h"
#include "rcc_link.h"

LV_FONT_DECLARE(hmi_f10);
LV_FONT_DECLARE(hmi_f12);
LV_FONT_DECLARE(hmi_f12b);
LV_FONT_DECLARE(hmi_f14b);
LV_FONT_DECLARE(hmi_f16b);
LV_FONT_DECLARE(hmi_f22b);
LV_FONT_DECLARE(hmi_f28b);
LV_FONT_DECLARE(hmi_hero28);

#define F10 (&hmi_f10)
#define F12 (&hmi_f12)
#define F12B (&hmi_f12b)
#define F14B (&hmi_f14b)
#define F16B (&hmi_f16b)
#define F22B (&hmi_f22b)
#define F28B (&hmi_f28b)
#define F28H (&hmi_hero28)

/* ===== Modern Industrial Dark Slate Palette ===== */
#define BG          0x090D14   /* Deep slate-black canvas */
#define CARD        0x111722   /* Elevated Bento card surface */
#define SURFACE2    0x172030   /* Secondary inner pill / track surface */
#define LINE        0x1E293B   /* Clean 1px structural border */
#define WHITE       0xF8FAFC   /* Crisp white primary text */
#define TXT2        0xCBD5E1   /* Soft slate secondary text */
#define LABEL       0x94A3B8   /* Muted label */
#define DIM         0x64748B   /* Inactive icon / disabled label */

#define NAV_CUR     0x38BDF8   /* Calm Sky Cyan (Navigation / Active) */
#define NAV_BOX     0x0C2538   /* Muted Cyan background */

#define VOLT_COL    0xFBBF24   /* Telemetry: Volt */
#define VOLT_T      0x2A220B
#define AMP_COL     0xFB923C   /* Telemetry: Current */
#define AMP_T       0x2C1A0D
#define WATT_COL    0x38BDF8   /* Telemetry: Power */
#define WATT_T      0x0C2538

#define OK_COL      0x22C55E   /* Status OK */
#define OK_BG       0x0A2617
#define WARN_COL    0xFACC15   /* Status Warning */
#define WARN_BG     0x2B2309
#define FAULT_COL   0xF43F5E   /* Status Fault */
#define FAULT_BG    0x35101B

#define STOP_COL    0xDC2626   /* E-Stop */
#define START_COL   0x0EA5E9   /* Charge Start */
#define OFF_COL     0x94A3B8
#define OFF_BG      0x161E2B
#define HERO_COL    0xFFFFFF

#define C(x) lv_color_hex(x)
#define COUNT(a) ((int)(sizeof(a) / sizeof((a)[0])))

typedef enum { OVERVIEW, MEASURE, FAULTS, SESSION, DEVICE } page_t;
typedef enum { K_NORMAL, K_WARN, K_FAULT, K_OFF } kind_t;
typedef enum { FS_OK, FS_FAULT, FS_INHIBIT, FS_UNKNOWN } fault_state_t;
typedef enum { B_NONE, B_STOP, B_START, B_STOPPING } bar_button_t;

static page_t page;
static bool link_started;
static int device_tab = 0;
static bool dirty = true;
static lv_obj_t *screen, *content, *bar_box, *modal;
static lv_obj_t *wifi_layer;
static lv_obj_t *rail_tab[5], *rail_mark[5], *rail_badge[5], *rail_icon[5], *rail_label[5];
static lv_obj_t *hdr_dev, *hdr_pill, *hdr_dot, *hdr_text, *hdr_fault_box, *hdr_fault_icon;
static lv_obj_t *banner_box, *banner_icon, *banner_text;
static char bar_signature[200];
static int hdr_dev_w = 99;
static int banner_target = -1;

static const lv_image_dsc_t *const nav_icon[5] = {
    &hmi_icon_home_19, &hmi_icon_gauge_19, &hmi_icon_warn_19,
    &hmi_icon_list_19, &hmi_icon_chip_19
};
static const char *const nav_name[5] = {"Overview", "Measure", "Faults", "Session", "Device"};

/* ------------------------------------------------------------------ names */
static const char *const fault_names[19] = {
    "CONTROL_INTERNAL", "NVS_WRITE_FAILED", "RELAY_COMMAND_FAILED", "RELAY_FEEDBACK_CONFLICT",
    "ADC_UNAVAILABLE", "ADC_STALE", "ADC_PATTERN_INVALID", "ADC_SATURATED", "VOUT_INVALID",
    "IOUT_INVALID", "VOUT_OVERVOLTAGE", "IOUT_OVERCURRENT", "IOUT_REVERSE_CURRENT",
    "CHARGE_NOT_ESTABLISHED", "CHARGE_MAX_DURATION", "CAN_DEGRADED", "RS485_DEGRADED",
    "OPERATIONAL_UART_DEGRADED", "DIAGNOSTIC_DEGRADED"};
static const uint16_t fault_ids[19] = {
    0x0101, 0x0102, 0x0201, 0x0202, 0x0301, 0x0302, 0x0303, 0x0304, 0x0305, 0x0306,
    0x0401, 0x0402, 0x0403, 0x0501, 0x0502, 0x0601, 0x0602, 0x0603, 0x0604};
static const char *const inhibit_names[3] = {"REMOTE", "RESET", "RECOVERY"};
static const char *const top_names[7] = {"BOOT_SAFE", "SELF_TEST", "SERVICE_LOCK", "CONFIG_MODE",
                                         "OPERATIONAL", "INHIBITED", "LATCHED_FAULT"};
static const char *const op_names[9] = {"NONE", "IDLE", "VREQ_VALIDATE", "PRECHECK",
                                        "RELAY_CLOSING", "CHARGE_VERIFY", "CHARGING",
                                        "COMPLETE", "WAIT_REARM"};
static const char *const end_names[10] = {"?", "COMPLETE", "REMOTE_STOP", "RECOVERABLE_FAULT",
                                          "LATCHED_FAULT", "NOT_ESTABLISHED", "MAX_DURATION",
                                          "INTERRUPTED_RESET", "START_ABORTED", "CHARGE_LOST"};
static const char *const reset_names[7] = {"UNKNOWN", "POWER_ON", "SOFTWARE", "WATCHDOG",
                                           "PANIC", "BROWNOUT", "OTHER"};

static const char *fault_name(uint16_t id)
{
    for (int i = 0; i < 19; ++i) if (fault_ids[i] == id) return fault_names[i];
    return id ? "UNRECOGNIZED" : "none";
}
static const char *state_name(uint8_t top, uint8_t op)
{
    if (top == 4) return op < 9 ? op_names[op] : "OPERATIONAL";
    return top < 7 ? top_names[top] : "UNKNOWN";
}
static void mask_text(char *out, size_t cap, uint32_t mask, const char *const *names, int count)
{
    out[0] = '\0';
    if (!mask) { snprintf(out, cap, "none"); return; }
    for (int b = 0; b < 32; ++b) {
        if (!(mask & (1u << b))) continue;
        size_t used = strlen(out);
        if (b < count) snprintf(out + used, cap - used, "%s%s", used ? "+" : "", names[b]);
        else snprintf(out + used, cap - used, "%sbit %d", used ? "+" : "", b);
    }
}

/* ------------------------------------------------------------------ derived state */
static bool fresh(const rcc_snapshot_t *m) {
    return m->online && m->status_valid && m->status_age_ms < 1500;
}
static bool v_valid(const rcc_snapshot_t *m) {
    return m->online && m->measurement_valid && m->measurement_age_ms <= 1500 &&
           (m->vout_status & 0x3f) == 0x3f;
}
static bool v_floor(const rcc_snapshot_t *m) { return v_valid(m) && (m->vout_status & 0x40); }
static bool i_valid(const rcc_snapshot_t *m) {
    return m->online && m->measurement_valid && m->measurement_age_ms <= 1500 &&
           (m->iout_status & 0x3f) == 0x3f;
}
static fault_state_t fault_state(const rcc_snapshot_t *m)
{
    if (!m->online || !m->faults_valid) return FS_UNKNOWN;
    if (m->active_fault_mask || m->primary_fault) return FS_FAULT;
    if (m->inhibit_mask || (fresh(m) && m->top_state == 5)) return FS_INHIBIT;
    return FS_OK;
}
static bool start_allowed(const rcc_snapshot_t *m)
{
    return fresh(m) && (!m->simulated_measurements || m->simulated_ready) &&
           m->command_state != RCC_COMMAND_SENDING &&
           m->command_state != RCC_COMMAND_ACCEPTED &&
           (m->top_state == 5 || (m->top_state == 4 && m->operational_state == 1));
}
static bool no_authority(const rcc_snapshot_t *m)
{
    return m->command_state == RCC_COMMAND_REJECTED && m->command_reason == 0x0005;
}
static void fmt_age(char *b, size_t n, uint32_t ms)
{
    if (ms == UINT32_MAX) snprintf(b, n, "\xE2\x80\x94");
    else if (ms < 10000u) snprintf(b, n, "%u.%u s", (unsigned)(ms / 1000u), (unsigned)(ms % 1000u / 100u));
    else if (ms < 600000u) snprintf(b, n, "%u s", (unsigned)(ms / 1000u));
    else snprintf(b, n, "%u min", (unsigned)(ms / 60000u));
}
static void fmt_milli(char *b, size_t n, int32_t v, bool below)
{
    const bool neg = v < 0;
    const uint32_t a = neg ? (uint32_t)(-(int64_t)v) : (uint32_t)v;
    snprintf(b, n, "%s%s%u.%u", below ? "\xE2\x89\xA4 " : "", neg ? "\xE2\x88\x92" : "",
             (unsigned)(a / 1000u), (unsigned)(a % 1000u / 100u));
}

typedef struct {
    const char *name;
    char sub[64];
    kind_t kind;
    int stage;   /* 0..4 on the stage strip, -1 none */
} state_info_t;

static void state_info(const rcc_snapshot_t *m, state_info_t *s)
{
    char t[40];
    memset(s, 0, sizeof(*s));
    s->stage = -1;
    if (!fresh(m)) {
        s->name = "UNKNOWN"; snprintf(s->sub, sizeof(s->sub), "STATE NOT CONFIRMED");
        s->kind = K_OFF; return;
    }
    s->name = state_name(m->top_state, m->operational_state);
    switch (m->top_state) {
    case 0: case 1: snprintf(s->sub, sizeof(s->sub), "STARTING UP"); break;
    case 2: snprintf(s->sub, sizeof(s->sub), "CONFIG / CALIBRATION NEEDED"); s->kind = K_WARN; break;
    case 3: snprintf(s->sub, sizeof(s->sub), "SERVICE CONFIGURATION"); break;
    case 5:
        mask_text(t, sizeof(t), m->inhibit_mask, inhibit_names, 3);
        for (char *c = t; *c; ++c) if (*c >= 'a' && *c <= 'z') *c -= 32;
        if (m->primary_fault) snprintf(s->sub, sizeof(s->sub), "%s \xC2\xB7 FAULT %s", t, fault_name(m->primary_fault));
        else snprintf(s->sub, sizeof(s->sub), "%s", m->inhibit_mask ? t : "INHIBITED");
        s->kind = K_WARN; break;
    case 6:
        snprintf(s->sub, sizeof(s->sub), "FAULT 0x%04X", (unsigned)m->primary_fault);
        s->kind = K_FAULT; break;
    case 4: {
        static const char *const subs[9] = {"OPERATIONAL", "OPERATIONAL \xC2\xB7 READY",
            "VALIDATING REQUEST", "PRE-CHECKING", "CLOSING RELAY", "VERIFYING CURRENT",
            "OPERATIONAL \xC2\xB7 SESSION ACTIVE", "CHARGE COMPLETE", "WAITING REARM"};
        const uint8_t op = m->operational_state < 9 ? m->operational_state : 0;
        snprintf(s->sub, sizeof(s->sub), "%s", subs[op]);
        if (op == 6 && m->session_id) snprintf(s->sub, sizeof(s->sub), "OPERATIONAL \xC2\xB7 SESSION #%u", (unsigned)m->session_id);
        static const int stage[9] = {-1, 0, 1, 1, 2, 3, 4, 4, -1};
        s->stage = stage[op];
        if (op == 6 && !m->charging_established) {
            s->name = "NOT_CONFIRMED"; s->kind = K_WARN; s->stage = 3;
            snprintf(s->sub, sizeof(s->sub), "CHARGING NOT ESTABLISHED");
        }
        break; }
    default: break;
    }
}
static uint32_t kind_color(kind_t k) { return k == K_WARN ? WARN_COL : k == K_FAULT ? FAULT_COL : k == K_OFF ? OFF_COL : WHITE; }

/* ------------------------------------------------------------------ drawing primitives */
/* Cards have a 1 px border and a -1 px padding, so child coordinates are relative to the
 * card's outer top-left corner (the same numbers the HTML concept uses). */
static lv_obj_t *obj(lv_obj_t *p, int x, int y, int w, int h)
{
    lv_obj_t *o = lv_obj_create(p);
    lv_obj_remove_style_all(o);
    lv_obj_set_pos(o, x, y);
    lv_obj_set_size(o, w, h);
    lv_obj_set_clickable(o, false);
    lv_obj_set_scrollable(o, false);
    return o;
}
static lv_obj_t *box(lv_obj_t *p, int x, int y, int w, int h, uint32_t bg, int radius)
{
    lv_obj_t *o = obj(p, x, y, w, h);
    lv_obj_set_style_bg_color(o, C(bg), 0);
    lv_obj_set_style_bg_opa(o, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(o, radius, 0);
    return o;
}
static lv_obj_t *card_box(lv_obj_t *p, int x, int y, int w, int h, uint32_t bg, int radius, uint32_t border_col)
{
    lv_obj_t *o = box(p, x, y, w, h, bg, radius);
    lv_obj_set_style_border_width(o, 1, 0);
    lv_obj_set_style_border_color(o, C(border_col), 0);
    lv_obj_set_style_pad_all(o, -1, 0);
    return o;
}

/* Vertical scroll areas: a transparent clip window inside a card. Scroll offsets survive the 1 s
 * rebuild (kept per slot) and are cleared whenever the page or tab changes. */
enum { SCR_FAULTS, SCR_INHIBITS, SCR_EVENTS, SCR_WIFI, SCR_SLOTS };
static int scroll_pos[SCR_SLOTS];

static void scroll_save_cb(lv_event_t *e)
{
    const int slot = (int)(uintptr_t)lv_event_get_user_data(e);
    scroll_pos[slot] = (int)lv_obj_get_scroll_y(lv_event_get_target_obj(e));
}
static lv_obj_t *scroll_area(lv_obj_t *p, int x, int y, int w, int h, int slot)
{
    lv_obj_t *a = obj(p, x, y, w, h);
    lv_obj_set_scrollable(a, true);
    lv_obj_set_clickable(a, true);
    lv_obj_set_scroll_dir(a, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(a, LV_SCROLLBAR_MODE_AUTO);
    lv_obj_set_style_bg_opa(a, LV_OPA_COVER, LV_PART_SCROLLBAR);
    lv_obj_set_style_bg_color(a, C(DIM), LV_PART_SCROLLBAR);
    lv_obj_set_style_width(a, 3, LV_PART_SCROLLBAR);
    lv_obj_set_style_radius(a, 2, LV_PART_SCROLLBAR);
    lv_obj_set_style_pad_right(a, 1, LV_PART_SCROLLBAR);
    lv_obj_add_event_cb(a, scroll_save_cb, LV_EVENT_SCROLL, (void *)(uintptr_t)slot);
    return a;
}
static void scroll_restore(lv_obj_t *a, int slot)
{
    lv_obj_update_layout(a);
    lv_obj_scroll_to_y(a, scroll_pos[slot], LV_ANIM_OFF);
}
static int lh(const lv_font_t *f) { return (int)lv_font_get_line_height(f); }
static void spacing(lv_obj_t *o, int px) { lv_obj_set_style_text_letter_space(o, px, 0); }
/* Auto-sized single-line label at (x, y). */
static lv_obj_t *text(lv_obj_t *p, const char *s, int x, int y, const lv_font_t *f, uint32_t c)
{
    lv_obj_t *o = lv_label_create(p);
    lv_label_set_text(o, s);
    lv_obj_set_pos(o, x, y);
    lv_obj_set_style_text_font(o, f, 0);
    lv_obj_set_style_text_color(o, C(c), 0);
    return o;
}
/* Fixed-width single-line label, vertically centred in a row of height h; long text ends in "...". */
static lv_obj_t *text_in(lv_obj_t *p, const char *s, int x, int y, int w, int h,
                         const lv_font_t *f, uint32_t c, lv_text_align_t align)
{
    lv_obj_t *o = lv_label_create(p);
    lv_label_set_long_mode(o, LV_LABEL_LONG_MODE_DOTS);
    lv_label_set_text(o, s);
    lv_obj_set_size(o, w, lh(f));
    lv_obj_set_style_text_align(o, align, 0);
    lv_obj_set_style_text_font(o, f, 0);
    lv_obj_set_style_text_color(o, C(c), 0);
    lv_obj_set_pos(o, x, y + (h - lh(f)) / 2);
    return o;
}
static int width_of(lv_obj_t *label) { lv_obj_update_layout(label); return lv_obj_get_width(label); }
static lv_obj_t *icon(lv_obj_t *p, const lv_image_dsc_t *d, int x, int y, uint32_t color)
{
    lv_obj_t *i = lv_image_create(p);
    lv_image_set_src(i, d);
    lv_obj_set_pos(i, x, y);
    lv_obj_set_style_image_recolor(i, C(color), 0);
    lv_obj_set_style_image_recolor_opa(i, LV_OPA_COVER, 0);
    lv_obj_set_clickable(i, false);
    return i;
}
/* Icon centred on a row of height h. */
static lv_obj_t *icon_row(lv_obj_t *p, const lv_image_dsc_t *d, int x, int y, int h, uint32_t color)
{
    return icon(p, d, x, y + (h - (int)d->header.h) / 2, color);
}
static lv_obj_t *icon_c(lv_obj_t *p, const lv_image_dsc_t *d, int cx, int cy, uint32_t color)
{
    return icon(p, d, cx - (int)d->header.w / 2, cy - (int)d->header.h / 2, color);
}
static void spin_anim(void *o, int32_t v) { lv_image_set_rotation(o, v); }
static void hline(lv_obj_t *p, int x, int y, int w, uint32_t c) { box(p, x, y, w, 1, c, 0); }

static lv_obj_t *button(lv_obj_t *p, int x, int y, int w, int h, uint32_t bg, int radius,
                        lv_event_cb_t cb, void *user)
{
    lv_obj_t *o = lv_button_create(p);
    lv_obj_remove_style_all(o);
    lv_obj_set_pos(o, x, y);
    lv_obj_set_size(o, w, h);
    lv_obj_set_style_bg_color(o, C(bg), 0);
    lv_obj_set_style_bg_opa(o, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(o, radius, 0);
    lv_obj_set_scrollable(o, false);
    lv_obj_set_style_bg_opa(o, LV_OPA_70, LV_STATE_PRESSED);
    if (cb) lv_obj_add_event_cb(o, cb, LV_EVENT_CLICKED, user);
    return o;
}
/* Pill label (17 px high). right_anchor: x is the right edge, otherwise the left edge. Returns width. */
static int chip_at(lv_obj_t *p, int x, int y, bool right_anchor, const char *s, uint32_t fg, uint32_t bg)
{
    lv_obj_t *l = text(p, s, 0, 0, F10, fg);
    const int w = width_of(l) + 14;
    const int x0 = right_anchor ? x - w : x;
    box(p, x0, y, w, 17, bg, 6);
    lv_obj_move_foreground(l);
    lv_obj_set_pos(l, x0 + 7, y + (17 - lh(F10)) / 2);
    return w;
}
static void chip(lv_obj_t *p, int right_x, int y, const char *s, uint32_t fg, uint32_t bg)
{
    (void)chip_at(p, right_x, y, true, s, fg, bg);
}
/* "label ............ value" row of height h. */
static void kv(lv_obj_t *p, int x, int y, int w, int h, const char *k, const char *v, uint32_t vc)
{
    lv_obj_t *vl = text(p, v, 0, 0, F12B, vc);
    int vw = width_of(vl);
    lv_obj_delete(vl);
    lv_obj_t *kl = text_in(p, k, x, y, w, h, F12, LABEL, LV_TEXT_ALIGN_LEFT);
    (void)kl;
    if (vw > w - 4) vw = w - 4;
    text_in(p, v, x + w - vw - 1, y, vw + 2, h, F12B, vc, LV_TEXT_ALIGN_RIGHT);
}
/* Section title row (16 px) with an optional right-aligned value. */
static void section(lv_obj_t *p, int x, int y, int w, const char *left, const char *right, uint32_t rc)
{
    lv_obj_t *l = text(p, left, x, y + (16 - lh(F10)) / 2, F10, LABEL);
    spacing(l, 1);
    if (right) {
        const int lw = width_of(l) + 8;
        text_in(p, right, x + lw, y, w - lw, 16, F10, rc, LV_TEXT_ALIGN_RIGHT);
    }
}

/* ------------------------------------------------------------------ Overview */
static const char *fault_short(uint16_t id)
{
    static const char *const s[19] = {
        "INTERNAL", "NVS WRITE", "RELAY CMD", "RELAY FB", "ADC N/A", "ADC STALE", "ADC PATT.",
        "ADC SAT.", "VOUT BAD", "IOUT BAD", "OVERVOLT", "OVERCURR.", "REV CURR.", "NOT ESTAB",
        "MAX TIME", "CAN DEGR.", "RS485 DEG", "UART DEGR", "DIAG DEGR"};
    for (int i = 0; i < 19; ++i) if (fault_ids[i] == id) return s[i];
    return "FAULT";
}
static void session_line(char *b, size_t n, const rcc_snapshot_t *m)
{
    const bool active = fresh(m) && m->top_state == 4 && m->operational_state >= 2 && m->operational_state <= 6;
    if (!fresh(m)) {
        if (m->session_id) snprintf(b, n, "#%" PRIu32 "? \xC2\xB7 Unconfirmed", m->session_id);
        else snprintf(b, n, "Unconfirmed");
    } else if (active && m->session_id) {
        snprintf(b, n, "#%" PRIu32, m->session_id);
    } else if (m->end_valid) {
        snprintf(b, n, "#%" PRIu32 " %s", m->end_session_id, m->end_reason == 1 ? "done" : "aborted");
    } else if (m->session_id) {
        snprintf(b, n, "#%" PRIu32, m->session_id);
    } else {
        snprintf(b, n, "Ready");
    }
}

static void goto_page(page_t n);
static void wifi_open(void);
/* Tap targets on the Overview: user data is a page, or 100 for Device > Link. */
static void nav_to_cb(lv_event_t *e)
{
    const intptr_t v = (intptr_t)lv_event_get_user_data(e);
    if (v == 100) { device_tab = 1; goto_page(DEVICE); }
    else goto_page((page_t)v);
}
static void make_tappable(lv_obj_t *o, intptr_t target)
{
    lv_obj_set_clickable(o, true);
    lv_obj_add_event_cb(o, nav_to_cb, LV_EVENT_CLICKED, (void *)target);
}

static void tel_card(lv_obj_t *p, int x, int y, int w, int h, const char *label, const char *val,
                     const char *unit, int permille, const char *maxs, uint32_t col,
                     const lv_image_dsc_t *ic, bool stale)
{
    lv_obj_t *c = card_box(p, x, y, w, h, CARD, 10, LINE);
    make_tappable(c, MEASURE);
    lv_obj_set_style_border_color(c, C(0x334155), LV_STATE_PRESSED);
    const int ix = 9, iw = w - 18, T = 8, B = h - 8;
    const int gap = (B - T - (13 + 27 + 5 + 13)) / 3;
    text(c, label, ix, T + (13 - lh(F10)) / 2, F10, LABEL);
    icon(c, ic, ix + iw - (int)ic->header.w, T, stale ? DIM : col);

    const int vy = T + 13 + gap + 1;
    lv_obj_t *vl = text(c, val, ix, 0, F22B, stale ? DIM : WHITE);
    const int vw = width_of(vl);
    lv_obj_set_pos(vl, ix, vy + (27 - lh(F22B)) / 2);
    const bool dash = (unsigned char)val[0] == 0xE2 && (unsigned char)val[1] == 0x80;
    if (!dash) text(c, unit, ix + vw + 2, vy + 7, F12B, LABEL);

    const int by = T + 13 + gap + 27 + gap;
    box(c, ix, by, iw, 5, SURFACE2, 3);
    int fw = iw * permille / 1000;
    if (fw > iw) fw = iw;
    if (!stale && fw > 0) box(c, ix, by, fw < 3 ? 3 : fw, 5, col, 3);
    lv_obj_t *tick = box(c, ix + iw * 8 / 10, by, 2, 5, 0xFFFFFF, 0);
    lv_obj_set_style_bg_opa(tick, 115, 0);

    char sub[24];
    if (stale) snprintf(sub, sizeof(sub), "STALE DATA");
    else snprintf(sub, sizeof(sub), "%d%%/%s", (permille + 5) / 10, maxs);
    text_in(c, sub, ix, B - 13, iw, 13, F10, stale ? DIM : DIM, LV_TEXT_ALIGN_LEFT);
}

static void page_overview(lv_obj_t *p, const rcc_snapshot_t *m, int H, int reason)
{
    state_info_t s; state_info(m, &s);
    const uint32_t kc = s.kind == K_NORMAL ? HERO_COL : kind_color(s.kind);

    /* Hero card with the connected stepper. */
    const int hero_h = s.stage >= 0 ? 74 : 72;
    lv_obj_t *hero = card_box(p, 0, 0, 408, hero_h, CARD, 10, LINE);
    lv_obj_t *st = text(hero, s.name, 0, 0, F28H, kc);
    const int sw = width_of(st);
    lv_obj_set_pos(st, 13, 10 + (30 - lh(F28H)) / 2);
    text_in(hero, s.sub, 13 + sw + 8, 22, 374 - sw, 14, F10, s.kind == K_NORMAL ? LABEL : kc, LV_TEXT_ALIGN_RIGHT);
    hline(hero, 13, 42, 382, LINE);

    static const char *const stg[5] = {"IDLE", "PRE", "RELAY", "VERIFY", "CHG"};
    lv_obj_t *tl[5]; int tw[5], sum = 0;
    for (int i = 0; i < 5; ++i) {
        const bool cur = s.stage == i, done = s.stage > i;
        tl[i] = text(hero, stg[i], 0, 0, F10, cur ? NAV_CUR : done ? TXT2 : DIM);
        tw[i] = width_of(tl[i]);
        sum += tw[i] + 24 + (cur ? 2 : 0);
    }
    const int conn = (382 - sum) / 4;
    int x = 13;
    for (int i = 0; i < 5; ++i) {
        const bool cur = s.stage == i, done = s.stage > i;
        const int nw = tw[i] + 24 + (cur ? 2 : 0), o = cur ? 1 : 0;
        if (cur) card_box(hero, x, 48, nw, 19, NAV_BOX, 5, 0x1E4963);
        box(hero, x + 7 + o, 54, 6, 6, cur ? NAV_CUR : done ? OK_COL : DIM, 3);
        lv_obj_move_foreground(tl[i]);
        lv_obj_set_pos(tl[i], x + 17 + o, 57 - lh(F10) / 2);
        if (i < 4) hline(hero, x + nw + 3, 57, conn - 6, done ? 0x1E4963 : LINE);
        x += nw + conn;
    }

    /* System card. */
    const int gy = hero_h + 6, gh = H - gy;
    lv_obj_t *sys = card_box(p, 0, gy, 156, gh, CARD, 10, LINE);
    const int gap = (gh - 14 - 88) / 3;
    char b[48], t[24];
    const fault_state_t fs = fault_state(m);
    const bool online = m->online;
    struct { const lv_image_dsc_t *ic; uint32_t ic_col; const char *k; char v[40]; uint32_t vc; } row[4];
    row[0].ic = &hmi_icon_link_13; row[0].ic_col = online ? OK_COL : OFF_COL; row[0].k = "UART";
    fmt_age(row[0].v, sizeof(row[0].v), m->reply_age_ms); row[0].vc = online ? WHITE : OFF_COL;
    row[1].ic = fs == FS_OK ? &hmi_icon_check_12 : &hmi_icon_warn_14; row[1].k = "Fault";
    row[1].ic_col = fs == FS_FAULT ? FAULT_COL : fs == FS_INHIBIT ? WARN_COL : fs == FS_UNKNOWN ? OFF_COL : OK_COL;
    if (fs == FS_FAULT) snprintf(row[1].v, sizeof(row[1].v), "%s", fault_short(m->primary_fault));
    else if (fs == FS_INHIBIT) snprintf(row[1].v, sizeof(row[1].v), "Inhibited");
    else if (fs == FS_UNKNOWN) snprintf(row[1].v, sizeof(row[1].v), "Unreadable");
    else snprintf(row[1].v, sizeof(row[1].v), "No faults");
    row[1].vc = fs == FS_OK ? TXT2 : row[1].ic_col;
    row[2].ic = &hmi_icon_relay_13; row[2].ic_col = LABEL; row[2].k = "Relay cmd";
    snprintf(row[2].v, sizeof(row[2].v), "%s", fresh(m) ? (m->relay_command ? "CLOSED" : "OPEN") : "\xE2\x80\x94");
    row[2].vc = WHITE;
    row[3].ic = &hmi_icon_clock_13; row[3].ic_col = LABEL; row[3].k = "Sess";
    session_line(row[3].v, sizeof(row[3].v), m); row[3].vc = TXT2;
    if (m->simulated_measurements) row[3].k = "Session (SIM V/I)";
    (void)b; (void)t;
    for (int i = 0; i < 4; ++i) {
        const int ry = 7 + i * (22 + gap);
        if (i != 2) {
            lv_obj_t *hit = obj(sys, 2, ry - 3, 152, 28);
            make_tappable(hit, i == 0 ? 100 : i == 1 ? FAULTS : SESSION);
            lv_obj_set_style_bg_color(hit, C(SURFACE2), LV_STATE_PRESSED);
            lv_obj_set_style_bg_opa(hit, LV_OPA_COVER, LV_STATE_PRESSED);
            lv_obj_set_style_radius(hit, 6, 0);
        }
        icon_row(sys, row[i].ic, 11, ry, 22, row[i].ic_col);
        lv_obj_t *kl = text(sys, row[i].k, 0, 0, F10, LABEL);
        const int kw = width_of(kl);
        lv_obj_set_pos(kl, 29, ry + (22 - lh(F10)) / 2);
        const int vx = 29 + kw + 6;
        text_in(sys, row[i].v, vx, ry, 145 - vx, 22, F12B, row[i].vc, LV_TEXT_ALIGN_RIGHT);
    }

    /* Telemetry cards. */
    const bool stale = !(m->online && m->measurement_valid && m->measurement_age_ms <= 1500);
    char vs[16] = "\xE2\x80\x94", as[16] = "\xE2\x80\x94", ws[16] = "\xE2\x80\x94";
    int pv = 0, pa = 0, pw = 0;
    if (v_valid(m)) {
        fmt_milli(vs, sizeof(vs), (int32_t)m->vout_mv, v_floor(m));
        if (v_floor(m)) memmove(vs + 3, vs + 4, strlen(vs + 4) + 1);   /* "\xE2\x89\xA4 3.4" -> "\xE2\x89\xA43.4" */
        pv = v_floor(m) ? 0 : (int)(m->vout_mv / 60u);
    }
    if (i_valid(m)) {
        fmt_milli(as, sizeof(as), m->iout_ma, false);
        pa = m->iout_ma > 0 ? (int)(m->iout_ma / 10) : 0;
    }
    if (v_valid(m) && !v_floor(m) && i_valid(m)) {
        const int64_t mw = (int64_t)m->vout_mv * m->iout_ma / 1000;
        snprintf(ws, sizeof(ws), "%d", (int)((mw + 500) / 1000));
        pw = mw > 0 ? (int)(mw / 600) : 0;
    }
    if (pv > 1000) pv = 1000;
    if (pa > 1000) pa = 1000;
    if (pw > 1000) pw = 1000;
    tel_card(p, 164, gy, 77, gh, "VOLT", vs, "V", pv, "60V", VOLT_COL, &hmi_icon_bolt_13, stale);
    tel_card(p, 247, gy, 77, gh, "CURR.", as, "A", pa, "10A", AMP_COL, &hmi_icon_wave_13, stale);
    tel_card(p, 331, gy, 77, gh, "POWER", ws, "W", pw, "600W", WATT_COL, &hmi_icon_power_13, stale);

    if (reason) {
        const uint32_t pc = WARN_COL;
        lv_obj_t *r = card_box(p, 164, H - 2 - 19, 244, 19, WARN_BG, 6, pc);
        text_in(r, reason == 1 ? "Start blocked: UART link stale" : "Start blocked: latched fault active",
                0, 0, 244, 19, F10, pc, LV_TEXT_ALIGN_CENTER);
    }
}

/* ------------------------------------------------------------------ Measure */
static void channel_card(lv_obj_t *p, int x, int y, int w, int h, const char *title, const char *lim, uint32_t hue,
                         bool valid, bool floor_bit, bool stale, int32_t value, int32_t filtered, const char *unit)
{
    lv_obj_t *c = card_box(p, x, y, w, h, CARD, 10, LINE);
    const char *st = stale ? "STALE" : valid ? (floor_bit ? "BELOW FLOOR" : "VALID") : "INVALID";
    const uint32_t sc = stale ? OFF_COL : valid ? (floor_bit ? WARN_COL : OK_COL) : FAULT_COL;
    lv_obj_t *tt = text(c, title, 13, 7 + (16 - lh(F10)) / 2, F10, LABEL);
    spacing(tt, 1);
    const int tw = width_of(tt);
    text(c, lim, 13 + tw + 6, 7 + (16 - lh(F10)) / 2, F10, DIM);
    chip(c, w - 13, 7, st, sc, SURFACE2);

    char vb[32], fb[56];
    if (valid) fmt_milli(vb, sizeof(vb), value, floor_bit);
    else snprintf(vb, sizeof(vb), "\xE2\x80\x94");
    lv_obj_t *vl = text(c, vb, 13, 0, F28B, valid ? hue : DIM);
    const int vw = width_of(vl);
    lv_obj_set_pos(vl, 13, 23 + (33 - lh(F28B)) / 2);
    if (valid) text(c, unit, 13 + vw + 4, 33, F14B, LABEL);

    if (valid) {
        char t[24]; fmt_milli(t, sizeof(t), filtered, floor_bit);
        snprintf(fb, sizeof(fb), "Filtered %s %s", t, unit);
    } else {
        snprintf(fb, sizeof(fb), "%s", stale ? "Last value is stale" : "Not valid");
    }
    text_in(c, fb, 13, 56, w - 26, 13, F10, TXT2, LV_TEXT_ALIGN_LEFT);
}

static void page_measure(lv_obj_t *p, const rcc_snapshot_t *m, int H)
{
    const bool stale = !(m->online && m->measurement_valid && m->measurement_age_ms <= 1500);
    channel_card(p, 0, 0, 200, 76, "VOUT", "\xC2\xB7 \xE2\x89\xA4" "60V", VOLT_COL, v_valid(m), v_floor(m), stale,
                 (int32_t)m->vout_mv, (int32_t)m->vout_filtered_mv, "V");
    channel_card(p, 208, 0, 200, 76, "IOUT", "\xC2\xB7 \xC2\xB1" "10A", AMP_COL, i_valid(m), false, stale,
                 m->iout_ma, m->iout_filtered_ma, "A");

    const int ch = H - 82;
    lv_obj_t *mx = card_box(p, 0, 82, 408, ch, CARD, 10, LINE);
    char seq[64], age[24];
    if (m->online && m->measurement_valid) {
        fmt_age(age, sizeof(age), m->measurement_age_ms);
        snprintf(seq, sizeof(seq), "Seq #%" PRIu32 " \xC2\xB7 Age %" PRIu32 " ms \xC2\xB7 Held %s",
                 m->measurement_sequence, m->measurement_sample_age_ms, age);
    } else {
        fmt_age(age, sizeof(age), m->measurement_age_ms);
        snprintf(seq, sizeof(seq), "Seq #%" PRIu32 " \xC2\xB7 Stale (%s)", m->measurement_sequence, age);
    }
    section(mx, 13, 7, 382, "ADC QUALITY MATRIX", seq, TXT2);

    static const char *const hdr[7] = {"PRES", "FRESH", "CAL", "RANGE", "SAT", "PLAUS", "FLOOR"};
    const int my = ch - 68;
    const int c0 = 13 + 46;
    const int cw6 = (382 - 96) * 100 / 6;           /* 1fr in hundredths of a pixel */
    for (int i = 0; i < 7; ++i) {
        const int cx = i < 6 ? c0 + (cw6 * i) / 100 : c0 + (cw6 * 6) / 100 + 3;
        const int w = i < 6 ? cw6 / 100 : 47;
        text_in(mx, hdr[i], cx, my, w, 15, F10, LABEL, LV_TEXT_ALIGN_CENTER);
    }
    box(mx, c0 + (cw6 * 6) / 100, my, 1, 61, LINE, 0);

    const bool have = m->online && m->measurement_valid;
    for (int r = 0; r < 2; ++r) {
        const uint16_t bits = r ? m->iout_status : m->vout_status;
        const int ry = my + 18 + r * 23;
        text_in(mx, r ? "IOUT" : "VOUT", 13, ry, 46, 20, F12B, r ? AMP_COL : VOLT_COL, LV_TEXT_ALIGN_LEFT);
        for (int i = 0; i < 7; ++i) {
            const int cx = i < 6 ? c0 + (cw6 * i) / 100 : c0 + (cw6 * 6) / 100 + 3;
            const int w = i < 6 ? cw6 / 100 : 47;
            if (!have) { text_in(mx, "\xE2\x80\x94", cx, ry, w, 20, F10, DIM, LV_TEXT_ALIGN_CENTER); continue; }
            const bool set = (bits >> i) & 1u;
            if (i == 6) {
                if (set) chip(mx, cx + w / 2 + 19, ry + 1, "LOW", WARN_COL, WARN_BG);
                else text_in(mx, "-", cx, ry, w, 20, F10, DIM, LV_TEXT_ALIGN_CENTER);
            } else {
                box(mx, cx + (w - 18) / 2, ry + 2, 18, 16, set ? OK_BG : FAULT_BG, 4);
                icon_c(mx, set ? &hmi_icon_check_11 : &hmi_icon_cross_10, cx + w / 2, ry + 10, set ? OK_COL : FAULT_COL);
            }
        }
    }
}

/* ------------------------------------------------------------------ Faults */
static const char *fault_hint(uint16_t id)
{
    if (id >= 0x0301 && id <= 0x0306) return "check the current-sense circuit";
    if (id >= 0x0401 && id <= 0x0403) return "check the load and the current limit";
    if (id >= 0x0201 && id <= 0x0202) return "check the relay and its driver";
    if (id >= 0x0601 && id <= 0x0604) return "check the controller communication links";
    return "check the controller service log";
}

static void mask_list(lv_obj_t *p, int x, int y, int w, int h, const char *title, uint32_t mask,
                      const char *const *names, int count, int slot)
{
    lv_obj_t *c = card_box(p, x, y, w, h, CARD, 10, LINE);
    char hs[16]; snprintf(hs, sizeof(hs), "0x%04" PRIX32, mask);
    section(c, 13, 7, w - 26, title, hs, LABEL);
    int n = 0;
    for (int b = 0; b < 32; ++b) if (mask & (1u << b)) ++n;
    if (!n) { text_in(c, "None (0x0)", 13, 23, w - 26, 18, F12, LABEL, LV_TEXT_ALIGN_LEFT); return; }
    lv_obj_t *a = scroll_area(c, 0, 23, w, h - 23 - 3, slot);
    int r = 0;
    for (int b = 0; b < 32; ++b) {
        if (!(mask & (1u << b))) continue;
        const int ry = r * 18;
        char nm[40], bl[8];
        if (b < count) snprintf(nm, sizeof(nm), "%s", names[b]); else snprintf(nm, sizeof(nm), "Unrecognized (bit %d)", b);
        snprintf(bl, sizeof(bl), "b%d", b);
        text_in(a, nm, 13, ry, w - 26 - 24, 18, F10, WHITE, LV_TEXT_ALIGN_LEFT);
        text_in(a, bl, w - 13 - 24, ry, 24, 18, F10, LABEL, LV_TEXT_ALIGN_RIGHT);
        ++r;
    }
    scroll_restore(a, slot);
}

static void page_faults(lv_obj_t *p, const rcc_snapshot_t *m, int H)
{
    const fault_state_t fs = fault_state(m);
    static const char *const title[4] = {"All Systems Nominal \xE2\x80\x94 No Faults", "Active Fault Condition",
                                         "Charging Inhibited", "Fault State Unreadable"};
    const uint32_t tc = fs == FS_OK ? OK_COL : fs == FS_FAULT ? FAULT_COL : fs == FS_INHIBIT ? WARN_COL : OFF_COL;
    const uint32_t bg = fs == FS_FAULT ? FAULT_BG : fs == FS_INHIBIT ? WARN_BG : fs == FS_UNKNOWN ? OFF_BG : CARD;
    const uint32_t bd = fs == FS_OK ? LINE : tc;
    lv_obj_t *sum = card_box(p, 0, 0, 408, 40, bg, 10, bd);
    icon_row(sum, fs == FS_OK ? &hmi_icon_checkc_18 : &hmi_icon_warn_18, 13, 0, 40, tc);
    text_in(sum, title[fs], 41, 0, 250, 40, F14B, fs == FS_OK ? WHITE : tc, LV_TEXT_ALIGN_LEFT);
    char rs[40], at[24];
    if (m->online && m->faults_valid) { fmt_age(at, sizeof(at), m->faults_age_ms); snprintf(rs, sizeof(rs), "Read %s ago", at); }
    else snprintf(rs, sizeof(rs), "Not read");
    text_in(sum, rs, 290, 0, 105, 40, F10, LABEL, LV_TEXT_ALIGN_RIGHT);

    if (fs == FS_OK || fs == FS_UNKNOWN) {
        const int ch = H - 46, ih = ch - 14;
        const int gap = (ih - 16 - 17 - 13) / 2;
        for (int k = 0; k < 2; ++k) {
            lv_obj_t *c = card_box(p, k * 208, 46, 200, ch, CARD, 10, LINE);
            section(c, 13, 7, 174, k ? "INHIBIT REGISTER" : "FAULT REGISTER", fs == FS_UNKNOWN ? "\xE2\x80\x94" : "0x0000", LABEL);
            text_in(c, fs == FS_UNKNOWN ? "Unreadable" : "0 active bits", 13, 7 + 16 + gap, 174, 17, F14B,
                    fs == FS_UNKNOWN ? DIM : OK_COL, LV_TEXT_ALIGN_LEFT);
            text_in(c, fs == FS_UNKNOWN ? "Link offline" : (k ? "No inhibit bits set" : "No fault bits set"),
                    13, ch - 7 - 13, 174, 13, F10, LABEL, LV_TEXT_ALIGN_LEFT);
        }
        return;
    }
    int y = 46;
    const bool pri = m->primary_fault || m->inhibit_mask;
    if (pri) {
        const uint32_t pc = m->primary_fault ? FAULT_COL : WARN_COL;
        lv_obj_t *c = card_box(p, 0, y, 408, 44, m->primary_fault ? FAULT_BG : WARN_BG, 10, pc);
        char nm[64], id[12], act[80], im[40];
        if (m->primary_fault) {
            snprintf(nm, sizeof(nm), "PRIMARY \xC2\xB7 %s", fault_name(m->primary_fault));
            snprintf(id, sizeof(id), "0x%04X", (unsigned)m->primary_fault);
            snprintf(act, sizeof(act), "Action: %s", fault_hint(m->primary_fault));
        } else {
            mask_text(im, sizeof(im), m->inhibit_mask, inhibit_names, 3);
            snprintf(nm, sizeof(nm), "INHIBIT \xC2\xB7 %s", im);
            id[0] = '\0';
            snprintf(act, sizeof(act), "START may clear it; the controller decides");
        }
        text_in(c, nm, 13, 6, 300, 18, F12B, pc, LV_TEXT_ALIGN_LEFT);
        text_in(c, id, 320, 6, 75, 18, F12, LABEL, LV_TEXT_ALIGN_RIGHT);
        text_in(c, act, 13, 23, 382, 14, F10, TXT2, LV_TEXT_ALIGN_LEFT);
        y += 50;
    }
    mask_list(p, 0, y, 200, H - y, "FAULTS", m->active_fault_mask, fault_names, 19, SCR_FAULTS);
    mask_list(p, 208, y, 200, H - y, "INHIBITS", m->inhibit_mask, inhibit_names, 3, SCR_INHIBITS);
}

/* ------------------------------------------------------------------ Session */
static const char *event_name(uint16_t code)
{
    switch (code) {
    case 0x0001: return "BOOT_COMPLETED";
    case 0x0010: return "STATE_CHANGED";
    case 0x0011: return "INHIBIT_CHANGED";
    case 0x0012: return "FAULT_CHANGED";
    case 0x0013: return "SESSION_ARMED";
    case 0x0014: return "CHARGE_ESTABLISHED";
    case 0x0015: return "SESSION_COMPLETED";
    case 0x0016: return "SESSION_ABORTED";
    case 0x0020: return "CONFIG_CHANGED";
    case 0x0021: return "CAL_CHANGED";
    case 0x0030: return "COMM_DEGRADED";
    case 0x0031: return "PERSISTENCE_HEALTH";
    case 0x0032: return "DIAGNOSTIC_LOSS";
    case 0x0033: return "MEASUREMENT_FAULT";
    case 0x0040: return "TIME_SYNC_CHANGED";
    default: return "EVENT";
    }
}
static uint32_t event_color(uint16_t code)
{
    if (code == 0x0012) return FAULT_COL;
    if (code == 0x0011 || (code >= 0x0030 && code <= 0x0033)) return WARN_COL;
    if (code >= 0x0013 && code <= 0x0016) return NAV_CUR;
    return LABEL;
}
static void fmt_duration(char *b, size_t n, uint32_t ms)
{
    const uint32_t s = ms / 1000u;
    if (s >= 3600u) snprintf(b, n, "%uh %02um", (unsigned)(s / 3600u), (unsigned)(s % 3600u / 60u));
    else if (s >= 60u) snprintf(b, n, "%um %02us", (unsigned)(s / 60u), (unsigned)(s % 60u));
    else snprintf(b, n, "%us", (unsigned)s);
}
/* Controller time since its boot (the log has no wall clock). */
static void fmt_since_boot(char *b, size_t n, uint64_t us)
{
    const uint32_t s = (uint32_t)(us / 1000000u);
    if (s < 120u) snprintf(b, n, "+%us", (unsigned)s);
    else if (s < 7200u) snprintf(b, n, "+%um", (unsigned)(s / 60u));
    else snprintf(b, n, "+%uh", (unsigned)(s / 3600u));
}
static void event_detail(char *b, size_t n, uint16_t code, const uint32_t a[4])
{
    char x[32], y[32], z[20];
    b[0] = '\0';
    switch (code) {
    case 0x0001: snprintf(b, n, "reset: %s", (a[0] & 0xffu) < 7u ? reset_names[a[0] & 0xffu] : "?"); break;
    case 0x0010:
        snprintf(x, sizeof(x), "%s", state_name(a[0] & 0xffu, (a[0] >> 8) & 0xffu));
        snprintf(y, sizeof(y), "%s", state_name((a[0] >> 16) & 0xffu, (a[0] >> 24) & 0xffu));
        snprintf(b, n, "%.24s \xE2\x86\x92 %.24s", x, y); break;
    case 0x0011:
        mask_text(x, sizeof(x), a[0], inhibit_names, 3); mask_text(y, sizeof(y), a[1], inhibit_names, 3);
        snprintf(b, n, "%.24s \xE2\x86\x92 %.24s", x, y); break;
    case 0x0012:
        snprintf(b, n, "%s \xE2\x86\x92 %s", a[0] ? "active" : "none",
                 (a[2] & 0xffffu) ? fault_name((uint16_t)a[2]) : "none"); break;
    case 0x0013: snprintf(b, n, "session #%" PRIu32, a[0]); break;
    case 0x0014:
        fmt_milli(x, sizeof(x), (int32_t)a[1], false); fmt_milli(y, sizeof(y), (int32_t)a[2], false);
        snprintf(b, n, "%.20s V \xC2\xB7 %.20s A", x, y); break;
    case 0x0015: case 0x0016:
        fmt_duration(z, sizeof(z), a[2]);
        snprintf(b, n, "%s \xC2\xB7 %s", (a[1] & 0xffu) < 10u ? end_names[a[1] & 0xffu] : "?", z); break;
    default: break;
    }
}
/* Right-aligned event detail; "A -> B" is drawn with a chevron between the parts. */
static void detail_right(lv_obj_t *p, int right, int y, int h, const char *s, int min_x)
{
    static const char arrow[] = "\xE2\x86\x92";
    const char *a = strstr(s, arrow);
    if (a) {
        char l[40], r[40];
        size_t ln = (size_t)(a - s);
        while (ln && s[ln - 1] == ' ') --ln;
        if (ln >= sizeof(l)) ln = sizeof(l) - 1;
        memcpy(l, s, ln); l[ln] = '\0';
        snprintf(r, sizeof(r), "%s", a + sizeof(arrow) - 1 + ((a[sizeof(arrow) - 1] == ' ') ? 1 : 0));
        lv_obj_t *rl = text(p, r, 0, 0, F10, TXT2);
        const int wr = width_of(rl);
        lv_obj_t *ll = text(p, l, 0, 0, F10, TXT2);
        const int wl = width_of(ll);
        const int xr = right - wr, xc = xr - 4 - 10, xl = xc - 4 - wl;
        if (xl >= min_x) {
            lv_obj_set_pos(rl, xr, y + (h - lh(F10)) / 2);
            lv_obj_set_pos(ll, xl, y + (h - lh(F10)) / 2);
            icon_row(p, &hmi_icon_chev_10, xc, y, h, LABEL);
            return;
        }
        lv_obj_delete(ll);
        lv_obj_delete(rl);
        text_in(p, r, min_x, y, right - min_x, h, F10, TXT2, LV_TEXT_ALIGN_RIGHT);
        return;
    }
    text_in(p, s, min_x, y, right - min_x, h, F10, TXT2, LV_TEXT_ALIGN_RIGHT);
}

static void sync_click(lv_event_t *e) { (void)e; dirty = true; }

static void page_session(lv_obj_t *p, const rcc_snapshot_t *m, int H)
{
    state_info_t s; state_info(m, &s);

    const bool in_session = fresh(m) && m->top_state == 4 && m->operational_state >= 2 && m->operational_state <= 6;

    static const char *const k[4] = {"SESSION", "DURATION", "LAST CMD", "END REASON"};
    char v[4][40];
    if (fresh(m) && m->session_id) snprintf(v[0], sizeof(v[0]), "#%" PRIu32 " \xC2\xB7 %s", m->session_id, m->simulated_measurements ? "SIMULATED V/I" : s.name);
    else snprintf(v[0], sizeof(v[0]), "%s", s.name);
    if (m->end_valid && !in_session && (!m->session_id || m->end_session_id == m->session_id)) fmt_duration(v[1], sizeof(v[1]), m->end_duration_ms);
    else if (in_session) snprintf(v[1], sizeof(v[1]), "In progress");
    else snprintf(v[1], sizeof(v[1]), "\xE2\x80\x94");
    if (m->command_state == RCC_COMMAND_NONE) snprintf(v[2], sizeof(v[2]), "\xE2\x80\x94");
    else {
        const char *res = m->command_state == RCC_COMMAND_COMPLETED ? "OK" :
                          m->command_state == RCC_COMMAND_REJECTED ? "REJECTED" :
                          m->command_state == RCC_COMMAND_FAILED ? "FAILED" :
                          m->command_state == RCC_COMMAND_UNCONFIRMED ? "NO REPLY" : "WAIT";
        snprintf(v[2], sizeof(v[2]), "%s \xC2\xB7 %s", m->last_command == 0x21 ? "STOP" : "START", res);
    }
    snprintf(v[3], sizeof(v[3]), "%s", m->end_valid ? (m->end_reason < 10 ? end_names[m->end_reason] : "?") : "\xE2\x80\x94");
    for (int i = 0; i < 4; ++i) {
        const int bx = i * 1035 / 10;       /* 4 columns, 6 px gaps */
        lv_obj_t *c = card_box(p, bx, 0, 98, 39, CARD, 8, LINE);
        text_in(c, k[i], 9, 5, 80, 13, F10, LABEL, LV_TEXT_ALIGN_LEFT);
        text_in(c, v[i], 9, 18, 80, 16, F12B, WHITE, LV_TEXT_ALIGN_LEFT);
    }

    const int ev_h = H - 45;
    lv_obj_t *ev = card_box(p, 0, 45, 408, ev_h, CARD, 10, LINE);
    char head[64];
    if (m->online && m->device_valid) {
        if (m->event_missed) snprintf(head, sizeof(head), "EVENT RING BUFFER (%u held \xC2\xB7 %" PRIu32 " rotated)", (unsigned)m->event_count, m->event_missed);
        else snprintf(head, sizeof(head), "EVENT RING BUFFER (%u held)", (unsigned)m->event_count);
    } else {
        snprintf(head, sizeof(head), "EVENT RING BUFFER (offline)");
    }
    lv_obj_t *hl = text(ev, head, 11, 6 + (22 - lh(F10)) / 2, F10, LABEL);
    spacing(hl, 1);
    if (m->event_missed) { /* second colour for the rotated count is not worth a span; keep one colour */ }

    lv_obj_t *sy = button(ev, 330, 6, 67, 22, NAV_BOX, 8, sync_click, NULL);
    lv_obj_set_style_border_width(sy, 1, 0);
    lv_obj_set_style_border_color(sy, C(0x1E4963), 0);
    lv_obj_t *sl = text(sy, "SYNC", 0, 0, F10, NAV_CUR);
    spacing(sl, 1);
    lv_obj_set_clickable(sl, false);
    const int sw = width_of(sl), tot = 11 + 6 + sw;
    icon_row(sy, &hmi_icon_refresh_11, (65 - tot) / 2, 0, 20, NAV_CUR);
    lv_obj_set_pos(sl, (65 - tot) / 2 + 17, (20 - lh(F10)) / 2);

    if (!m->online) {
        icon_row(ev, &hmi_icon_warn_16, 90, 34, ev_h - 34, WARN_COL);
        text_in(ev, "Event log unavailable \xE2\x80\x94 UART link lost", 112, 34, 290, ev_h - 34, F12B, WARN_COL, LV_TEXT_ALIGN_LEFT);
        return;
    }
    if (!m->event_count) {
        text_in(ev, "No events held by the controller", 11, 34, 386, 20, F12, LABEL, LV_TEXT_ALIGN_LEFT);
        return;
    }
    lv_obj_t *a = scroll_area(ev, 0, 34, 408, ev_h - 34 - 3, SCR_EVENTS);
    for (int i = 0; i < m->event_count; ++i) {
        const int n = (int)m->event_count - 1 - i, ry = i * 20;
        char det[64], since[16];
        fmt_since_boot(since, sizeof(since), m->event_us[n]);
        text_in(a, since, 11, ry, 44, 20, F10, LABEL, LV_TEXT_ALIGN_LEFT);
        box(a, 57, ry + 7, 6, 6, event_color(m->event_codes[n]), 3);
        lv_obj_t *nl = text(a, event_name(m->event_codes[n]), 70, 0, F12B, WHITE);
        const int nw = width_of(nl);
        lv_obj_set_pos(nl, 70, ry + (20 - lh(F12B)) / 2);
        event_detail(det, sizeof(det), m->event_codes[n], m->event_args[n]);
        detail_right(a, 397, ry, 20, det, 70 + nw + 8);
        if (i + 1 < m->event_count) hline(a, 11, ry + 19, 386, LINE);
    }
    scroll_restore(a, SCR_EVENTS);
}

/* ------------------------------------------------------------------ Device & Settings */
static void redraw_cb(void *unused);

static void tab_click(lv_event_t *e)
{
    device_tab = (int)(uintptr_t)lv_event_get_user_data(e);
    memset(scroll_pos, 0, sizeof(scroll_pos));
    (void)lv_async_call(redraw_cb, NULL);
}

static void set_brightness_cb(lv_event_t *e)
{
    const uint8_t pct = (uint8_t)(uintptr_t)lv_event_get_user_data(e);
    hmi_settings_set_brightness(pct);
    hmi_backlight_set_duty(pct, true);
    (void)lv_async_call(redraw_cb, NULL);
}

static void set_sleep_cb(lv_event_t *e)
{
    const uint16_t sec = (uint16_t)(uintptr_t)lv_event_get_user_data(e);
    hmi_settings_set_sleep_timeout(sec);
    (void)lv_async_call(redraw_cb, NULL);
}

static hmi_wifi_status_t ws;
static void wifi_open_cb(lv_event_t *e) { (void)e; wifi_open(); }

static void set_lang_cb(lv_event_t *e)
{
    const uint8_t l = (uint8_t)(uintptr_t)lv_event_get_user_data(e);
    hmi_settings_set_language(l);
    (void)lv_async_call(redraw_cb, NULL);
}

static void page_device(lv_obj_t *p, const rcc_snapshot_t *m, int H)
{
    const bool rd = m->online && m->device_valid;
    const bool provisional = rd && (m->build_flags & 1u);
    const uint32_t nc = rd ? WARN_COL : OFF_COL;

    lv_obj_t *nt = card_box(p, 0, 0, 408, 24, rd ? WARN_BG : OFF_BG, 8, nc);
    if (rd) icon_row(nt, &hmi_icon_warn_13, 11, 0, 24, WARN_COL);
    lv_obj_t *nl = text_in(nt, rd ? (provisional ? "BENCH IMAGE \xE2\x80\x94 NOT A PRODUCT BUILD" : "PRODUCT BUILD")
                                  : "CONTROLLER INFO NOT READ", rd ? 32 : 11, 0, 250, 24, F10, nc, LV_TEXT_ALIGN_LEFT);
    spacing(nl, 0);
    text_in(nt, rd ? ((m->build_flags & 2u) ? "" : "Relay locked") : "Link lost", 280, 0, 115, 24, F10, TXT2, LV_TEXT_ALIGN_RIGHT);

    static const char *const tab_names[3] = {"CONTROLLER", "COMM LINK", "SETTINGS"};
    lv_obj_t *seg = card_box(p, 0, 30, 408, 32, CARD, 8, LINE);
    const int tab_w = 134;
    for (int i = 0; i < 3; ++i) {
        const bool on = (i == device_tab);
        lv_obj_t *bt = button(seg, 3 + i * tab_w, 3, tab_w, 26, on ? NAV_BOX : CARD, 6, tab_click, (void *)(uintptr_t)i);
        if (on) { lv_obj_set_style_border_width(bt, 1, 0); lv_obj_set_style_border_color(bt, C(0x1E4963), 0); }
        lv_obj_t *l = text_in(bt, tab_names[i], 0, 0, tab_w, 26, F10, on ? NAV_CUR : DIM, LV_TEXT_ALIGN_CENTER);
        spacing(l, 1);
        lv_obj_set_clickable(l, false);
    }

    const int ry = 68, ch = H - ry, ih = ch - 14;
    char b[64], t[24];
    if (device_tab == 0) {
        lv_obj_t *c1 = card_box(p, 0, ry, 200, ch, CARD, 10, LINE);
        section(c1, 13, 7, 174, "FIRMWARE", NULL, 0);
        const bool compat = rd && m->product_id == 1u && m->protocol_version == 1u;
        chip(c1, 187, 7, rd ? (compat ? "COMPATIBLE" : "INCOMPAT.") : "UNREAD", rd ? (compat ? OK_COL : FAULT_COL) : OFF_COL,
             rd ? (compat ? OK_BG : FAULT_BG) : SURFACE2);
        const int g1 = (ih - 16 - 72) / 4;
        const char *k1[4] = {"Product", "HW / FW", "Image CRC", "Boot ID"};
        char v1[4][24];
        if (rd) {
            snprintf(v1[0], 24, "0x%04X \xC2\xB7 v%u", (unsigned)m->product_id, (unsigned)m->protocol_version);
            snprintf(v1[1], 24, "0x%02" PRIX32 " / 0x%02" PRIX32, m->hardware_revision, m->firmware_revision);
            snprintf(v1[2], 24, "%02X%02X\xE2\x80\xA6%02X%02X", m->image_id[0], m->image_id[1], m->image_id[6], m->image_id[7]);
            snprintf(v1[3], 24, "\xE2\x80\xA6%04X", (unsigned)(m->boot_id & 0xffffu));
        } else {
            for (int i = 0; i < 4; ++i) snprintf(v1[i], 24, "\xE2\x80\x94");
        }
        for (int i = 0; i < 4; ++i) kv(c1, 13, 7 + 16 + (i + 1) * g1 + i * 18, 174, 18, k1[i], v1[i], WHITE);

        lv_obj_t *c2 = card_box(p, 208, ry, 200, ch, CARD, 10, LINE);
        section(c2, 13, 7, 174, "BUILD FLAGS & BUSES", NULL, 0);
        const int g2 = (ih - 16 - 54 - 20) / 4;
        static const char *const fl[3] = {"Provisional", "Relay enable", "Op. node"};
        for (int i = 0; i < 3; ++i) {
            const int yy = 7 + 16 + (i + 1) * g2 + i * 18;
            const bool on = rd && ((m->build_flags >> i) & 1u);
            text_in(c2, fl[i], 13, yy, 120, 18, F12, LABEL, LV_TEXT_ALIGN_LEFT);
            if (!rd) text_in(c2, "\xE2\x80\x94", 13, yy, 174, 18, F12B, DIM, LV_TEXT_ALIGN_RIGHT);
            else chip(c2, 187, yy, on ? "ON" : "OFF", on ? WARN_COL : LABEL, on ? WARN_BG : SURFACE2);
        }
        static const char *const ln[4] = {"CAN", "RS485", "UART", "SVC"};
        int lw[4], tot = 0;
        for (int i = 0; i < 4; ++i) {
            lv_obj_t *tmp = text(c2, ln[i], 0, 0, F10, DIM);
            lw[i] = width_of(tmp) + 14; tot += lw[i];
            lv_obj_delete(tmp);
        }
        const int cg = (174 - tot) / 3, cy = 7 + 16 + 4 * g2 + 54 + 1;
        int cx = 13;
        for (int i = 0; i < 4; ++i) {
            const bool on = rd && ((m->running_links >> i) & 1u);
            chip_at(c2, cx, cy, false, ln[i], on ? NAV_CUR : DIM, on ? NAV_BOX : SURFACE2);
            cx += lw[i] + cg;
        }
    } else if (device_tab == 1) {
        lv_obj_t *c1 = card_box(p, 0, ry, 233, ch, CARD, 10, LINE);
        const bool on = m->online && link_started;
        const uint32_t c = on ? OK_COL : OFF_COL;
        box(c1, 13, 7 + 7, 7, 7, c, 4);
        text_in(c1, on ? "ONLINE" : "OFFLINE", 26, 7, 80, 20, F14B, c, LV_TEXT_ALIGN_LEFT);
        fmt_age(t, sizeof(t), m->reply_age_ms); snprintf(b, sizeof(b), "Reply %s ago", t);
        text_in(c1, b, 110, 7, 110, 20, F10, LABEL, LV_TEXT_ALIGN_RIGHT);
        const int y0 = ch - 7 - 54;
        snprintf(b, sizeof(b), "%" PRIu32, m->timeouts); kv(c1, 13, y0, 98, 18, "Timeouts", b, WHITE);
        snprintf(b, sizeof(b), "%" PRIu32, m->rx_bad_frames); kv(c1, 13, y0 + 18, 98, 18, "Bad frames", b, WHITE);
        snprintf(b, sizeof(b), "%" PRIu32, m->event_missed); kv(c1, 13, y0 + 36, 98, 18, "Missed ev.", b, WHITE);
        fmt_age(t, sizeof(t), m->status_age_ms); kv(c1, 122, y0, 98, 18, "Status age", t, WHITE);
        fmt_age(t, sizeof(t), m->measurement_age_ms); kv(c1, 122, y0 + 18, 98, 18, "Meas. age", t, WHITE);
        fmt_age(t, sizeof(t), m->faults_age_ms); kv(c1, 122, y0 + 36, 98, 18, "Fault age", t, WHITE);

        lv_obj_t *c2 = card_box(p, 241, ry, 167, ch, CARD, 10, LINE);
        section(c2, 13, 7, 141, "HMI PORT", NULL, 0);
        const int g = (ih - 16 - 54) / 3;
        kv(c2, 13, 7 + 16 + g, 141, 18, "Bus", "UART0", WHITE);
        kv(c2, 13, 7 + 16 + 2 * g + 18, 141, 18, "Baud", "115200 8N1", WHITE);
        kv(c2, 13, 7 + 16 + 3 * g + 36, 141, 18, "Pins", "GPIO1 / 3", WHITE);
    } else {
        const hmi_settings_t *cfg = hmi_settings_get();

        /* Card 1: display and sleep. Rows are packed so everything fits the shortest layout (ch = 162). */
        lv_obj_t *c1 = card_box(p, 0, ry, 200, ch, CARD, 10, LINE);
        section(c1, 13, 7, 174, "DISPLAY & POWER", NULL, 0);

        char br_label[32];
        snprintf(br_label, sizeof(br_label), "Backlight: %u%%", cfg->brightness_pct);
        text_in(c1, br_label, 13, 24, 174, 16, F12, LABEL, LV_TEXT_ALIGN_LEFT);

        static const uint8_t br_presets[4] = {25, 50, 75, 100};
        static const char *const br_names[4] = {"25%", "50%", "75%", "100%"};
        for (int i = 0; i < 4; ++i) {
            const bool sel = (cfg->brightness_pct == br_presets[i]) ||
                             (i == 3 && cfg->brightness_pct > 75) ||
                             (i == 0 && cfg->brightness_pct < 35);
            lv_obj_t *bt = button(c1, 13 + i * 44, 42, 40, 22, sel ? NAV_BOX : SURFACE2, 6,
                                  set_brightness_cb, (void *)(uintptr_t)br_presets[i]);
            if (sel) {
                lv_obj_set_style_border_width(bt, 1, 0);
                lv_obj_set_style_border_color(bt, C(NAV_CUR), 0);
            }
            lv_obj_t *l = text_in(bt, br_names[i], 0, 0, 40, 22, F10, sel ? NAV_CUR : WHITE, LV_TEXT_ALIGN_CENTER);
            lv_obj_set_clickable(l, false);
        }

        char sl_label[32];
        if (cfg->sleep_timeout_s == 0) snprintf(sl_label, sizeof(sl_label), "Sleep timeout: off");
        else snprintf(sl_label, sizeof(sl_label), "Sleep timeout: %u min", (unsigned)(cfg->sleep_timeout_s / 60));
        text_in(c1, sl_label, 13, 70, 174, 16, F12, LABEL, LV_TEXT_ALIGN_LEFT);

        static const uint16_t sl_presets[4] = {0, 60, 180, 300};
        static const char *const sl_names[4] = {"OFF", "1m", "3m", "5m"};
        for (int i = 0; i < 4; ++i) {
            const bool sel = (cfg->sleep_timeout_s == sl_presets[i]);
            lv_obj_t *bt = button(c1, 13 + i * 44, 88, 40, 22, sel ? NAV_BOX : SURFACE2, 6,
                                  set_sleep_cb, (void *)(uintptr_t)sl_presets[i]);
            if (sel) {
                lv_obj_set_style_border_width(bt, 1, 0);
                lv_obj_set_style_border_color(bt, C(NAV_CUR), 0);
            }
            lv_obj_t *l = text_in(bt, sl_names[i], 0, 0, 40, 22, F10, sel ? NAV_CUR : WHITE, LV_TEXT_ALIGN_CENTER);
            lv_obj_set_clickable(l, false);
        }

        char dim[24]; snprintf(dim, sizeof(dim), "%u%%", (unsigned)cfg->dim_pct);
        kv(c1, 13, 118, 174, 18, "Idle dim level", dim, WHITE);
        kv(c1, 13, 136, 174, 18, "Wake method", "Touch", WHITE);

        /* Card 2: language and system info */
        lv_obj_t *c2 = card_box(p, 208, ry, 200, ch, CARD, 10, LINE);
        section(c2, 13, 7, 100, "SYSTEM", NULL, 0);
        chip(c2, 187, 7, "ACTIVE", OK_COL, OK_BG);

        text_in(c2, "Language", 13, 24, 174, 16, F12, LABEL, LV_TEXT_ALIGN_LEFT);
        static const char *const lang_names[2] = {"English", "Ti\xE1\xBA\xBFng Vi\xE1\xBB\x87t"};
        for (int i = 0; i < 2; ++i) {
            const bool sel = (cfg->language == (uint8_t)i);
            const int lw = 84;
            lv_obj_t *bt = button(c2, 13 + i * (lw + 6), 42, lw, 22, sel ? NAV_BOX : SURFACE2, 6,
                                  set_lang_cb, (void *)(uintptr_t)i);
            if (sel) {
                lv_obj_set_style_border_width(bt, 1, 0);
                lv_obj_set_style_border_color(bt, C(NAV_CUR), 0);
            }
            lv_obj_t *l = text_in(bt, lang_names[i], 0, 0, lw, 22, F10, sel ? NAV_CUR : WHITE, LV_TEXT_ALIGN_CENTER);
            lv_obj_set_clickable(l, false);
        }

        kv(c2, 13, 74, 174, 18, "Task watchdog", "5.0 s", WHITE);
        kv(c2, 13, 92, 174, 18, "LCD driver", "ST77922 QSPI", WHITE);
        kv(c2, 13, 110, 174, 18, "UI version", "v3.3 OTA Bento", OK_COL);

        hmi_wifi_get(&ws);
        lv_obj_t *wr = button(c2, 13, 132, 174, 22, SURFACE2, 6, wifi_open_cb, NULL);
        icon_row(wr, &hmi_icon_wifi_13, 8, 0, 22, NAV_CUR);
        text_in(wr, "Wi-Fi", 28, 0, 40, 22, F10, WHITE, LV_TEXT_ALIGN_LEFT);
        const char *wst = ws.state == HMI_WIFI_UNAVAILABLE ? "Unavailable" :
                          ws.state == HMI_WIFI_CONNECTED ? ws.ssid : ws.state == HMI_WIFI_CONNECTING ? "Connecting..." : "Not connected";
        text_in(wr, wst, 66, 0, 84, 22, F10, ws.state == HMI_WIFI_CONNECTED ? OK_COL : LABEL, LV_TEXT_ALIGN_RIGHT);
        icon_row(wr, &hmi_icon_chev_10, 156, 0, 22, DIM);
    }
}

/* ------------------------------------------------------------------ Action bar */
typedef struct {
    enum { BAR_NONE, BAR_FULL, BAR_SPLIT, BAR_MSG, BAR_NOAUTH } kind;
    bar_button_t button;
    char title[48];
    char sub[72];
    uint32_t title_color;
} bar_spec_t;

static const char *reason_text(uint16_t r, char *buf, size_t n)
{
    static const char *const names[0x17] = {"NONE", "UNSUPPORTED_VERSION", "MALFORMED_MESSAGE",
        "INTEGRITY_FAILED", "UNSUPPORTED_COMMAND", "UNAUTHORIZED_SOURCE", "DUPLICATE_ID_CONFLICT",
        "QUEUE_FULL", "INVALID_STATE", "INHIBITED", "FAULT_ACTIVE", "INTERLOCK_FAILED",
        "MEASUREMENT_INVALID", "CONFIG_INVALID", "CAL_INVALID", "PERSISTENCE_FAILED",
        "CHARGER_REQUEST_ABSENT", "TIMEOUT", "CHARGE_NOT_ESTABLISHED", "ABORTED_BY_STOP",
        "ABORTED_BY_FAULT", "INTERNAL_ERROR", "RELAY_FEEDBACK_MISMATCH"};
    if (r < 0x17) return names[r];
    snprintf(buf, n, "0x%04X", (unsigned)r);
    return buf;
}

static void bar_spec(const rcc_snapshot_t *m, bar_spec_t *b)
{
    memset(b, 0, sizeof(*b));
    state_info_t s; state_info(m, &s);
    char rb[16];
    const bool pending = m->command_state == RCC_COMMAND_SENDING || m->command_state == RCC_COMMAND_ACCEPTED ||
                         m->command_state == RCC_COMMAND_UNCONFIRMED;
    const bool terminal = m->command_state == RCC_COMMAND_COMPLETED || m->command_state == RCC_COMMAND_REJECTED ||
                          m->command_state == RCC_COMMAND_FAILED;
    const bool recent = pending || (terminal && m->command_age_ms < 15000u);
    const bool is_overview = page == OVERVIEW;
    b->title_color = NAV_CUR;
    if (recent && no_authority(m)) { b->kind = BAR_NOAUTH; return; }
    if (recent && m->last_command == 0x21) {
        b->kind = BAR_SPLIT; b->button = B_STOP;
        switch (m->command_state) {
        case RCC_COMMAND_SENDING:
            snprintf(b->title, sizeof(b->title), "Waiting for the controller\xE2\x80\xA6");
            snprintf(b->sub, sizeof(b->sub), "STOP sent \xE2\x80\x94 not yet confirmed"); b->button = B_STOPPING; break;
        case RCC_COMMAND_COMPLETED:
            snprintf(b->title, sizeof(b->title), "Stopped by command");
            snprintf(b->sub, sizeof(b->sub), "Relay commanded open");
            b->button = start_allowed(m) ? B_START : B_NONE; break;
        case RCC_COMMAND_REJECTED:
            b->title_color = WARN_COL;
            snprintf(b->title, sizeof(b->title), "STOP rejected");
            snprintf(b->sub, sizeof(b->sub), "%s", reason_text(m->command_reason, rb, sizeof(rb))); break;
        case RCC_COMMAND_FAILED:
            b->title_color = WARN_COL;
            snprintf(b->title, sizeof(b->title), "Stop took effect, not tracked");
            snprintf(b->sub, sizeof(b->sub), "Send STOP again"); break;
        default:
            b->title_color = WARN_COL;
            snprintf(b->title, sizeof(b->title), "NOT CONFIRMED");
            snprintf(b->sub, sizeof(b->sub), "No reply to STOP \xE2\x80\x94 check controller"); break;
        }
    } else if (recent && m->last_command == 0x20 && m->command_state != RCC_COMMAND_COMPLETED) {
        b->kind = BAR_SPLIT; b->button = B_STOP;
        switch (m->command_state) {
        case RCC_COMMAND_SENDING:
            snprintf(b->title, sizeof(b->title), "Sending START\xE2\x80\xA6");
            snprintf(b->sub, sizeof(b->sub), "Waiting for the controller"); break;
        case RCC_COMMAND_ACCEPTED:
            snprintf(b->title, sizeof(b->title), "Request accepted \xE2\x80\x94 checking");
            snprintf(b->sub, sizeof(b->sub), "Accepted, not yet complete"); break;
        case RCC_COMMAND_UNCONFIRMED:
            b->title_color = WARN_COL;
            snprintf(b->title, sizeof(b->title), "NOT CONFIRMED");
            snprintf(b->sub, sizeof(b->sub), "No reply to START"); break;
        default:
            b->title_color = WARN_COL;
            snprintf(b->title, sizeof(b->title), "START rejected");
            snprintf(b->sub, sizeof(b->sub), "%s", reason_text(m->command_reason, rb, sizeof(rb)));
            b->button = start_allowed(m) ? B_START : B_STOP; break;
        }
    } else {
        b->kind = BAR_FULL; b->button = start_allowed(m) ? B_START : B_STOP;
        if (!is_overview) {
            if (b->button == B_START) { b->kind = BAR_NONE; b->button = B_NONE; return; }
            b->kind = BAR_SPLIT;
            b->title_color = s.kind == K_NORMAL ? WHITE : kind_color(s.kind);
            snprintf(b->title, sizeof(b->title), "%s", s.name);
            if (!fresh(m)) snprintf(b->sub, sizeof(b->sub), "Charging may still be in progress");
            else if (m->top_state == 6) snprintf(b->sub, sizeof(b->sub), "%s \xC2\xB7 service needed", fault_name(m->primary_fault));
            else if (m->session_id) snprintf(b->sub, sizeof(b->sub), "Session #%" PRIu32 " \xC2\xB7 relay commanded %s", m->session_id, m->relay_command ? "closed" : "open");
            else snprintf(b->sub, sizeof(b->sub), "Relay commanded %s", m->relay_command ? "closed" : "open");
        }
    }
    if (b->kind == BAR_SPLIT && b->button == B_NONE) b->kind = BAR_MSG;
    if (!is_overview && b->button == B_START) { b->button = B_NONE; b->kind = BAR_MSG; }
}

static void close_modal(void)
{
    if (modal) { lv_obj_delete_async(modal); modal = NULL; lv_obj_set_style_opa(bar_box, LV_OPA_COVER, 0); }
}
static void cancel_click(lv_event_t *e) { (void)e; close_modal(); dirty = true; }
static void send_start_click(lv_event_t *e) { (void)e; (void)rcc_link_request_start(); close_modal(); dirty = true; }

static void open_modal(const rcc_snapshot_t *m)
{
    if (modal) return;
    state_info_t s; state_info(m, &s);
    char b[48], t[24];
    lv_obj_set_style_opa(bar_box, 77, 0);
    modal = obj(screen, 56, 32, 424, 288);
    lv_obj_set_clickable(modal, true);
    lv_obj_set_style_bg_color(modal, C(0x06090E), 0);
    lv_obj_set_style_bg_opa(modal, 209, 0);
    lv_obj_add_event_cb(modal, cancel_click, LV_EVENT_CLICKED, NULL);

    lv_obj_t *card = card_box(modal, 36, 14, 352, 208, CARD, 12, NAV_CUR);
    lv_obj_set_clickable(card, true);                       /* taps inside the card must not cancel */
    text_in(card, m->simulated_measurements ? "Start with SIMULATED V/I" : "Confirm Charge Start", 17, 12, 318, 19, F16B, WHITE, LV_TEXT_ALIGN_LEFT);

    if (fresh(m) && m->top_state == 4 && m->operational_state == 1) snprintf(b, sizeof(b), "IDLE \xC2\xB7 Ready");
    else snprintf(b, sizeof(b), "%s", s.name);
    kv(card, 17, 38, 318, 21, "Controller state", b, WHITE);

    fmt_age(t, sizeof(t), m->reply_age_ms); snprintf(b, sizeof(b), "ONLINE \xC2\xB7 %s ago", t);
    kv(card, 17, 59, 318, 21, "UART link", b, WHITE);

    if (v_valid(m) && i_valid(m)) snprintf(b, sizeof(b), "Fresh \xC2\xB7 %" PRIu32 " ms", m->measurement_sample_age_ms);
    else snprintf(b, sizeof(b), "Not valid");
    kv(card, 17, 80, 318, 21, m->simulated_measurements ? "SIMULATED V/I" : "ADC telemetry", b, WHITE);

    const fault_state_t fs = fault_state(m);
    if (fs == FS_FAULT) snprintf(b, sizeof(b), "%s", fault_name(m->primary_fault));
    else if (fs == FS_INHIBIT) mask_text(b, sizeof(b), m->inhibit_mask, inhibit_names, 3);
    else snprintf(b, sizeof(b), "0x00000000 (Clear)");
    kv(card, 17, 101, 318, 21, "Fault / Inhibit", b, fs == FS_OK ? WHITE : fs == FS_FAULT ? FAULT_COL : WARN_COL);

    text_in(card, m->simulated_measurements ? "REAL relay command; contacts are not measured." : "Controller validates VREQ & precheck before closing relay.", 17, 128, 318, 13, F10, LABEL, LV_TEXT_ALIGN_LEFT);

    lv_obj_t *c1 = button(card, 17, 157, 154, 38, SURFACE2, 10, cancel_click, NULL);
    lv_obj_t *l1 = text_in(c1, "CANCEL", 0, 0, 154, 38, F12B, WHITE, LV_TEXT_ALIGN_CENTER); spacing(l1, 1);
    lv_obj_set_clickable(l1, false);
    lv_obj_t *c2 = button(card, 181, 157, 154, 38, START_COL, 10, send_start_click, NULL);
    lv_obj_t *l2 = text_in(c2, "SEND START", 0, 0, 154, 38, F12B, 0x06090E, LV_TEXT_ALIGN_CENTER); spacing(l2, 1);
    lv_obj_set_clickable(l2, false);
}

static void action_click(lv_event_t *e)
{
    const bool start = (uintptr_t)lv_event_get_user_data(e) == 1u;
    if (modal) return;
    if (start) { rcc_snapshot_t m; rcc_link_snapshot(&m); if (start_allowed(&m)) open_modal(&m); }
    else (void)rcc_link_request_stop();
    dirty = true;
}

static void bar_button(lv_obj_t *p, int x, int w, bar_button_t kind, const lv_font_t *f)
{
    const char *label = kind == B_START ? "START" : kind == B_STOPPING ? "STOPPING\xE2\x80\xA6" : "STOP";
    const lv_image_dsc_t *glyph = kind == B_START ? &hmi_icon_play_16 : kind == B_STOPPING ? &hmi_icon_spin_16 : &hmi_icon_stop_16;
    const uint32_t bg = kind == B_START ? START_COL : kind == B_STOPPING ? 0x4A1818 : STOP_COL;
    const uint32_t fg = kind == B_START ? 0x06090E : kind == B_STOPPING ? 0xFCA5A5 : 0xFFFFFF;
    lv_obj_t *b = button(p, x, 0, w, 42, bg, 10, action_click, (void *)(uintptr_t)(kind == B_START ? 1 : 0));
    lv_obj_set_style_border_width(b, 1, 0);
    lv_obj_set_style_border_color(b, C(0xFFFFFF), 0);
    lv_obj_set_style_border_opa(b, 36, 0);
    lv_obj_t *tx = text(b, label, 0, 0, f, fg);
    spacing(tx, 1);
    lv_obj_set_clickable(tx, false);
    const int iw = glyph->header.w, tw = width_of(tx), total = iw + 8 + tw;
    lv_obj_t *ic = icon(b, glyph, (w - total) / 2, (42 - (int)glyph->header.h) / 2, fg);
    lv_obj_set_pos(tx, (w - total) / 2 + iw + 8, (42 - lh(f)) / 2);
    if (kind == B_STOPPING) {
        lv_image_set_pivot(ic, glyph->header.w / 2, glyph->header.h / 2);
        lv_anim_t an; lv_anim_init(&an);
        lv_anim_set_var(&an, ic); lv_anim_set_exec_cb(&an, spin_anim);
        lv_anim_set_values(&an, 0, 3600); lv_anim_set_duration(&an, 1000);
        lv_anim_set_repeat_count(&an, LV_ANIM_REPEAT_INFINITE);
        lv_anim_start(&an);
    }
}

static void build_bar(const bar_spec_t *b)
{
    lv_obj_clean(bar_box);
    if (b->kind == BAR_NONE) return;
    if (b->kind == BAR_NOAUTH) {
        lv_obj_t *nb = card_box(bar_box, 0, 0, 408, 42, WARN_BG, 10, WARN_COL);
        icon_row(nb, &hmi_icon_warn_20, 12, 0, 42, WARN_COL);
        text_in(nb, "No control authority \xE2\x80\x94 monitoring only", 42, 6, 354, 17, F12B, WHITE, LV_TEXT_ALIGN_LEFT);
        text_in(nb, "Use physical E-stop. Grant control on the controller service port.", 42, 22, 354, 13, F10, LABEL, LV_TEXT_ALIGN_LEFT);
        return;
    }
    if (b->kind == BAR_FULL) { bar_button(bar_box, 0, 408, b->button, F16B); return; }
    const int msg_w = b->kind == BAR_SPLIT ? 256 : 408;
    lv_obj_t *mb = card_box(bar_box, 0, 0, msg_w, 42, CARD, 10, LINE);
    text_in(mb, b->title, 13, 7, msg_w - 26, 15, F12B, b->title_color, LV_TEXT_ALIGN_LEFT);
    text_in(mb, b->sub, 13, 23, msg_w - 26, 13, F10, LABEL, LV_TEXT_ALIGN_LEFT);
    if (b->kind == BAR_SPLIT) bar_button(bar_box, 266, 142, b->button, F14B);
}

/* ------------------------------------------------------------------ Wi-Fi screens
 * Full-screen overlay (no nav rail) opened from Device > Settings > Wi-Fi. Views: network list,
 * password entry (on-screen keyboard), connecting, connected info, failure. The overlay polls
 * hmi_wifi_get() every 250 ms and re-renders only when the backend version or the view changes. */
typedef enum { WV_LIST, WV_PASS, WV_BUSY, WV_INFO, WV_FAIL } wifi_view_t;
enum { KEY_SHIFT = 256, KEY_BKSP, KEY_SYM, KEY_GO, KEY_NONE };

static lv_obj_t *wifi_field, *wifi_go_key;
static lv_timer_t *wifi_timer;
static wifi_view_t wifi_view;
static char wifi_target[33], wifi_pw[HMI_WIFI_PASS_MAX + 1];
static bool wifi_target_secure, wifi_show_pw, wifi_shift, wifi_sym;
static int wifi_pw_len;
static uint32_t wifi_conn_ver, wifi_sig = 0xffffffffu;

static void wifi_render(void);
static void wifi_render_cb(void *unused) { (void)unused; wifi_render(); }
static void wifi_redraw(void) { wifi_sig = 0xffffffffu; (void)lv_async_call(wifi_render_cb, NULL); }

static int rssi_level(int r) { return r >= -55 ? 4 : r >= -67 ? 3 : r >= -78 ? 2 : 1; }
static const char *rssi_word(int r) { return r >= -55 ? "Excellent" : r >= -67 ? "Good" : r >= -78 ? "Fair" : "Weak"; }
static int target_rssi(void)
{
    for (int i = 0; i < ws.ap_count; ++i) if (!strcmp(ws.ap[i].ssid, wifi_target)) return ws.ap[i].rssi;
    return 0;
}

static void wifi_close(void)
{
    if (wifi_timer) { lv_timer_delete(wifi_timer); wifi_timer = NULL; }
    if (wifi_layer) { lv_obj_delete_async(wifi_layer); wifi_layer = NULL; }
    memset(wifi_pw, 0, sizeof(wifi_pw)); wifi_pw_len = 0;
    dirty = true;
    (void)lv_async_call(redraw_cb, NULL);
}

static void wifi_back_cb(lv_event_t *e)
{
    (void)e;
    if (wifi_view == WV_LIST) { wifi_close(); return; }
    if (wifi_view == WV_BUSY) hmi_wifi_cancel();
    wifi_view = WV_LIST;
    wifi_redraw();
}
static void wifi_scan_cb(lv_event_t *e) { (void)e; hmi_wifi_scan(); wifi_redraw(); }
static void wifi_disc_cb(lv_event_t *e) { (void)e; hmi_wifi_disconnect(); wifi_redraw(); }
static void wifi_forget_cb(lv_event_t *e) { (void)e; hmi_wifi_forget(); wifi_view = WV_LIST; wifi_redraw(); }
static void wifi_done_cb(lv_event_t *e) { (void)e; wifi_view = WV_LIST; wifi_redraw(); }

static void wifi_do_connect(void)
{
    hmi_wifi_get(&ws);
    wifi_conn_ver = ws.version;
    hmi_wifi_connect(wifi_target, wifi_target_secure ? wifi_pw : "");
    wifi_view = WV_BUSY;
    wifi_redraw();
}
static void wifi_retry_cb(lv_event_t *e) { (void)e; wifi_do_connect(); }

static void wifi_row_cb(lv_event_t *e)
{
    const int i = (int)(uintptr_t)lv_event_get_user_data(e);
    if (i < 0 || i >= ws.ap_count) return;
    snprintf(wifi_target, sizeof(wifi_target), "%s", ws.ap[i].ssid);
    wifi_target_secure = ws.ap[i].secure != 0;
    if (ws.state == HMI_WIFI_CONNECTED && !strcmp(ws.ssid, wifi_target)) { wifi_view = WV_INFO; wifi_redraw(); return; }
    memset(wifi_pw, 0, sizeof(wifi_pw)); wifi_pw_len = 0;
    wifi_shift = false; wifi_sym = false; wifi_show_pw = false;
    if (wifi_target_secure) { wifi_view = WV_PASS; wifi_redraw(); }
    else wifi_do_connect();
}

/* ---- shared pieces */
static void wifi_header(lv_obj_t *l, const char *title, const char *sub)
{
    lv_obj_t *b = button(l, 8, 8, 34, 30, SURFACE2, 8, wifi_back_cb, NULL);
    lv_obj_set_style_border_width(b, 1, 0);
    lv_obj_set_style_border_color(b, C(LINE), 0);
    lv_obj_set_style_pad_all(b, -1, 0);
    icon_c(b, &hmi_icon_back_14, 17, 15, TXT2);
    text_in(l, title, 52, 8, 330, 17, F14B, WHITE, LV_TEXT_ALIGN_LEFT);
    text_in(l, sub, 52, 25, 330, 13, F10, LABEL, LV_TEXT_ALIGN_LEFT);
}
static void wifi_bars(lv_obj_t *p, int x, int bottom, int level)
{
    static const int hh[4] = {4, 7, 10, 13};
    const uint32_t col = level >= 3 ? OK_COL : level == 2 ? WARN_COL : 0xFB923C;
    for (int i = 0; i < 4; ++i) box(p, x + i * 5, bottom - hh[i], 3, hh[i], i < level ? col : 0x2B3648, 1);
}
static lv_obj_t *wifi_btn(lv_obj_t *l, int x, int y, int w, const char *s, uint32_t bg, uint32_t fg, bool border, lv_event_cb_t cb)
{
    lv_obj_t *b = button(l, x, y, w, 40, bg, 10, cb, NULL);
    if (border) {
        lv_obj_set_style_border_width(b, 1, 0);
        lv_obj_set_style_border_color(b, C(LINE), 0);
        lv_obj_set_style_pad_all(b, -1, 0);
    }
    lv_obj_t *t = text_in(b, s, 0, 0, w, 40, F12B, fg, LV_TEXT_ALIGN_CENTER);
    spacing(t, 1);
    return b;
}
static void wifi_cell(lv_obj_t *l, int x, int y, const char *k, const char *v)
{
    lv_obj_t *c = card_box(l, x, y, 229, 44, CARD, 10, LINE);
    lv_obj_t *kl = text(c, k, 12, 7, F10, LABEL);
    spacing(kl, 1);
    text_in(c, v, 12, 22, 205, 17, F12B, WHITE, LV_TEXT_ALIGN_LEFT);
}
static void wifi_spinner(lv_obj_t *p, const lv_image_dsc_t *d, int cx, int cy, uint32_t col)
{
    lv_obj_t *ic = icon_c(p, d, cx, cy, col);
    lv_image_set_pivot(ic, d->header.w / 2, d->header.h / 2);
    lv_anim_t an; lv_anim_init(&an);
    lv_anim_set_var(&an, ic); lv_anim_set_exec_cb(&an, spin_anim);
    lv_anim_set_values(&an, 0, 3600); lv_anim_set_duration(&an, 1000);
    lv_anim_set_repeat_count(&an, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&an);
}

/* ---- list view */
static void wifi_view_list(lv_obj_t *l)
{
    char sub[32];
    if (ws.state == HMI_WIFI_UNAVAILABLE) snprintf(sub, sizeof(sub), "Wi-Fi unavailable");
    else if (ws.state == HMI_WIFI_SCANNING) snprintf(sub, sizeof(sub), "Scanning...");
    else snprintf(sub, sizeof(sub), "%u network%s found", (unsigned)ws.ap_count, ws.ap_count == 1 ? "" : "s");
    wifi_header(l, "Wi-Fi", sub);

    lv_obj_t *sc = button(l, 400, 10, 72, 26, NAV_BOX, 8, wifi_scan_cb, NULL);
    lv_obj_set_style_border_width(sc, 1, 0);
    lv_obj_set_style_border_color(sc, C(0x1E4963), 0);
    lv_obj_set_style_pad_all(sc, -1, 0);
    lv_obj_t *sl = text(sc, "SCAN", 0, 0, F10, NAV_CUR);
    spacing(sl, 1);
    const int sw = width_of(sl), tot = 12 + 6 + sw;
    icon_row(sc, &hmi_icon_refresh_11, (72 - tot) / 2, 0, 26, NAV_CUR);
    lv_obj_set_pos(sl, (72 - tot) / 2 + 17, (26 - lh(F10)) / 2);

    const bool conn = ws.state == HMI_WIFI_CONNECTED, un = ws.state == HMI_WIFI_UNAVAILABLE;
    lv_obj_t *st = card_box(l, 8, 46, 464, 26, conn ? OK_BG : un ? WARN_BG : CARD, 8, conn ? OK_COL : un ? WARN_COL : LINE);
    box(st, 10, 10, 6, 6, conn ? OK_COL : un ? WARN_COL : ws.state == HMI_WIFI_SCANNING ? NAV_CUR : DIM, 3);
    if (conn) {
        lv_obj_t *a = text(st, "Connected to", 24, 0, F10, TXT2);
        const int aw = width_of(a);
        lv_obj_set_y(a, (26 - lh(F10)) / 2);
        text_in(st, ws.ssid, 24 + aw + 5, 0, 200, 26, F10, WHITE, LV_TEXT_ALIGN_LEFT);
        lv_obj_t *db = button(st, 360, 0, 104, 26, CARD, 8, wifi_disc_cb, NULL);
        lv_obj_set_style_bg_opa(db, LV_OPA_TRANSP, 0);
        lv_obj_t *dl = text_in(db, "DISCONNECT", 0, 0, 94, 26, F10, LABEL, LV_TEXT_ALIGN_RIGHT);
        spacing(dl, 1);
    } else {
        text_in(st, un ? "Wi-Fi driver is not available in this build" : ws.state == HMI_WIFI_SCANNING ? "Scanning for networks..." : "Not connected",
                24, 0, 420, 26, F10, un ? WARN_COL : LABEL, LV_TEXT_ALIGN_LEFT);
    }

    if (!ws.ap_count) {
        text_in(l, un ? "Wi-Fi is not available on this controller" : "No networks found - tap SCAN", 0, 150, 480, 17, F12, LABEL, LV_TEXT_ALIGN_CENTER);
        return;
    }
    lv_obj_t *a = scroll_area(l, 8, 78, 464, 214, SCR_WIFI);
    for (int i = 0; i < ws.ap_count; ++i) {
        const hmi_wifi_ap_t *ap = &ws.ap[i];
        const bool cur = conn && !strcmp(ap->ssid, ws.ssid);
        lv_obj_t *b = button(a, 0, i * 44, 464, 40, cur ? NAV_BOX : CARD, 10, wifi_row_cb, (void *)(uintptr_t)i);
        lv_obj_set_style_border_width(b, 1, 0);
        lv_obj_set_style_border_color(b, C(cur ? 0x1E4963 : LINE), 0);
        lv_obj_set_style_pad_all(b, -1, 0);
        text_in(b, ap->ssid, 12, 6, ap->secure ? 330 : 345, 15, F12B, WHITE, LV_TEXT_ALIGN_LEFT);
        char s2[40];
        if (cur) snprintf(s2, sizeof(s2), "Connected \xC2\xB7 %s", ws.ip);
        else snprintf(s2, sizeof(s2), "%s", ap->secure ? "Secured" : "Open network");
        text_in(b, s2, 12, 21, 330, 13, F10, cur ? TXT2 : LABEL, LV_TEXT_ALIGN_LEFT);
        wifi_bars(b, 434, 27, rssi_level(ap->rssi));
        char rs[16]; snprintf(rs, sizeof(rs), "%d dBm", (int)ap->rssi);
        text_in(b, rs, 374, 0, 50, 40, F10, LABEL, LV_TEXT_ALIGN_RIGHT);
        if (ap->secure) icon_row(b, &hmi_icon_lock_11, 353, 0, 40, DIM);
    }
    scroll_restore(a, SCR_WIFI);
    text_in(l, "Drag to scroll \xC2\xB7 tap a network to connect", 0, 299, 480, 13, F10, DIM, LV_TEXT_ALIGN_CENTER);
}

/* ---- password view */
static void wifi_go_style(void)
{
    if (!wifi_go_key) return;
    const bool ok = !wifi_target_secure || wifi_pw_len >= HMI_WIFI_PASS_MIN;
    lv_obj_set_style_bg_color(wifi_go_key, C(ok ? NAV_CUR : SURFACE2), 0);
    lv_obj_set_style_border_color(wifi_go_key, C(ok ? NAV_CUR : LINE), 0);
    lv_obj_t *lab = lv_obj_get_child(wifi_go_key, 0);
    if (lab) lv_obj_set_style_text_color(lab, C(ok ? BG : DIM), 0);
}
static void wifi_field_draw(void)
{
    if (!wifi_field) return;
    lv_obj_clean(wifi_field);
    const int maxw = 400;
    int cx = 12;
    if (!wifi_pw_len) {
        text_in(wifi_field, "Enter password", 12, 0, 300, 34, F12, DIM, LV_TEXT_ALIGN_LEFT);
    } else if (!wifi_show_pw) {
        const int vis = wifi_pw_len > 34 ? 34 : wifi_pw_len;
        for (int i = 0; i < vis; ++i) box(wifi_field, 12 + i * 11, 14, 6, 6, WHITE, 3);
        cx = 12 + vis * 11;
    } else {
        const char *s = wifi_pw;
        lv_obj_t *t = text(wifi_field, s, 12, 0, F14B, WHITE);
        while (*s && width_of(t) > maxw) { ++s; lv_label_set_text(t, s); }
        lv_obj_set_y(t, (34 - lh(F14B)) / 2);
        cx = 12 + width_of(t) + 2;
    }
    box(wifi_field, cx, 9, 2, 16, NAV_CUR, 1);
    wifi_go_style();
}
static lv_obj_t *wifi_count_lbl;

static void wifi_count_update(void)
{
    if (!wifi_count_lbl) return;
    char c[16]; snprintf(c, sizeof(c), "%d / %d", wifi_pw_len, HMI_WIFI_PASS_MAX);
    lv_label_set_text(wifi_count_lbl, c);
}
static void wifi_type(int c)
{
    if (wifi_pw_len >= HMI_WIFI_PASS_MAX) return;
    wifi_pw[wifi_pw_len++] = (char)c; wifi_pw[wifi_pw_len] = '\0';
}
static void wifi_key(int code)
{
    if (code >= 32 && code < 127) {
        if (wifi_shift && code >= 'a' && code <= 'z') code -= 32;
        wifi_type(code);
        if (wifi_shift) { wifi_shift = false; wifi_redraw(); return; }
    } else if (code == KEY_BKSP) {
        if (wifi_pw_len) wifi_pw[--wifi_pw_len] = '\0';
    } else if (code == KEY_SHIFT) {
        if (!wifi_sym) { wifi_shift = !wifi_shift; wifi_redraw(); }
        return;
    } else if (code == KEY_SYM) {
        wifi_sym = !wifi_sym; wifi_shift = false; wifi_redraw();
        return;
    } else if (code == KEY_GO) {
        if (!wifi_target_secure || wifi_pw_len >= HMI_WIFI_PASS_MIN) wifi_do_connect();
        return;
    }
    wifi_field_draw();
    wifi_count_update();
}
static void wifi_key_cb(lv_event_t *e) { wifi_key((int)(uintptr_t)lv_event_get_user_data(e)); }
static void wifi_eye_cb(lv_event_t *e) { (void)e; wifi_show_pw = !wifi_show_pw; wifi_redraw(); }

typedef struct { int code; int w; } kkey_t;
static lv_style_t ks_key, ks_key_pr, ks_lbl;
static bool ks_ready;
static void kb_styles_init(void)
{
    if (ks_ready) return;
    ks_ready = true;
    lv_style_init(&ks_key);
    lv_style_set_bg_color(&ks_key, C(SURFACE2)); lv_style_set_bg_opa(&ks_key, LV_OPA_COVER);
    lv_style_set_radius(&ks_key, 6);
    lv_style_set_border_width(&ks_key, 1); lv_style_set_border_color(&ks_key, C(LINE));
    lv_style_set_pad_all(&ks_key, -1);
    lv_style_init(&ks_key_pr);
    lv_style_set_bg_opa(&ks_key_pr, LV_OPA_70);
    lv_style_init(&ks_lbl);
    lv_style_set_text_font(&ks_lbl, F12B); lv_style_set_text_color(&ks_lbl, C(TXT2));
    lv_style_set_text_align(&ks_lbl, LV_TEXT_ALIGN_CENTER);
}
static void kb_btn(lv_obj_t *l, int x, int y, int w, int code)
{
    const bool go = code == KEY_GO, shift = code == KEY_SHIFT, space = code == ' ';
    const bool shift_on = shift && wifi_shift;
    /* ~50 keys: share styles (no per-object style allocation) to stay inside LVGL's 64 KB heap */
    kb_styles_init();
    lv_obj_t *b = lv_button_create(l);
    lv_obj_remove_style_all(b);
    lv_obj_set_pos(b, x, y);
    lv_obj_set_size(b, w, 38);
    lv_obj_set_scrollable(b, false);
    lv_obj_add_style(b, &ks_key, 0);
    lv_obj_add_style(b, &ks_key_pr, LV_STATE_PRESSED);
    lv_obj_add_event_cb(b, wifi_key_cb, LV_EVENT_CLICKED, (void *)(uintptr_t)code);
    if (go) { lv_obj_set_style_bg_color(b, C(NAV_CUR), 0); lv_obj_set_style_border_color(b, C(NAV_CUR), 0); }
    if (shift_on) { lv_obj_set_style_bg_color(b, C(NAV_BOX), 0); lv_obj_set_style_border_color(b, C(NAV_CUR), 0); }
    if (code == KEY_SHIFT) icon_c(b, &hmi_icon_shift_14, w / 2, 19, wifi_sym ? 0x2B3648 : shift_on ? NAV_CUR : LABEL);
    else if (code == KEY_BKSP) icon_c(b, &hmi_icon_bksp_16, w / 2, 19, LABEL);
    else {
        char s[12];
        bool small = false; uint32_t fg = TXT2;
        if (code == KEY_SYM) { snprintf(s, sizeof(s), "%s", wifi_sym ? "ABC" : "?123"); small = true; fg = LABEL; }
        else if (go) { snprintf(s, sizeof(s), "Connect"); fg = BG; }
        else if (space) { snprintf(s, sizeof(s), "space"); small = true; fg = DIM; }
        else {
            int ch = code;
            if (wifi_shift && ch >= 'a' && ch <= 'z') ch -= 32;
            snprintf(s, sizeof(s), "%c", ch);
        }
        lv_obj_t *t = lv_label_create(b);
        lv_label_set_text(t, s);
        lv_obj_add_style(t, &ks_lbl, 0);
        if (small) lv_obj_set_style_text_font(t, F10, 0);
        if (fg != TXT2) lv_obj_set_style_text_color(t, C(fg), 0);
        lv_obj_set_size(t, w, lh(small ? F10 : F12B));
        lv_obj_set_pos(t, 0, (38 - lh(small ? F10 : F12B)) / 2);
    }
    if (go) wifi_go_key = b;
}
/* Full-width key row, weights as in the concept; fill=false keeps the 10-key unit width. */
static void wifi_kb_row(lv_obj_t *l, int y, const kkey_t *k, int n, bool fill)
{
    int total = 0;
    for (int i = 0; i < n; ++i) total += k[i].w;
    const int avail = fill ? 472 - 3 * (n - 1) : 445;
    if (!fill) total = 100;
    int acc = 0, x = 4;
    for (int i = 0; i < n; ++i) {
        const int w = (acc + k[i].w) * avail / total - acc * avail / total;
        acc += k[i].w;
        if (k[i].code != KEY_NONE) kb_btn(l, x, y, w, k[i].code);
        x += w + 3;
    }
}

/* Letter/digit rows are one lv_buttonmatrix each (a handful of objects instead of ~10 buttons). */
static lv_style_t ms_main, ms_item, ms_item_pr;
static bool ms_ready;
static char kb_txt[4][10][2];
static const char *kb_map[4][11];
static void kb_matrix_cb(lv_event_t *e)
{
    lv_obj_t *m = lv_event_get_target_obj(e);
    const uint32_t id = lv_buttonmatrix_get_selected_button(m);
    if (id == LV_BUTTONMATRIX_BUTTON_NONE) return;
    const char *t = lv_buttonmatrix_get_button_text(m, id);
    if (t && t[0]) wifi_key((unsigned char)t[0]);
}
static void kb_matrix(lv_obj_t *l, int x, int y, int w, int row, const char *chars)
{
    if (!ms_ready) {
        ms_ready = true;
        lv_style_init(&ms_main);
        lv_style_set_bg_opa(&ms_main, LV_OPA_TRANSP); lv_style_set_border_width(&ms_main, 0);
        lv_style_set_pad_all(&ms_main, 0); lv_style_set_pad_gap(&ms_main, 3); lv_style_set_radius(&ms_main, 0);
        lv_style_init(&ms_item);
        lv_style_set_bg_color(&ms_item, C(SURFACE2)); lv_style_set_bg_opa(&ms_item, LV_OPA_COVER);
        lv_style_set_radius(&ms_item, 6);
        lv_style_set_border_width(&ms_item, 1); lv_style_set_border_color(&ms_item, C(LINE));
        lv_style_set_text_font(&ms_item, F12B); lv_style_set_text_color(&ms_item, C(TXT2));
        lv_style_init(&ms_item_pr);
        lv_style_set_bg_opa(&ms_item_pr, LV_OPA_70);
    }
    const int n = (int)strlen(chars);
    for (int i = 0; i < n; ++i) {
        kb_txt[row][i][0] = (wifi_shift && chars[i] >= 'a' && chars[i] <= 'z') ? (char)(chars[i] - 32) : chars[i];
        kb_txt[row][i][1] = '\0';
        kb_map[row][i] = kb_txt[row][i];
    }
    kb_map[row][n] = "";
    lv_obj_t *m = lv_buttonmatrix_create(l);
    lv_obj_remove_style_all(m);
    lv_obj_add_style(m, &ms_main, 0);
    lv_obj_add_style(m, &ms_item, LV_PART_ITEMS);
    lv_obj_add_style(m, &ms_item_pr, LV_PART_ITEMS | LV_STATE_PRESSED);
    lv_obj_set_pos(m, x, y);
    lv_obj_set_size(m, w, 38);
    lv_buttonmatrix_set_map(m, kb_map[row]);
    lv_obj_add_event_cb(m, kb_matrix_cb, LV_EVENT_VALUE_CHANGED, NULL);
}

static void wifi_view_pass(lv_obj_t *l)
{
    char sub[48]; const int r = target_rssi();
    if (r) snprintf(sub, sizeof(sub), "Secured \xC2\xB7 Signal %d dBm", r); else snprintf(sub, sizeof(sub), "Secured network");
    wifi_header(l, wifi_target, sub);
    lv_obj_t *pl = text(l, "PASSWORD", 8, 46, F10, LABEL);
    spacing(pl, 1);
    char c[16]; snprintf(c, sizeof(c), "%d / %d", wifi_pw_len, HMI_WIFI_PASS_MAX);
    wifi_count_lbl = text_in(l, c, 372, 46, 100, 13, F10, DIM, LV_TEXT_ALIGN_RIGHT);
    lv_obj_t *f = card_box(l, 8, 62, 464, 34, CARD, 10, NAV_CUR);
    wifi_field = obj(f, 0, 0, 440, 34);
    lv_obj_t *ey = button(f, 420, 0, 44, 34, CARD, 10, wifi_eye_cb, NULL);
    lv_obj_set_style_bg_opa(ey, LV_OPA_TRANSP, 0);
    icon_c(ey, wifi_show_pw ? &hmi_icon_eyeoff_14 : &hmi_icon_eye_14, 22, 17, wifi_show_pw ? NAV_CUR : LABEL);

    static const char *const nrm[4] = {"1234567890", "qwertyuiop", "asdfghjkl", "zxcvbnm"};
    static const char *const sym[4] = {"!@#$%^&*()", "-_=+[]{}\\|", ";:'\"/?<>~", "`"};
    const char *const *rows = wifi_sym ? sym : nrm;
    kb_matrix(l, 4, 112, 472, 0, wifi_sym ? sym[0] : nrm[0]);
    kb_matrix(l, 4, 153, 472, 1, rows[1]);
    kb_matrix(l, 4, 194, 472, 2, rows[2]);
    kb_btn(l, 4, 235, 67, KEY_SHIFT);
    kb_btn(l, 409, 235, 67, KEY_BKSP);
    if (!wifi_sym) kb_matrix(l, 74, 235, 331, 3, nrm[3]);
    else kb_matrix(l, 74, 235, 44, 3, sym[3]);
    const kkey_t last[5] = {{KEY_SYM,16},{',',10},{' ',46},{'.',10},{KEY_GO,24}};
    wifi_go_key = NULL;
    wifi_kb_row(l, 276, last, 5, true);
    wifi_field_draw();
}

/* ---- busy / info / fail views */
static void wifi_view_busy(lv_obj_t *l)
{
    wifi_header(l, "Connecting", wifi_target);
    wifi_spinner(l, &hmi_icon_spin_44, 240, 72, NAV_CUR);
    text_in(l, "Joining network...", 0, 104, 480, 17, F14B, WHITE, LV_TEXT_ALIGN_CENTER);
    text_in(l, "Please wait, usually under 10 s", 0, 127, 480, 13, F10, LABEL, LV_TEXT_ALIGN_CENTER);
    static const char *const names[3] = {"Associating", "Authenticating", "Obtaining IP address"};
    const int step = ws.state == HMI_WIFI_CONNECTING ? ws.step : 0;
    for (int k = 0; k < 3; ++k) {
        const int y = 168 + k * 22;
        if (k < step) {
            box(l, 90, y, 16, 16, OK_BG, 8);
            icon_c(l, &hmi_icon_check_11, 98, y + 8, OK_COL);
        } else if (k == step) {
            wifi_spinner(l, &hmi_icon_spin_16, 98, y + 8, NAV_CUR);
        } else {
            lv_obj_t *o = obj(l, 90, y, 16, 16);
            lv_obj_set_style_radius(o, 8, 0);
            lv_obj_set_style_border_width(o, 2, 0);
            lv_obj_set_style_border_color(o, C(0x2B3648), 0);
        }
        text_in(l, names[k], 116, y, 270, 16, k == step ? F12B : F12, k < step ? TXT2 : k == step ? WHITE : DIM, LV_TEXT_ALIGN_LEFT);
    }
    (void)wifi_btn(l, 150, 268, 180, "CANCEL", SURFACE2, TXT2, true, wifi_back_cb);
}
static void wifi_view_info(lv_obj_t *l)
{
    wifi_header(l, "Wi-Fi", "Connected");
    lv_obj_t *c = card_box(l, 8, 48, 464, 62, OK_BG, 12, OK_COL);
    box(c, 14, 13, 36, 36, 0x0F3A22, 18);
    icon_c(c, &hmi_icon_wifi_22, 32, 31, OK_COL);
    text_in(c, ws.ssid, 62, 15, 290, 17, F14B, WHITE, LV_TEXT_ALIGN_LEFT);
    text_in(c, "Station connected", 62, 34, 290, 13, F10, TXT2, LV_TEXT_ALIGN_LEFT);
    chip(c, 450, 22, "ONLINE", OK_COL, 0x0F3A22);
    char sg[40]; snprintf(sg, sizeof(sg), "%d dBm \xC2\xB7 %s", (int)ws.rssi, rssi_word(ws.rssi));
    wifi_cell(l, 8, 118, "IP ADDRESS", ws.ip);
    wifi_cell(l, 243, 118, "GATEWAY", ws.gateway);
    wifi_cell(l, 8, 168, "SIGNAL", sg);
    wifi_cell(l, 243, 168, "MAC", ws.mac);

    hmi_ota_status_t ota;
    hmi_ota_get_status(&ota);
    char ota_url[64];
    snprintf(ota_url, sizeof(ota_url), "http://%s/update", ws.ip);
    char slot_str[80];
    snprintf(slot_str, sizeof(slot_str), "%s (Target: %s)", ota.running_slot, ota.next_slot);
    wifi_cell(l, 8, 218, "WEB OTA", ota_url);
    wifi_cell(l, 243, 218, "DUAL-OTA SLOT", slot_str);

    (void)wifi_btn(l, 8, 268, 228, "FORGET NETWORK", SURFACE2, TXT2, true, wifi_forget_cb);
    (void)wifi_btn(l, 244, 268, 228, "DONE", START_COL, 0xFFFFFF, false, wifi_done_cb);
}
static void wifi_view_fail(lv_obj_t *l)
{
    static const char *const title[5] = {"Connection failed", "Wrong password", "Network not found", "No IP address received", "Connection failed"};
    static const char *const msg[5] = {
        "The Wi-Fi driver reported an error. Try again.",
        "The access point rejected the credentials. Check the password and try again.",
        "The access point is out of range or was renamed. Move closer or scan again.",
        "Joined the access point, but DHCP did not answer. Check the router.",
        "The Wi-Fi driver reported an error. Try again."};
    const int r = ws.fail_reason < 5 ? ws.fail_reason : 4;
    wifi_header(l, "Connection failed", wifi_target);
    lv_obj_t *c = card_box(l, 8, 48, 464, 62, FAULT_BG, 12, FAULT_COL);
    box(c, 14, 13, 36, 36, 0x4A1624, 18);
    icon_c(c, &hmi_icon_warn_20, 32, 31, FAULT_COL);
    text_in(c, title[r], 62, 9, 390, 17, F14B, WHITE, LV_TEXT_ALIGN_LEFT);
    lv_obj_t *m = text(c, msg[r], 62, 28, F10, TXT2);
    lv_label_set_long_mode(m, LV_LABEL_LONG_MODE_WRAP);
    lv_obj_set_width(m, 388);
    char at[16]; snprintf(at, sizeof(at), "%u", (unsigned)(ws.attempt ? ws.attempt : 1));
    char sg[24]; const int tr = target_rssi();
    if (tr) snprintf(sg, sizeof(sg), "%d dBm", tr); else snprintf(sg, sizeof(sg), "\xE2\x80\x94");
    wifi_cell(l, 8, 118, "ATTEMPT", at);
    wifi_cell(l, 243, 118, "SIGNAL", sg);
    if (r == HMI_WIFI_FAIL_PASSWORD)
        text_in(l, "Passwords are case-sensitive. WPA2 passwords have 8 to 63 characters.", 12, 172, 456, 14, F10, LABEL, LV_TEXT_ALIGN_LEFT);
    (void)wifi_btn(l, 8, 268, 228, "CHOOSE OTHER", SURFACE2, TXT2, true, wifi_done_cb);
    (void)wifi_btn(l, 244, 268, 228, "RETRY", START_COL, 0xFFFFFF, false, wifi_retry_cb);
}

static void wifi_render(void)
{
    if (!wifi_layer) return;
    lv_obj_clean(wifi_layer);
    wifi_field = NULL; wifi_go_key = NULL; wifi_count_lbl = NULL;
    switch (wifi_view) {
    case WV_PASS: wifi_view_pass(wifi_layer); break;
    case WV_BUSY: wifi_view_busy(wifi_layer); break;
    case WV_INFO: wifi_view_info(wifi_layer); break;
    case WV_FAIL: wifi_view_fail(wifi_layer); break;
    default: wifi_view_list(wifi_layer); break;
    }
    wifi_sig = ws.version * 16u + (uint32_t)wifi_view * 4u + (ws.state == HMI_WIFI_CONNECTING ? ws.step : 0u) + ws.state * 1000003u;
}
static void wifi_tick(lv_timer_t *t)
{
    (void)t;
    if (!wifi_layer) return;
    hmi_wifi_get(&ws);
    if (wifi_view == WV_BUSY && ws.version != wifi_conn_ver) {
        if (ws.state == HMI_WIFI_CONNECTED) wifi_view = WV_INFO;
        else if (ws.state == HMI_WIFI_FAILED) wifi_view = WV_FAIL;
    }
    if (wifi_view == WV_INFO && ws.state != HMI_WIFI_CONNECTED) wifi_view = WV_LIST;
    if (wifi_view == WV_PASS) return;
    const uint32_t sig = ws.version * 16u + (uint32_t)wifi_view * 4u + (ws.state == HMI_WIFI_CONNECTING ? ws.step : 0u) + ws.state * 1000003u;
    if (sig == wifi_sig) return;
    for (lv_indev_t *i = lv_indev_get_next(NULL); i; i = lv_indev_get_next(i))
        if (lv_indev_get_state(i) == LV_INDEV_STATE_PRESSED) return;
    wifi_render();
}

static void wifi_open(void)
{
    if (wifi_layer) return;
    close_modal();
    scroll_pos[SCR_WIFI] = 0;
    wifi_view = WV_LIST;
    lv_obj_clean(content);   /* free the page underneath: the overlay replaces it, and LVGL's heap is small */
    wifi_layer = box(screen, 0, 0, 480, 320, BG, 0);
    lv_obj_set_clickable(wifi_layer, true);
    lv_obj_move_foreground(wifi_layer);
    hmi_wifi_scan();
    hmi_wifi_get(&ws);
    wifi_render();
    wifi_timer = lv_timer_create(wifi_tick, 250, NULL);
}
void hmi_ui_open_wifi(void) { wifi_open(); }
#ifdef HMI_SIM
void hmi_ui_wifi_inject(const char *s) { for (; *s; ++s) wifi_type((unsigned char)*s); wifi_field_draw(); wifi_count_update(); }
#endif

/* ------------------------------------------------------------------ Chrome */
static void goto_page(page_t n)
{
    close_modal();
    memset(scroll_pos, 0, sizeof(scroll_pos));
    page = n;
    dirty = true;
    (void)lv_async_call(redraw_cb, NULL);
}
static void nav_click(lv_event_t *e) { goto_page((page_t)(uintptr_t)lv_event_get_user_data(e)); }
static void fault_click(lv_event_t *e) { (void)e; goto_page(FAULTS); }
static void dev_click(lv_event_t *e) { (void)e; device_tab = 0; goto_page(DEVICE); }
static void link_click(lv_event_t *e) { (void)e; device_tab = 1; goto_page(DEVICE); }
static void banner_click(lv_event_t *e)
{
    if (banner_target == FAULTS) goto_page(FAULTS);
    else if (banner_target == DEVICE) link_click(e);
}

static void build_chrome(void)
{
    box(screen, 0, 0, 56, 320, 0x06090E, 0);
    box(screen, 55, 0, 1, 320, LINE, 0);

    for (int i = 0; i < 5; ++i) {
        rail_tab[i] = button(screen, 4, 4 + i * 63, 47, 60, 0x06090E, 10, nav_click, (void *)(uintptr_t)i);
        lv_obj_set_style_bg_opa(rail_tab[i], LV_OPA_TRANSP, 0);
        rail_mark[i] = box(screen, 0, 4 + i * 63 + 14, 3, 32, NAV_CUR, 2);
        rail_icon[i] = icon(rail_tab[i], nav_icon[i], 14, 12, DIM);
        rail_label[i] = text(rail_tab[i], nav_name[i], 0, 35, F10, DIM);
        lv_obj_set_clickable(rail_label[i], false);
        rail_badge[i] = NULL;
    }
    rail_badge[FAULTS] = box(rail_tab[FAULTS], 31, 8, 8, 8, WARN_COL, 4);
    lv_obj_set_style_outline_width(rail_badge[FAULTS], 2, 0);
    lv_obj_set_style_outline_color(rail_badge[FAULTS], C(0x06090E), 0);

    hdr_dev = button(screen, 64, 4, 110, 24, BG, 6, dev_click, NULL);
    lv_obj_set_style_bg_opa(hdr_dev, LV_OPA_TRANSP, 0);
    lv_obj_t *n1 = text(hdr_dev, "NODE", 0, 0, F12B, LABEL);
    spacing(n1, 1);
    lv_obj_set_clickable(n1, false);
    const int nw = width_of(n1);
    lv_obj_set_pos(n1, 0, (24 - lh(F12B)) / 2);
    lv_obj_t *n2 = text(hdr_dev, "RCC-01", nw + 8, 0, F12B, TXT2);
    spacing(n2, 1);
    lv_obj_set_clickable(n2, false);
    lv_obj_set_pos(n2, nw + 8, (24 - lh(F12B)) / 2);
    hdr_dev_w = nw + 8 + width_of(n2);
    lv_obj_set_width(hdr_dev, hdr_dev_w);

    hdr_pill = button(screen, 239, 5, 123, 23, CARD, 10, link_click, NULL);
    lv_obj_set_style_border_width(hdr_pill, 1, 0);
    lv_obj_set_style_border_color(hdr_pill, C(LINE), 0);
    lv_obj_set_style_pad_all(hdr_pill, -1, 0);
    hdr_dot = box(hdr_pill, 11, 8, 7, 7, OK_COL, 4);
    hdr_text = text(hdr_pill, "", 24, 0, F12B, OK_COL);
    lv_obj_set_clickable(hdr_text, false);

    hdr_fault_box = button(screen, 438, 4, 34, 24, CARD, 8, fault_click, NULL);
    lv_obj_set_style_border_width(hdr_fault_box, 1, 0);
    lv_obj_set_style_border_color(hdr_fault_box, C(LINE), 0);
    lv_obj_set_style_pad_all(hdr_fault_box, -1, 0);
    hdr_fault_icon = icon(hdr_fault_box, &hmi_icon_checkc_15, 9, 5, DIM);
    lv_obj_set_clickable(hdr_fault_icon, false);

    banner_box = button(screen, 64, 32, 408, 20, OFF_BG, 6, banner_click, NULL);
    lv_obj_set_style_border_width(banner_box, 1, 0);
    lv_obj_set_style_border_color(banner_box, C(0xFFFFFF), 0);
    lv_obj_set_style_border_opa(banner_box, 26, 0);
    lv_obj_set_style_pad_all(banner_box, -1, 0);
    banner_icon = icon(banner_box, &hmi_icon_warn_12, 0, 0, WARN_COL);
    banner_text = text(banner_box, "", 0, 0, F10, WARN_COL);
    lv_obj_set_clickable(banner_icon, false);
    lv_obj_set_clickable(banner_text, false);
    lv_obj_set_hidden(banner_box, true);
}

static void update_chrome(const rcc_snapshot_t *m, const char *banner, kind_t banner_kind, int target)
{
    const fault_state_t fs = fault_state(m);
    const uint32_t badge = (fs == FS_FAULT) ? FAULT_COL : WARN_COL;
    for (int i = 0; i < 5; ++i) {
        const bool on = i == (int)page;
        lv_obj_set_style_image_recolor(rail_icon[i], C(on ? NAV_CUR : DIM), 0);
        lv_obj_set_style_text_color(rail_label[i], C(on ? WHITE : DIM), 0);
        lv_obj_set_hidden(rail_mark[i], !on);
        if (on) {
            lv_obj_set_style_bg_color(rail_tab[i], C(CARD), 0);
            lv_obj_set_style_bg_opa(rail_tab[i], LV_OPA_COVER, 0);
        } else {
            lv_obj_set_style_bg_opa(rail_tab[i], LV_OPA_TRANSP, 0);
        }
        lv_obj_set_pos(rail_label[i], (47 - width_of(rail_label[i])) / 2, 35 + (13 - lh(F10)) / 2);
    }
    lv_obj_set_style_bg_color(rail_badge[FAULTS], C(badge), 0);
    lv_obj_set_hidden(rail_badge[FAULTS], !(fs == FS_FAULT || fs == FS_INHIBIT));

    char b[40], t[24];
    uint32_t pc = LABEL;
    if (!link_started) { snprintf(b, sizeof(b), "UART FAILED"); pc = FAULT_COL; }
    else if (!m->online) { fmt_age(t, sizeof(t), m->reply_age_ms); snprintf(b, sizeof(b), "OFFLINE \xC2\xB7 %s", t); pc = OFF_COL; }
    else { fmt_age(t, sizeof(t), m->reply_age_ms); snprintf(b, sizeof(b), "ONLINE \xC2\xB7 %s", t); pc = OK_COL; }
    lv_label_set_text(hdr_text, b);
    lv_obj_set_style_text_color(hdr_text, C(pc), 0);
    lv_obj_set_style_bg_color(hdr_dot, C(pc), 0);
    const int tw = width_of(hdr_text), pw = tw + 35;
    lv_obj_set_size(hdr_pill, pw, 23);
    lv_obj_set_pos(hdr_text, 24, (23 - lh(F12B)) / 2);
    lv_obj_set_x(hdr_pill, 64 + hdr_dev_w + (408 - hdr_dev_w - pw - 34) / 2);

    const uint32_t fbg = (fs == FS_FAULT) ? FAULT_BG : (fs == FS_INHIBIT) ? WARN_BG : CARD;
    lv_obj_set_style_bg_color(hdr_fault_box, C(fbg), 0);
    lv_image_set_src(hdr_fault_icon, (fs == FS_OK) ? &hmi_icon_checkc_15 : &hmi_icon_warn_15);
    lv_obj_set_style_image_recolor(hdr_fault_icon, C(fs == FS_FAULT ? FAULT_COL : fs == FS_INHIBIT ? WARN_COL : fs == FS_UNKNOWN ? OFF_COL : OK_COL), 0);

    banner_target = target;
    if (banner) {
        const uint32_t fg = (banner_kind == K_FAULT) ? FAULT_COL : (banner_kind == K_WARN) ? WARN_COL : OFF_COL;
        const uint32_t bg_c = (banner_kind == K_FAULT) ? FAULT_BG : (banner_kind == K_WARN) ? WARN_BG : OFF_BG;
        lv_obj_set_hidden(banner_box, false);
        lv_obj_set_clickable(banner_box, target >= 0);
        lv_obj_set_style_bg_color(banner_box, C(bg_c), 0);
        lv_label_set_text(banner_text, banner);
        lv_obj_set_style_text_color(banner_text, C(fg), 0);
        lv_obj_set_style_image_recolor(banner_icon, C(fg), 0);
        const int bw = 12 + 6 + width_of(banner_text), x0 = (408 - bw) / 2;
        lv_obj_set_pos(banner_icon, x0, (20 - 12) / 2);
        lv_obj_set_pos(banner_text, x0 + 18, (20 - lh(F10)) / 2);
    } else {
        lv_obj_set_hidden(banner_box, true);
    }
}

/* ------------------------------------------------------------------ Rebuild */
static void rebuild(void)
{
    rcc_snapshot_t m;
    rcc_link_snapshot(&m);
    const char *banner = NULL;
    kind_t bk = K_NORMAL;
    int target = -1;
    char btext[128];
    bool two_line_banner = false;
    if (!link_started) { banner = "UART INIT FAILED \xC2\xB7 NO CONTROLLER LINK"; bk = K_FAULT; }
    else if (!m.online) { banner = "LINK LOST \xC2\xB7 CHARGING MAY CONTINUE"; bk = K_OFF; target = DEVICE; }
    else if (no_authority(&m) && m.command_age_ms < 15000u) { banner = "HMI HAS NO CONTROL AUTHORITY"; bk = K_WARN; }
    else if (fresh(&m) && m.top_state == 6) {
        snprintf(btext, sizeof(btext), "NEW FAULT: %s \xC2\xB7 TAP TO VIEW", fault_name(m.primary_fault));
        banner = btext; bk = K_FAULT; target = FAULTS;
    }
    /* Shared chrome keeps source identity on all five pages, including stale data.
     * Retain safety/link warnings as a second line in the existing banner. */
    if (m.simulated_measurements) {
        const bool active_demo = fresh(&m) && m.top_state == 4 &&
                                 m.operational_state >= 2 && m.operational_state <= 6;
        const char *source = !fresh(&m) ? "SIMULATED V/I (STALE) | REAL relay cmd, not contacts" :
                             (!m.simulated_ready && !active_demo) ? "SIMULATED V/I (WAIT) | REAL relay cmd, not contacts" :
                                                  "SIMULATED V/I | REAL relay cmd, not contacts";
        if (banner) {
            char warning[sizeof(btext)];
            snprintf(warning, sizeof(warning), "%s", banner);
            snprintf(btext, sizeof(btext), "%s\n%.70s", source, warning);
            banner = btext;
            two_line_banner = true;
        } else {
            banner = source;
            bk = K_WARN;
            target = DEVICE;
        }
    }
    lv_obj_set_height(banner_box, two_line_banner ? 34 : 20);
    update_chrome(&m, banner, bk, target);

    bar_spec_t spec; bar_spec(&m, &spec);
    char sig[sizeof(bar_signature)];
    snprintf(sig, sizeof(sig), "%d|%d|%d|%s|%s|%u", (int)spec.kind, (int)spec.button, (int)page, spec.title, spec.sub, (unsigned)spec.title_color);
    if (strcmp(sig, bar_signature) != 0) { memcpy(bar_signature, sig, sizeof(sig)); build_bar(&spec); }

    lv_obj_clean(content);
    const int top = banner ? (two_line_banner ? 70 : 56) : 34;
    const int bottom = (spec.kind == BAR_NONE) ? 312 : 264;
    lv_obj_t *p = obj(content, 64, top, 408, bottom - top);
    switch (page) {
    case OVERVIEW: {
        int reason = 0;
        if (spec.kind == BAR_FULL && spec.button == B_STOP && !start_allowed(&m)) {
            reason = !fresh(&m) ? 1 : (m.top_state == 6 ? 2 : 0);
        }
        page_overview(p, &m, bottom - top, reason);
        break; }
    case MEASURE: page_measure(p, &m, bottom - top); break;
    case FAULTS: page_faults(p, &m, bottom - top); break;
    case SESSION: page_session(p, &m, bottom - top); break;
    default: page_device(p, &m, bottom - top); break;
    }
    dirty = false;
}
static void redraw_cb(void *unused) { (void)unused; rebuild(); }

static lv_obj_t *s_ota_modal = NULL;
static lv_obj_t *s_ota_bar = NULL;
static lv_obj_t *s_ota_label = NULL;

static void update_ota_overlay(void)
{
    hmi_ota_status_t ota;
    hmi_ota_get_status(&ota);
    if (ota.state == HMI_OTA_RECEIVING || ota.state == HMI_OTA_VERIFYING || ota.state == HMI_OTA_SUCCESS) {
        if (!s_ota_modal) {
            s_ota_modal = box(screen, 30, 50, 420, 220, 0x070D19, 14);
            lv_obj_set_style_border_color(s_ota_modal, lv_color_hex(0x0284C7), LV_PART_MAIN);
            lv_obj_set_style_border_width(s_ota_modal, 2, LV_PART_MAIN);
            lv_obj_move_foreground(s_ota_modal);

            text_in(s_ota_modal, "FIRMWARE UPDATE IN PROGRESS", 20, 20, 380, 22, F16B, 0x38BDF8, LV_TEXT_ALIGN_CENTER);
            text_in(s_ota_modal, "Target slot: writing to flash partition", 20, 50, 380, 16, F12, 0x94A3B8, LV_TEXT_ALIGN_CENTER);
            text_in(s_ota_modal, "DO NOT DISCONNECT POWER", 20, 75, 380, 18, F12B, 0xF59E0B, LV_TEXT_ALIGN_CENTER);

            lv_obj_t *bg = box(s_ota_modal, 30, 115, 360, 20, 0x1E293B, 8);
            s_ota_bar = box(bg, 0, 0, 4, 20, 0x38BDF8, 8);
            s_ota_label = text_in(s_ota_modal, "Writing: 0%", 20, 150, 380, 20, F14B, 0xFFFFFF, LV_TEXT_ALIGN_CENTER);
        }
        if (s_ota_modal) {
            int w = (ota.progress_pct * 360) / 100;
            if (w < 4) w = 4;
            lv_obj_set_width(s_ota_bar, w);
            char txt[64];
            if (ota.state == HMI_OTA_SUCCESS) {
                snprintf(txt, sizeof(txt), "Done 100%%! Rebooting in 2s...");
            } else if (ota.state == HMI_OTA_VERIFYING) {
                snprintf(txt, sizeof(txt), "Verifying SHA256 checksum...");
            } else {
                snprintf(txt, sizeof(txt), "Writing flash: %u%% (%zu / %zu KB)",
                         (unsigned)ota.progress_pct, ota.bytes_written / 1024, ota.total_bytes / 1024);
            }
            lv_label_set_text(s_ota_label, txt);
        }
    } else if (s_ota_modal && ota.state == HMI_OTA_IDLE) {
        lv_obj_delete_async(s_ota_modal);
        s_ota_modal = NULL;
    }
}

static void refresh(lv_timer_t *timer)
{
    (void)timer;

    /* Task Watchdog Timer (TWDT) monitoring for LVGL task */
    static bool s_lvgl_wdt_added = false;
    if (!s_lvgl_wdt_added) {
        if (esp_task_wdt_add(NULL) == ESP_OK) {
            s_lvgl_wdt_added = true;
            ESP_LOGI("hmi_ui", "LVGL task subscribed to TWDT (5000 ms)");
        }
    }
    if (s_lvgl_wdt_added) {
        esp_task_wdt_reset();
    }

    /* Screen saver / backlight sleep check */
    rcc_snapshot_t m_now;
    rcc_link_snapshot(&m_now);
    const bool in_session = fresh(&m_now) && m_now.top_state == 4 &&
                            m_now.operational_state >= 2 && m_now.operational_state <= 6;
    hmi_backlight_tick(!in_session);

    update_ota_overlay();

    for (lv_indev_t *i = lv_indev_get_next(NULL); i; i = lv_indev_get_next(i)) {
        if (lv_indev_get_state(i) == LV_INDEV_STATE_PRESSED) return;
    }
    if (modal || wifi_layer) return;
    rebuild();
}

void hmi_ui_start(bool started)
{
    link_started = started;
    screen = lv_screen_active();
    lv_obj_remove_style_all(screen);
    lv_obj_set_style_bg_color(screen, C(BG), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
    lv_obj_set_scrollable(screen, false);

    build_chrome();
    bar_box = obj(screen, 64, 270, 408, 42);
    content = obj(screen, 0, 0, 480, 320);
    lv_obj_move_background(content);
    lv_timer_create(refresh, 1000, NULL);
    rebuild();
}

void hmi_ui_select_page(int page_idx, int sub_tab)
{
    if (page_idx >= 0 && page_idx <= 4) {
        device_tab = sub_tab;
        goto_page((page_t)page_idx);
    }
}
