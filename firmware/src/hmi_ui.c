/*
 * HMI pages. Geometry, fonts and colours follow docs/mockups/hmi-ui-landscape.html
 * (480x320 landscape). Render on the PC with firmware/tools/sim to compare.
 * Called only under the LVGL port lock.
 */
#include "hmi_ui.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "hmi_icons.h"
#include "lvgl.h"
#include "rcc_link.h"

LV_FONT_DECLARE(hmi_f10);
LV_FONT_DECLARE(hmi_f12);
LV_FONT_DECLARE(hmi_f12b);
LV_FONT_DECLARE(hmi_f14b);
LV_FONT_DECLARE(hmi_f16b);
LV_FONT_DECLARE(hmi_f20b);
LV_FONT_DECLARE(hmi_f22b);
LV_FONT_DECLARE(hmi_f28b);
LV_FONT_DECLARE(hmi_hero28);
LV_FONT_DECLARE(hmi_hero40);
LV_FONT_DECLARE(hmi_hero48);

#define F10 (&hmi_f10)
#define F12 (&hmi_f12)
#define F12B (&hmi_f12b)
#define F14B (&hmi_f14b)
#define F16B (&hmi_f16b)
#define F20B (&hmi_f20b)
#define F22B (&hmi_f22b)
#define F28B (&hmi_f28b)

/* Palette (from the watch photo; see the mockup :root). */
#define BG 0x000000
#define CARD 0x1A1A1F
#define CARD2 0x24242B
#define WHITE 0xFFFFFF
#define TXT2 0xD8D8E0
#define LABEL 0x9A9A9A
#define DIM 0x86868F
#define SUN 0xF5C518        /* warn */
#define DAY 0xA8705C
#define DAYCUR 0xE8553A     /* STOP, current tab */
#define DAYBOX 0x2A1A16
#define HEART 0xFF4D7A      /* fault */
#define KCAL 0xF7B731       /* VOLT */
#define KCALT 0x3A2A1C
#define STEPS 0xF26B3A      /* AMP */
#define STEPST 0x4A2420
#define POW 0x1FA3F0        /* WATT, online, START */
#define POWT 0x1A2A48
#define COPPER 0xB87A5E
#define HERO 0xECECF1
#define FAULTBG 0x3A1422
#define WARNBG 0x2E2608
#define OFFBG 0x202026
#define CHIPBG 0x26262C

#define C(x) lv_color_hex(x)
#define COUNT(a) ((int)(sizeof(a) / sizeof((a)[0])))

typedef enum { OVERVIEW, MEASURE, FAULTS, SESSION, DEVICE } page_t;
typedef enum { K_NORMAL, K_WARN, K_FAULT, K_OFF } kind_t;
typedef enum { FS_OK, FS_FAULT, FS_INHIBIT, FS_UNKNOWN } fault_state_t;
typedef enum { B_NONE, B_STOP, B_START, B_STOPPING } bar_button_t;

static page_t page;
static bool link_started, link_tab;
static bool dirty = true;
static lv_obj_t *screen, *content, *bar_box, *modal;
static lv_obj_t *rail_tab[5], *rail_mark[5], *rail_badge[5], *rail_icon[5], *rail_label[5];
static lv_obj_t *hdr_dev, *hdr_pill, *hdr_dot, *hdr_text, *hdr_fault_box, *hdr_fault_icon;
static lv_obj_t *banner_box, *banner_icon, *banner_text;
static char bar_signature[200];

static const lv_image_dsc_t *const nav_icon[5] = {&hmi_icon_home_22, &hmi_icon_gauge_22, &hmi_icon_warn_22,
                                                  &hmi_icon_list_22, &hmi_icon_chip_22};
static const char *const nav_name[5] = {"Overview", "Measure", "Faults", "Session", "Device"};

/* ------------------------------------------------------------------ names (ICD section 8) */
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
/* "none", a single name, or NAME+NAME for a mask. */
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
    return m->online && m->status_valid && m->status_age_ms <= 1500;
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
    return fresh(m) && m->command_state != RCC_COMMAND_SENDING &&
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
static void fmt_milli(char *b, size_t n, int32_t v, bool below)   /* mV/mA -> "48.2" */
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
    case 2: snprintf(s->sub, sizeof(s->sub), "CONFIGURATION OR CALIBRATION NEEDED"); s->kind = K_WARN; break;
    case 3: snprintf(s->sub, sizeof(s->sub), "BEING CONFIGURED (SERVICE)"); break;
    case 5:
        mask_text(t, sizeof(t), m->inhibit_mask, inhibit_names, 3);
        for (char *c = t; *c; ++c) if (*c >= 'a' && *c <= 'z') *c -= 32;
        if (m->primary_fault) snprintf(s->sub, sizeof(s->sub), "%s \xC2\xB7 PRIMARY FAULT %s", t, fault_name(m->primary_fault));
        else snprintf(s->sub, sizeof(s->sub), "%s", m->inhibit_mask ? t : "INHIBITED");
        s->kind = K_WARN; break;
    case 6:
        snprintf(s->sub, sizeof(s->sub), "PRIMARY FAULT 0x%04X", (unsigned)m->primary_fault);
        s->kind = K_FAULT; break;
    case 4: {
        static const char *const subs[9] = {"OPERATIONAL", "OPERATIONAL \xC2\xB7 IDLE \xC2\xB7 NO SESSION",
            "OPERATIONAL \xC2\xB7 VALIDATING REQUEST", "OPERATIONAL \xC2\xB7 PRE-CHECKING",
            "OPERATIONAL \xC2\xB7 CLOSING RELAY", "OPERATIONAL \xC2\xB7 VERIFYING CHARGE CURRENT",
            "OPERATIONAL \xC2\xB7 CHARGING", "OPERATIONAL \xC2\xB7 CHARGE COMPLETE",
            "WAITING FOR CHARGER REMOVAL"};
        const uint8_t op = m->operational_state < 9 ? m->operational_state : 0;
        snprintf(s->sub, sizeof(s->sub), "%s", subs[op]);
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
static uint32_t kind_color(kind_t k) { return k == K_WARN ? SUN : k == K_FAULT ? HEART : k == K_OFF ? LABEL : WHITE; }

/* ------------------------------------------------------------------ drawing primitives */
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
static lv_obj_t *text(lv_obj_t *p, const char *s, int x, int y, const lv_font_t *f, uint32_t c)
{
    lv_obj_t *o = lv_label_create(p);
    lv_label_set_text(o, s);
    lv_obj_set_pos(o, x, y);
    lv_obj_set_style_text_font(o, f, 0);
    lv_obj_set_style_text_color(o, C(c), 0);
    return o;
}
/* Text in a w x h box, vertically centred; align is LV_TEXT_ALIGN_*. */
static lv_obj_t *text_in(lv_obj_t *p, const char *s, int x, int y, int w, int h,
                         const lv_font_t *f, uint32_t c, lv_text_align_t align)
{
    lv_obj_t *o = lv_label_create(p);
    lv_label_set_long_mode(o, LV_LABEL_LONG_MODE_CLIP);
    lv_label_set_text(o, s);
    lv_obj_set_width(o, w);
    lv_obj_set_style_text_align(o, align, 0);
    lv_obj_set_style_text_font(o, f, 0);
    lv_obj_set_style_text_color(o, C(c), 0);
    lv_obj_set_pos(o, x, y + (h - (int)lv_font_get_line_height(f)) / 2);
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
/* centred on (cx, cy) */
static lv_obj_t *icon_c(lv_obj_t *p, const lv_image_dsc_t *d, int cx, int cy, uint32_t color)
{
    return icon(p, d, cx - (int)d->header.w / 2, cy - (int)d->header.h / 2, color);
}
static void spin_anim(void *o, int32_t v) { lv_image_set_rotation(o, v); }
static void spacing(lv_obj_t *o, int px) { lv_obj_set_style_text_letter_space(o, px, 0); }
static void hline(lv_obj_t *p, int x, int y, int w, uint32_t c, lv_opa_t opa)
{
    lv_obj_t *l = box(p, x, y, w, 1, c, 0);
    lv_obj_set_style_bg_opa(l, opa, 0);
}
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
static void chip(lv_obj_t *p, int right_x, int y, const char *s, uint32_t fg, uint32_t bg)
{
    lv_obj_t *l = text(p, s, 0, 0, F12B, fg);
    const int w = width_of(l) + 12;
    box(p, right_x - w, y, w, 18, bg, 9);
    lv_obj_move_foreground(l);
    lv_obj_set_pos(l, right_x - w + 6, y + (18 - (int)lv_font_get_line_height(F12B)) / 2);
}
/* label/value row, 20 px high */
static void kv(lv_obj_t *p, int x, int y, int w, const char *k, const char *v, uint32_t vc)
{
    text_in(p, k, x, y, w, 20, F12, LABEL, LV_TEXT_ALIGN_LEFT);
    text_in(p, v, x, y, w, 20, F12B, vc, LV_TEXT_ALIGN_RIGHT);
}
static void section(lv_obj_t *p, int x, int y, int w, const char *left, const char *right, uint32_t rc)
{
    lv_obj_t *l = text_in(p, left, x, y, w, 18, F12B, LABEL, LV_TEXT_ALIGN_LEFT);
    spacing(l, 1);
    if (right) text_in(p, right, x, y, w, 18, F12, rc, LV_TEXT_ALIGN_RIGHT);
}

/* ------------------------------------------------------------------ Overview */
static void ring(lv_obj_t *p, int x, int y, int permille, uint32_t hue, uint32_t track,
                 int icon, bool dim)
{
    lv_obj_t *a = lv_arc_create(p);
    lv_obj_remove_style_all(a);
    lv_obj_set_pos(a, x, y);
    lv_obj_set_size(a, 58, 58);
    lv_obj_set_clickable(a, false);
    lv_arc_set_rotation(a, 135);
    lv_arc_set_bg_angles(a, 0, 270);
    lv_arc_set_range(a, 0, 1000);
    lv_arc_set_value(a, dim ? 0 : (permille < 0 ? 0 : permille > 1000 ? 1000 : permille));
    lv_obj_set_style_arc_width(a, 7, LV_PART_MAIN);
    lv_obj_set_style_arc_width(a, 7, LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(a, true, LV_PART_MAIN);
    lv_obj_set_style_arc_rounded(a, true, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(a, C(dim ? 0x1C1C22 : track), LV_PART_MAIN);
    lv_obj_set_style_arc_color(a, C(dim ? DIM : hue), LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(a, LV_OPA_TRANSP, LV_PART_KNOB);
    const uint32_t col = dim ? DIM : hue;
    icon_c(p, icon == 0 ? &hmi_icon_bolt_20 : icon == 1 ? &hmi_icon_wave_20 : &hmi_icon_power_20, x + 29, y + 29, col);
}

static void page_overview(lv_obj_t *p, const rcc_snapshot_t *m, bool banner, int reason_kind)
{
    state_info_t s; state_info(m, &s);
    const int hero_top = (banner ? 52 : 38) + 2;
    int len = (int)strlen(s.name);
    const lv_font_t *hf = banner ? &hmi_hero40 : &hmi_hero48;
    int lh = banner ? 44 : 54;
    if (len > 9) hf = banner ? &hmi_hero28 : &hmi_hero40;
    lv_obj_t *h = text_in(p, s.name, 60, hero_top, 412, lh, hf, s.kind == K_NORMAL ? HERO : kind_color(s.kind),
                          LV_TEXT_ALIGN_CENTER);
    spacing(h, 1);
    lv_obj_t *sub = text_in(p, s.sub, 60, hero_top + lh, 412, 14, F12B, s.kind == K_NORMAL ? LABEL : kind_color(s.kind),
                            LV_TEXT_ALIGN_CENTER);
    spacing(sub, 1);

    hline(p, 72, 112, 388, COPPER, LV_OPA_60);
    hline(p, 72, 144, 388, COPPER, LV_OPA_60);

    /* stage strip: space-around over x 64..468 */
    static const char *const stages[5] = {"IDLE", "PRE", "RELAY", "VERIFY", "CHG"};
    lv_obj_t *lab[5]; int wd[5], sum = 0;
    for (int i = 0; i < 5; ++i) {
        lab[i] = text(p, stages[i], 0, 0, F14B, DIM);
        spacing(lab[i], 1);
        wd[i] = width_of(lab[i]) + 16;
        sum += wd[i];
    }
    const int slot = (404 - sum) / 5;
    int x = 64 + slot / 2;
    for (int i = 0; i < 5; ++i) {
        const bool cur = s.stage >= 0 && i == s.stage, done = s.stage >= 0 && i < s.stage;
        if (cur) { box(p, x, 116, wd[i], 24, DAYBOX, 6); lv_obj_move_foreground(lab[i]); }
        lv_obj_set_style_text_color(lab[i], C(cur ? DAYCUR : done ? DAY : DIM), 0);
        lv_obj_set_pos(lab[i], x + 8, 116 + (24 - (int)lv_font_get_line_height(F14B)) / 2);
        x += wd[i] + slot;
    }

    /* left column */
    char b[48];
    const bool online = m->online;
    const uint32_t cc = online ? POW : LABEL;
    box(p, 60, 152, 196, 40, CARD, 12);
    icon(p, &hmi_icon_link_22, 71, 152 + 9, cc);
    fmt_age(b, sizeof(b), m->reply_age_ms);
    text(p, b, 106, 152 + 3, F20B, online ? WHITE : LABEL);
    text(p, "UART \xC2\xB7 last reply", 106, 152 + 3 + 21, F12, LABEL);

    const fault_state_t fs = fault_state(m);
    const bool bad = fs == FS_FAULT || fs == FS_INHIBIT || fs == FS_UNKNOWN;
    const uint32_t fc = fs == FS_FAULT ? HEART : fs == FS_INHIBIT ? SUN : LABEL;
    icon(p, bad ? &hmi_icon_warn_15 : &hmi_icon_check_15, 62, 197 + 3, fc);
    char fl[56];
    if (fs == FS_FAULT) snprintf(fl, sizeof(fl), "%s", fault_name(m->primary_fault));
    else if (fs == FS_INHIBIT) {
        mask_text(fl, sizeof(fl), m->inhibit_mask, inhibit_names, 3);
        char t[56];
        snprintf(t, sizeof(t), "Inhibited: %.44s", fl);
        snprintf(fl, sizeof(fl), "%s", t);
    }
    else if (fs == FS_UNKNOWN) snprintf(fl, sizeof(fl), "Faults unreadable");
    else snprintf(fl, sizeof(fl), "No active faults");
    text_in(p, fl, 84, 196, 172, 22, F12B, fs == FS_OK ? WHITE : fc, LV_TEXT_ALIGN_LEFT);
    hline(p, 60, 217, 196, 0x2E2420, LV_OPA_COVER);
    icon(p, &hmi_icon_relay_15, 62, 218 + 3, LABEL);
    text_in(p, "Relay command", 84, 218, 120, 22, F12, TXT2, LV_TEXT_ALIGN_LEFT);
    text_in(p, fresh(m) ? (m->relay_command ? "CLOSED" : "OPEN") : "\xE2\x80\x94", 160, 218, 96, 22, F12B, WHITE, LV_TEXT_ALIGN_RIGHT);
    hline(p, 60, 239, 196, 0x2E2420, LV_OPA_COVER);
    icon(p, &hmi_icon_sensor_15, 62, 240 + 3, LABEL);
    text_in(p, "Contact feedback", 84, 240, 120, 22, F12, TXT2, LV_TEXT_ALIGN_LEFT);
    text_in(p, "None", 160, 240, 96, 22, F12B, LABEL, LV_TEXT_ALIGN_RIGHT);

    /* metrics */
    const bool stale = !(m->online && m->measurement_valid && m->measurement_age_ms <= 1500);
    char v[16] = "\xE2\x80\x94", a[16] = "\xE2\x80\x94", w[16] = "\xE2\x80\x94";
    int pv = 0, pa = 0, pw = 0;
    if (v_valid(m)) { fmt_milli(v, sizeof(v), (int32_t)m->vout_mv, v_floor(m)); pv = (int)(m->vout_mv / 60u); }
    if (i_valid(m)) { fmt_milli(a, sizeof(a), m->iout_ma, false); pa = m->iout_ma / 10; }
    if (v_valid(m) && !v_floor(m) && i_valid(m)) {
        const int64_t mw = (int64_t)m->vout_mv * m->iout_ma / 1000;
        snprintf(w, sizeof(w), "%d", (int)((mw + 500) / 1000));
        pw = (int)(mw * 10 / 6000);
    }
    const char *const caption[3] = {"VOLT", "AMP", "WATT"};
    const char *const val[3] = {v, a, w};
    const uint32_t hue[3] = {KCAL, STEPS, POW}, trk[3] = {KCALT, STEPST, POWT};
    const int per[3] = {pv, pa, pw};
    for (int i = 0; i < 3; ++i) {
        const int x0 = 262 + i * 71;
        text_in(p, val[i], x0, 150, 70, 24, F22B, stale ? DIM : WHITE, LV_TEXT_ALIGN_CENTER);
        lv_obj_t *u = text_in(p, stale ? "STALE" : caption[i], x0, 175, 70, 14, F12B, LABEL, LV_TEXT_ALIGN_CENTER);
        spacing(u, 1);
        ring(p, x0 + 6, 192, per[i], hue[i], trk[i], i, stale);
    }
    if (reason_kind == 1) text_in(p, "Cannot start: link stale", 262, 250, 212, 14, F12B, SUN, LV_TEXT_ALIGN_CENTER);
    else if (reason_kind == 2) text_in(p, "Cannot start: fault active", 262, 250, 212, 14, F12B, SUN, LV_TEXT_ALIGN_CENTER);
}

/* ------------------------------------------------------------------ Measure */
static void channel_card(lv_obj_t *p, int x, const char *title, uint32_t hue, bool valid,
                         bool floor_bit, bool stale, bool present, int32_t value, int32_t filtered,
                         const char *unit, const char *fname)
{
    box(p, x, 0, 202, 84, CARD, 12);
    section(p, x + 12, 6, 178, title, NULL, 0);
    const char *st = stale ? "STALE" : valid ? (floor_bit ? "BELOW FLOOR" : "VALID") : "INVALID";
    const uint32_t sc = stale ? LABEL : valid ? (floor_bit ? SUN : POW) : HEART;
    chip(p, x + 190, 6, st, sc, CHIPBG);
    char b[40], f[56];
    if (valid) fmt_milli(b, sizeof(b), value, floor_bit); else snprintf(b, sizeof(b), "\xE2\x80\x94");
    lv_obj_t *vl = text(p, b, x + 12, 24 + (34 - (int)lv_font_get_line_height(F28B)) / 2, F28B, valid ? hue : DIM);
    if (valid) {
        const int vw = width_of(vl);
        text_in(p, unit, x + 12 + vw + 4, 24, 40, 34, F14B, LABEL, LV_TEXT_ALIGN_LEFT);
        char t[24]; fmt_milli(t, sizeof(t), filtered, floor_bit);
        snprintf(f, sizeof(f), "%s %s %s", fname, t, unit);
    } else snprintf(f, sizeof(f), "%s", stale ? "Last value is stale" : present ? "Not valid" : "Not present");
    text_in(p, f, x + 12, 58, 180, 16, F12, TXT2, LV_TEXT_ALIGN_LEFT);
}
static void page_measure(lv_obj_t *p, const rcc_snapshot_t *m)
{
    const bool stale = !(m->online && m->measurement_valid && m->measurement_age_ms <= 1500);
    const bool vv = v_valid(m), iv = i_valid(m);
    channel_card(p, 0, "VOUT", KCAL, vv, v_floor(m), stale, m->measurement_valid, (int32_t)m->vout_mv,
                 (int32_t)m->vout_filtered_mv, "V", "Filtered");
    channel_card(p, 210, "IOUT \xC2\xB7 signed", STEPS, iv, false, stale, m->measurement_valid, m->iout_ma,
                 m->iout_filtered_ma, "A", "Filtered");
    /* validity matrix */
    box(p, 0, 92, 412, 70, CARD, 12);
    static const char *const hdr[7] = {"PRES", "FRESH", "CAL", "RANGE", "SAT", "PLAUS", "FLOOR"};
    const int col0 = 12 + 44, colw = (388 - 44) / 7;
    for (int i = 0; i < 7; ++i)
        text_in(p, hdr[i], col0 + i * colw, 98, colw, 18, F12, LABEL, LV_TEXT_ALIGN_CENTER);
    for (int r = 0; r < 2; ++r) {
        const uint16_t bits = r ? m->iout_status : m->vout_status;
        const int y = 116 + r * 20;
        text_in(p, r ? "IOUT" : "VOUT", 12, y, 44, 20, F12B, r ? STEPS : KCAL, LV_TEXT_ALIGN_LEFT);
        for (int i = 0; i < 7; ++i) {
            const int cx = col0 + i * colw;
            if (!m->online || !m->measurement_valid) { text_in(p, "\xE2\x80\x94", cx, y, colw, 20, F12, DIM, LV_TEXT_ALIGN_CENTER); continue; }
            const bool set = (bits >> i) & 1u;
            if (i == 6) {
                if (r == 0 && set) icon_c(p, &hmi_icon_warn_14, cx + colw / 2, y + 10, SUN);
                else text_in(p, "-", cx, y, colw, 20, F12, DIM, LV_TEXT_ALIGN_CENTER);
            } else icon_c(p, set ? &hmi_icon_check_14 : &hmi_icon_cross_13, cx + colw / 2, y + 10, set ? POW : HEART);
        }
    }
    /* sequence / age */
    box(p, 0, 170, 412, 28, CARD, 12);
    char b[40], c[24];
    if (m->online && m->measurement_valid) snprintf(b, sizeof(b), "%" PRIu32, m->measurement_sequence);
    else snprintf(b, sizeof(b), "\xE2\x80\x94");
    kv(p, 12, 174, 176, "Sequence", b, WHITE);
    if (m->online && m->measurement_valid) {
        fmt_age(c, sizeof(c), m->measurement_age_ms);
        snprintf(b, sizeof(b), "%" PRIu32 " ms \xC2\xB7 held %s", m->measurement_sample_age_ms, c);
    } else snprintf(b, sizeof(b), "\xE2\x80\x94");
    kv(p, 212, 174, 188, "Age", b, WHITE);
}

/* ------------------------------------------------------------------ Faults */
static const char *fault_hint(uint16_t id)
{
    if (id >= 0x0301 && id <= 0x0306) return "Hint: check the current-sense circuit";
    if (id >= 0x0401 && id <= 0x0403) return "Hint: check the load and the current limit";
    if (id >= 0x0201 && id <= 0x0202) return "Hint: check the relay and its driver";
    if (id >= 0x0601 && id <= 0x0604) return "Hint: check the controller link";
    return NULL;
}
static void mask_card(lv_obj_t *p, int x, int w, const char *title, uint32_t mask,
                      const char *const *names, int count, int max_rows)
{
    char hexs[16]; snprintf(hexs, sizeof(hexs), "0x%08" PRIX32, mask);
    int rows = 0;
    for (int b = 0; b < 32; ++b) if (mask & (1u << b)) ++rows;
    const int shown = rows ? (rows > max_rows ? max_rows : rows) : 1;
    box(p, x, 0, w, 12 + 18 + shown * 20, CARD, 12);
    section(p, x + 12, 6, w - 24, title, hexs, LABEL);
    if (!rows) { text_in(p, "None", x + 12, 24, w - 24, 20, F12, LABEL, LV_TEXT_ALIGN_LEFT); return; }
    int y = 24, n = 0;
    for (int b = 0; b < 32; ++b) {
        if (!(mask & (1u << b))) continue;
        char t[40], bit[12];
        if (n == max_rows - 1 && rows > max_rows) {
            snprintf(t, sizeof(t), "+ %d more", rows - n);
            text_in(p, t, x + 12, y, w - 24, 20, F12, LABEL, LV_TEXT_ALIGN_LEFT);
            return;
        }
        if (b < count) snprintf(t, sizeof(t), "%s", names[b]); else snprintf(t, sizeof(t), "Unrecognized");
        snprintf(bit, sizeof(bit), "bit %d", b);
        text_in(p, t, x + 12, y, w - 24, 20, F12B, WHITE, LV_TEXT_ALIGN_LEFT);
        text_in(p, bit, x + 12, y, w - 24, 20, F12, LABEL, LV_TEXT_ALIGN_RIGHT);
        y += 20; ++n;
    }
}
static void page_faults(lv_obj_t *p, const rcc_snapshot_t *m, int height)
{
    const fault_state_t fs = fault_state(m);
    static const char *const title[4] = {"No fault", "Fault active", "Inhibited", "Fault state unreadable"};
    const uint32_t tc = fs == FS_OK ? POW : fs == FS_FAULT ? HEART : fs == FS_INHIBIT ? SUN : LABEL;
    box(p, 0, 0, 412, 44, CARD, 12);
    icon(p, fs == FS_OK ? &hmi_icon_checkc_22 : &hmi_icon_warn_22, 12, 11, tc);
    text_in(p, title[fs], 44, 0, 250, 44, F16B, fs == FS_OK ? WHITE : tc, LV_TEXT_ALIGN_LEFT);
    char b[48], t[24];
    if (m->online && m->faults_valid) { fmt_age(t, sizeof(t), m->faults_age_ms); snprintf(b, sizeof(b), "Read %s ago", t); }
    else snprintf(b, sizeof(b), "Not read");
    text_in(p, b, 200, 0, 200, 44, F12, LABEL, LV_TEXT_ALIGN_RIGHT);
    if (fs == FS_OK || fs == FS_UNKNOWN) {
        box(p, 0, 52, 412, 52, CARD, 12);
        const char *none = fs == FS_UNKNOWN ? "\xE2\x80\x94" : "None";
        snprintf(b, sizeof(b), "%s  0x%08" PRIX32, none, (uint32_t)0);
        if (fs == FS_UNKNOWN) snprintf(b, sizeof(b), "\xE2\x80\x94");
        kv(p, 12, 58, 388, "Active faults", b, WHITE);
        kv(p, 12, 78, 388, "Inhibit", b, WHITE);
        return;
    }
    int y = 52;
    const bool fault_primary = m->primary_fault != 0;
    if (fault_primary || m->inhibit_mask) {
        const char *hint = fault_primary ? fault_hint(m->primary_fault) :
                           "Set by STOP; START may clear it, the controller decides";
        const int ph = hint ? 46 : 28;
        box(p, 0, y, 412, ph, CARD, 12);
        char name[40], id[12];
        if (fault_primary) { snprintf(name, sizeof(name), "%s", fault_name(m->primary_fault)); snprintf(id, sizeof(id), "0x%04X", (unsigned)m->primary_fault); }
        else { char t2[24]; mask_text(t2, sizeof(t2), m->inhibit_mask, inhibit_names, 3); snprintf(name, sizeof(name), "INHIBIT \xC2\xB7 %s", t2); id[0] = 0; }
        text_in(p, name, 12, y + 6, 300, 20, F14B, fault_primary ? HEART : SUN, LV_TEXT_ALIGN_LEFT);
        text_in(p, id, 12, y + 6, 388, 20, F12, LABEL, LV_TEXT_ALIGN_RIGHT);
        if (hint) text_in(p, hint, 12, y + 26, 388, 16, F12, TXT2, LV_TEXT_ALIGN_LEFT);
        y += ph + 8;
    }
    const int avail = height - y - 30;
    int max_rows = avail / 20; if (max_rows < 1) max_rows = 1;
    lv_obj_t *left = obj(p, 0, y, 202, height - y);
    lv_obj_t *right = obj(p, 210, y, 202, height - y);
    mask_card(left, 0, 202, "FAULTS", m->active_fault_mask, fault_names, 19, max_rows);
    mask_card(right, 0, 202, "INHIBIT", m->inhibit_mask, inhibit_names, 3, max_rows);
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
    if (code == 0x0012) return HEART;
    if (code == 0x0011 || (code >= 0x0030 && code <= 0x0033)) return SUN;
    if (code >= 0x0013 && code <= 0x0016) return POW;
    return LABEL;
}
static void fmt_duration(char *b, size_t n, uint32_t ms)
{
    const uint32_t s = ms / 1000u;
    if (s >= 3600u) snprintf(b, n, "%u h %02u min", (unsigned)(s / 3600u), (unsigned)(s % 3600u / 60u));
    else if (s >= 60u) snprintf(b, n, "%u min %02u s", (unsigned)(s / 60u), (unsigned)(s % 60u));
    else snprintf(b, n, "%u s", (unsigned)s);
}
static void event_detail(char *b, size_t n, uint16_t code, const uint32_t a[4])
{
    char x[40], y[40], z[20];
    b[0] = '\0';
    switch (code) {
    case 0x0001: snprintf(b, n, "reset: %s", (a[0] & 0xffu) < 7u ? reset_names[a[0] & 0xffu] : "?"); break;
    case 0x0010:
        snprintf(x, sizeof(x), "%s", state_name(a[0] & 0xffu, (a[0] >> 8) & 0xffu));
        snprintf(y, sizeof(y), "%s", state_name((a[0] >> 16) & 0xffu, (a[0] >> 24) & 0xffu));
        snprintf(b, n, "%s \xE2\x86\x92 %s", x, y); break;
    case 0x0011:
        mask_text(x, sizeof(x), a[0], inhibit_names, 3); mask_text(y, sizeof(y), a[1], inhibit_names, 3);
        snprintf(b, n, "%s \xE2\x86\x92 %s", x, y); break;
    case 0x0012:
        snprintf(b, n, "%s \xE2\x86\x92 %s", a[0] ? "active" : "none",
                 (a[2] & 0xffffu) ? fault_name((uint16_t)a[2]) : "none"); break;
    case 0x0013: snprintf(b, n, "session #%" PRIu32, a[0]); break;
    case 0x0014:
        fmt_milli(x, sizeof(x), (int32_t)a[1], false); fmt_milli(y, sizeof(y), (int32_t)a[2], false);
        snprintf(b, n, "%s V \xC2\xB7 %s A", x, y); break;
    case 0x0015: case 0x0016:
        fmt_duration(z, sizeof(z), a[2]);
        snprintf(b, n, "%s \xC2\xB7 %s", (a[1] & 0xffu) < 10u ? end_names[a[1] & 0xffu] : "?", z); break;
    default: break;
    }
}
static void fmt_since_boot(char *b, size_t n, uint64_t us)
{
    const uint32_t s = (uint32_t)(us / 1000000u);
    if (s < 120u) snprintf(b, n, "+%us", (unsigned)s);
    else if (s < 7200u) snprintf(b, n, "+%um", (unsigned)(s / 60u));
    else snprintf(b, n, "+%uh", (unsigned)(s / 3600u));
}
static const char *command_text(const rcc_snapshot_t *m)
{
    static const char *const st[7] = {"", "SENDING", "ACCEPTED", "COMPLETED", "REJECTED", "FAILED", "NOT CONFIRMED"};
    return st[m->command_state < 7 ? m->command_state : 0];
}
static void page_session(lv_obj_t *p, const rcc_snapshot_t *m, int height)
{
    state_info_t s; state_info(m, &s);
    char b[64], t[24];
    box(p, 0, 0, 412, 68, CARD, 12);
    if (fresh(m) && m->session_id) snprintf(b, sizeof(b), "#%" PRIu32, m->session_id);
    else snprintf(b, sizeof(b), "\xE2\x80\x94");
    kv(p, 12, 4, 166, "Session", b, WHITE);
    kv(p, 12, 24, 166, "State", s.name, WHITE);
    if (m->end_valid && (!m->session_id || m->end_session_id == m->session_id)) fmt_duration(b, sizeof(b), m->end_duration_ms);
    else if (fresh(m) && m->top_state == 4 && m->operational_state >= 2 && m->operational_state <= 6) snprintf(b, sizeof(b), "In progress");
    else snprintf(b, sizeof(b), "\xE2\x80\x94");
    kv(p, 12, 44, 166, "Duration", b, WHITE);
    if (m->command_state == RCC_COMMAND_NONE) snprintf(b, sizeof(b), "\xE2\x80\x94");
    else snprintf(b, sizeof(b), "%s \xC2\xB7 %s", m->last_command == 0x21 ? "STOP" : "START", command_text(m));
    kv(p, 192, 4, 208, "Last cmd", b, WHITE);
    if (m->command_state == RCC_COMMAND_NONE || m->command_age_ms == UINT32_MAX) snprintf(b, sizeof(b), "\xE2\x80\x94");
    else { fmt_age(t, sizeof(t), m->command_age_ms); snprintf(b, sizeof(b), "%s ago", t); }
    kv(p, 192, 24, 208, "Received", b, WHITE);
    snprintf(b, sizeof(b), "%s", m->end_valid ? (m->end_reason < 10 ? end_names[m->end_reason] : "?") : "\xE2\x80\x94");
    kv(p, 192, 44, 208, "End reason", b, WHITE);

    const bool log_ok = m->online && m->device_valid;
    char right[48] = "\xE2\x80\x94";
    if (log_ok) {
        if (m->event_missed) snprintf(right, sizeof(right), "%u shown \xC2\xB7 %" PRIu32 " overwritten", (unsigned)m->event_count, m->event_missed);
        else snprintf(right, sizeof(right), "%u shown", (unsigned)m->event_count);
    }
    section(p, 12, 74, 388, "EVENT LOG", right, m->event_missed ? SUN : LABEL);
    const int rows = (height - 96) / 22;
    if (!log_ok) {
        icon_c(p, &hmi_icon_warn_18, 206, 112, SUN);
        text_in(p, "Log unavailable \xE2\x80\x94 link lost", 0, 124, 412, 22, F14B, SUN, LV_TEXT_ALIGN_CENTER);
        return;
    }
    if (!m->event_count) { text_in(p, "No events yet", 12, 96, 388, 22, F12, LABEL, LV_TEXT_ALIGN_LEFT); return; }
    for (int i = 0; i < rows && i < m->event_count; ++i) {
        const int n = (int)m->event_count - 1 - i, y = 96 + i * 22;
        char d[96];
        fmt_since_boot(t, sizeof(t), m->event_us[n]);
        text_in(p, t, 12, y, 52, 22, F12, LABEL, LV_TEXT_ALIGN_LEFT);
        box(p, 12 + 52, y + 7, 8, 8, event_color(m->event_codes[n]), 4);
        text_in(p, event_name(m->event_codes[n]), 12 + 52 + 16, y, 200, 22, F12B, WHITE, LV_TEXT_ALIGN_LEFT);
        event_detail(d, sizeof(d), m->event_codes[n], m->event_args[n]);
        text_in(p, d, 160, y, 240, 22, F12, TXT2, LV_TEXT_ALIGN_RIGHT);
        if (i + 1 < rows) hline(p, 12, y + 21, 388, 0x2E2420, LV_OPA_COVER);
    }
}

/* ------------------------------------------------------------------ Device */
static void redraw_cb(void *unused);
static void tab_click(lv_event_t *e)
{
    link_tab = (uintptr_t)lv_event_get_user_data(e) != 0;
    (void)lv_async_call(redraw_cb, NULL);         /* never delete the active event target */
}
static void page_device(lv_obj_t *p, const rcc_snapshot_t *m)
{
    const bool rd = m->online && m->device_valid;
    const bool provisional = rd && (m->build_flags & 1u);
    const uint32_t nc = rd ? SUN : LABEL;
    box(p, 0, 0, 412, 28, rd ? WARNBG : OFFBG, 10);
    box(p, 0, 4, 3, 20, nc, 1);
    if (rd) icon(p, &hmi_icon_warn_15, 14, 6, SUN);
    lv_obj_t *nt = text_in(p, rd ? (provisional ? "BENCH IMAGE \xE2\x80\x94 NOT A PRODUCT BUILD" : "PRODUCT BUILD")
                                 : "CONTROLLER INFO NOT READ", rd ? 36 : 14, 0, 300, 28, F12B, nc, LV_TEXT_ALIGN_LEFT);
    spacing(nt, 0);
    text_in(p, rd ? ((m->build_flags & 2u) ? "" : "Relay locked") : "Link lost", 200, 0, 198, 28, F12, TXT2, LV_TEXT_ALIGN_RIGHT);
    /* segmented control */
    box(p, 0, 36, 412, 40, CARD, 12);
    for (int i = 0; i < 2; ++i) {
        const bool on = (i == 1) == link_tab;
        lv_obj_t *s = button(p, 4 + i * 202, 40, 202, 32, on ? DAYBOX : CARD, 9, tab_click, (void *)(uintptr_t)i);
        lv_obj_set_style_bg_opa(s, on ? LV_OPA_COVER : LV_OPA_TRANSP, 0);
        lv_obj_t *l = text_in(s, i ? "LINK" : "CONTROLLER", 0, 0, 202, 32, F12B, on ? DAYCUR : DIM, LV_TEXT_ALIGN_CENTER);
        spacing(l, 1);
        lv_obj_set_clickable(l, false);
    }
    char b[64], t[24];
    if (!link_tab) {
        const int cw = 202;
        box(p, 0, 84, cw, 112, CARD, 12);
        section(p, 12, 90, cw - 24, "IDENTITY", NULL, 0);
        const bool compat = rd && m->product_id == 1u && m->protocol_version == 1u;
        chip(p, cw - 12, 90, rd ? (compat ? "COMPATIBLE" : "INCOMPATIBLE") : "NOT READ", rd ? (compat ? POW : HEART) : LABEL, CHIPBG);
        if (rd) snprintf(b, sizeof(b), "0x%04X \xC2\xB7 v%u", (unsigned)m->product_id, (unsigned)m->protocol_version); else snprintf(b, sizeof(b), "\xE2\x80\x94");
        kv(p, 12, 108, cw - 24, "Product", b, WHITE);
        if (rd) snprintf(b, sizeof(b), "%" PRIu32 " / %" PRIu32, m->hardware_revision, m->firmware_revision); else snprintf(b, sizeof(b), "\xE2\x80\x94");
        kv(p, 12, 128, cw - 24, "HW / FW", b, WHITE);
        if (rd) snprintf(b, sizeof(b), "%02X%02X%02X%02X\xE2\x80\xA6", m->image_id[0], m->image_id[1], m->image_id[2], m->image_id[3]); else snprintf(b, sizeof(b), "\xE2\x80\x94");
        kv(p, 12, 148, cw - 24, "Image", b, WHITE);
        if (rd) snprintf(b, sizeof(b), "\xE2\x80\xA6%04X", (unsigned)(m->boot_id & 0xffffu)); else snprintf(b, sizeof(b), "\xE2\x80\x94");
        kv(p, 12, 168, cw - 24, "Boot ID", b, WHITE);

        box(p, 210, 84, cw, 112, CARD, 12);
        section(p, 222, 90, cw - 24, "BUILD FLAGS \xC2\xB7 LINKS", NULL, 0);
        static const char *const fl[3] = {"Provisional", "Relay enable", "Operational node"};
        for (int i = 0; i < 3; ++i) {
            const bool on = rd && ((m->build_flags >> i) & 1u);
            text_in(p, fl[i], 222, 108 + i * 20, cw - 24, 20, F12, LABEL, LV_TEXT_ALIGN_LEFT);
            if (rd) chip(p, 210 + cw - 12, 109 + i * 20, on ? "ON" : "OFF", on ? SUN : LABEL, on ? WARNBG : CHIPBG);
            else text_in(p, "\xE2\x80\x94", 222, 108 + i * 20, cw - 24, 20, F12B, DIM, LV_TEXT_ALIGN_RIGHT);
        }
        static const char *const ln[4] = {"CAN", "RS485", "UART", "SVC"};
        lv_obj_t *ll[4]; int lw[4], tot = 0;
        for (int i = 0; i < 4; ++i) { ll[i] = text(p, ln[i], 0, 0, F12B, DIM); lw[i] = width_of(ll[i]) + 12; tot += lw[i]; }
        const int gap = (cw - 24 - tot) / 3;
        int cx = 222;
        for (int i = 0; i < 4; ++i) {
            const bool on = rd && ((m->running_links >> i) & 1u);
            box(p, cx, 168, lw[i], 18, on ? 0x10202E : 0x1E1E24, 9);
            lv_obj_move_foreground(ll[i]);
            lv_obj_set_style_text_color(ll[i], C(on ? POW : DIM), 0);
            lv_obj_set_pos(ll[i], cx + 6, 168 + (18 - (int)lv_font_get_line_height(F12B)) / 2);
            cx += lw[i] + gap;
        }
    } else {
        const bool on = m->online && link_started;
        const uint32_t c = on ? POW : LABEL;
        box(p, 0, 84, 254, 112, CARD, 12);
        box(p, 12, 94, 8, 8, c, 4);
        text_in(p, on ? "ONLINE" : "OFFLINE", 26, 88, 100, 20, F16B, c, LV_TEXT_ALIGN_LEFT);
        fmt_age(t, sizeof(t), m->reply_age_ms);
        snprintf(b, sizeof(b), "%s ago", t);
        text_in(p, b, 130, 88, 112, 20, F12, LABEL, LV_TEXT_ALIGN_RIGHT);
        snprintf(b, sizeof(b), "%" PRIu32, m->timeouts); kv(p, 12, 114, 112, "Timeouts", b, WHITE);
        snprintf(b, sizeof(b), "%" PRIu32, m->rx_bad_frames); kv(p, 12, 134, 112, "Bad frames", b, WHITE);
        snprintf(b, sizeof(b), "%" PRIu32, m->event_missed); kv(p, 12, 154, 112, "Missed events", b, WHITE);
        fmt_age(t, sizeof(t), m->status_age_ms); kv(p, 136, 114, 106, "Status", t, WHITE);
        fmt_age(t, sizeof(t), m->measurement_age_ms); kv(p, 136, 134, 106, "Measure", t, WHITE);
        fmt_age(t, sizeof(t), m->faults_age_ms); kv(p, 136, 154, 106, "Faults", t, WHITE);
        box(p, 262, 84, 150, 112, CARD, 12);
        section(p, 274, 90, 126, "HMI UART", NULL, 0);
        kv(p, 274, 108, 126, "UART0", "115200 8N1", WHITE);
        kv(p, 274, 128, 126, "Pins", "TX 1 \xC2\xB7 RX 3", WHITE);
        kv(p, 274, 148, 126, "Mode", "Queries", WHITE);
    }
}

/* ------------------------------------------------------------------ action bar */
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
    char rb[16], r2[64];
    const bool pending = m->command_state == RCC_COMMAND_SENDING || m->command_state == RCC_COMMAND_ACCEPTED ||
                         m->command_state == RCC_COMMAND_UNCONFIRMED;
    const bool terminal = m->command_state == RCC_COMMAND_COMPLETED || m->command_state == RCC_COMMAND_REJECTED ||
                          m->command_state == RCC_COMMAND_FAILED;
    const bool recent = pending || (terminal && m->command_age_ms < 15000u);
    const bool is_overview = page == OVERVIEW;
    b->title_color = POW;
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
            b->title_color = SUN;
            if (m->command_reason == 0x0007) {
                snprintf(b->title, sizeof(b->title), "Stop took effect, not tracked");
                snprintf(b->sub, sizeof(b->sub), "Send STOP again");
            } else {
                snprintf(b->title, sizeof(b->title), "STOP rejected");
                snprintf(b->sub, sizeof(b->sub), "%s", reason_text(m->command_reason, rb, sizeof(rb)));
            } break;
        case RCC_COMMAND_FAILED:
            b->title_color = SUN;
            if (m->command_reason == 0x000F) {
                snprintf(b->title, sizeof(b->title), "Relay commanded open");
                snprintf(b->sub, sizeof(b->sub), "Saving the state failed");
            } else {
                snprintf(b->title, sizeof(b->title), "STOP failed");
                snprintf(b->sub, sizeof(b->sub), "%s", reason_text(m->command_reason, rb, sizeof(rb)));
            } break;
        default:
            b->title_color = SUN;
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
            b->title_color = SUN;
            snprintf(b->title, sizeof(b->title), "NOT CONFIRMED");
            snprintf(b->sub, sizeof(b->sub), "No reply to START \xE2\x80\x94 check controller"); break;
        default:
            b->title_color = SUN;
            snprintf(b->title, sizeof(b->title), "START %s", m->command_state == RCC_COMMAND_REJECTED ? "rejected" : "failed");
            snprintf(r2, sizeof(r2), "%s", reason_text(m->command_reason, rb, sizeof(rb)));
            snprintf(b->sub, sizeof(b->sub), "%s", r2);
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
            else if (m->top_state == 4 && m->operational_state == 6) snprintf(b->sub, sizeof(b->sub), "Session #%" PRIu32 " \xC2\xB7 relay commanded closed", m->session_id);
            else snprintf(b->sub, sizeof(b->sub), "Relay %s", m->relay_command ? "commanded closed" : "commanded open");
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
    modal = obj(screen, 52, 34, 428, 230);
    lv_obj_set_clickable(modal, true);
    lv_obj_set_style_bg_color(modal, C(BG), 0);
    lv_obj_set_style_bg_opa(modal, (lv_opa_t)191, 0);
    lv_obj_t *card = box(modal, 34, 10, 360, 212, CARD, 14);
    lv_obj_set_style_border_width(card, 1, 0);
    lv_obj_set_style_border_color(card, C(0x3A2A22), 0);
    text_in(card, "Request charge start?", 16, 7, 328, 26, F16B, WHITE, LV_TEXT_ALIGN_LEFT);
    if (fresh(m) && m->top_state == 4 && m->operational_state == 1) snprintf(b, sizeof(b), "IDLE \xC2\xB7 Ready");
    else snprintf(b, sizeof(b), "%s", s.name);
    kv(card, 16, 37, 328, "State", b, WHITE);
    fmt_age(t, sizeof(t), m->reply_age_ms); snprintf(b, sizeof(b), "Connected \xC2\xB7 %s ago", t);
    kv(card, 16, 59, 328, "Link", b, WHITE);
    if (v_valid(m) && i_valid(m)) snprintf(b, sizeof(b), "Fresh \xC2\xB7 %" PRIu32 " ms", m->measurement_sample_age_ms);
    else snprintf(b, sizeof(b), "Not valid");
    kv(card, 16, 81, 328, "Measurements", b, WHITE);
    const fault_state_t fs = fault_state(m);
    if (fs == FS_FAULT) snprintf(b, sizeof(b), "%s", fault_name(m->primary_fault));
    else if (fs == FS_INHIBIT) mask_text(b, sizeof(b), m->inhibit_mask, inhibit_names, 3);
    else snprintf(b, sizeof(b), "None");
    kv(card, 16, 103, 328, "Fault / inhibit", b, fs == FS_OK ? WHITE : fs == FS_FAULT ? HEART : SUN);
    text_in(card, "The controller decides and may reject it.", 16, 128, 328, 16, F12, LABEL, LV_TEXT_ALIGN_LEFT);
    lv_obj_t *c1 = button(card, 16, 153, 158, 44, 0x2A2A32, 12, cancel_click, NULL);
    lv_obj_t *l1 = text_in(c1, "CANCEL", 0, 0, 158, 44, F14B, WHITE, LV_TEXT_ALIGN_CENTER); spacing(l1, 1);
    lv_obj_t *c2 = button(card, 186, 153, 158, 44, POW, 12, send_start_click, NULL);
    lv_obj_t *l2 = text_in(c2, "SEND START", 0, 0, 158, 44, F14B, BG, LV_TEXT_ALIGN_CENTER); spacing(l2, 1);
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
    const lv_image_dsc_t *glyph = kind == B_START ? &hmi_icon_play_18 : kind == B_STOPPING ? &hmi_icon_spin_18 : &hmi_icon_stop_18;
    const uint32_t bg = kind == B_START ? POW : kind == B_STOPPING ? 0x4A2018 : DAYCUR;
    const uint32_t fg = kind == B_STOPPING ? 0xF3A08A : BG;
    lv_obj_t *b = button(p, x, 0, w, 44, bg, 12, action_click, (void *)(uintptr_t)(kind == B_START ? 1 : 0));
    lv_obj_t *tx = text(b, label, 0, 0, f, fg);
    spacing(tx, 2);
    lv_obj_set_clickable(tx, false);
    const int iw = glyph->header.w, tw = width_of(tx), total = iw + 8 + tw;
    lv_obj_t *ic = icon(b, glyph, (w - total) / 2, (44 - (int)glyph->header.h) / 2, fg);
    lv_obj_set_pos(tx, (w - total) / 2 + iw + 8, (44 - (int)lv_font_get_line_height(f)) / 2);
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
        box(bar_box, 0, 0, 412, 44, CARD, 12);
        box(bar_box, 0, 6, 3, 32, SUN, 1);
        icon(bar_box, &hmi_icon_warn_22, 14, 11, SUN);
        text_in(bar_box, "No control authority \xE2\x80\x94 monitoring only", 46, 4, 360, 18, F12B, WHITE, LV_TEXT_ALIGN_LEFT);
        text_in(bar_box, "Use the E-stop. Grant control on the controller (service).", 46, 22, 360, 16, F12, LABEL, LV_TEXT_ALIGN_LEFT);
        return;
    }
    if (b->kind == BAR_FULL) { bar_button(bar_box, 0, 412, b->button, F20B); return; }
    const int msg_w = b->kind == BAR_SPLIT ? 252 : 412;
    text_in(bar_box, b->title, 0, 2, msg_w, 20, F14B, b->title_color, LV_TEXT_ALIGN_LEFT);
    text_in(bar_box, b->sub, 0, 22, msg_w, 18, F12, LABEL, LV_TEXT_ALIGN_LEFT);
    if (b->kind == BAR_SPLIT) bar_button(bar_box, 262, 150, b->button, F16B);
}

/* ------------------------------------------------------------------ chrome */
static void goto_page(page_t n)
{
    close_modal();
    page = n;
    dirty = true;
    (void)lv_async_call(redraw_cb, NULL);
}
static void nav_click(lv_event_t *e) { goto_page((page_t)(uintptr_t)lv_event_get_user_data(e)); }
static void fault_click(lv_event_t *e) { (void)e; goto_page(FAULTS); }

static void build_chrome(void)
{
    box(screen, 51, 0, 1, 320, 0x1C1C22, 0);
    for (int i = 0; i < 5; ++i) {
        rail_tab[i] = button(screen, 0, i * 64, 51, 64, BG, 0, nav_click, (void *)(uintptr_t)i);
        lv_obj_set_style_bg_opa(rail_tab[i], LV_OPA_TRANSP, 0);
        rail_mark[i] = box(rail_tab[i], 0, 14, 3, 36, DAYCUR, 2);
        rail_icon[i] = icon(rail_tab[i], nav_icon[i], 0, 0, DIM);
        rail_label[i] = text(rail_tab[i], nav_name[i], 0, 0, F10, DIM);
        spacing(rail_label[i], -1);
        lv_obj_set_clickable(rail_label[i], false);
        rail_badge[i] = NULL;
    }
    rail_badge[FAULTS] = box(rail_tab[FAULTS], 29, 9, 8, 8, SUN, 4);
    lv_obj_set_style_outline_width(rail_badge[FAULTS], 2, 0);
    lv_obj_set_style_outline_color(rail_badge[FAULTS], C(BG), 0);

    hdr_dev = text(screen, "RCC-01", 60, 0, F14B, LABEL);
    lv_obj_set_y(hdr_dev, (34 - (int)lv_font_get_line_height(F14B)) / 2);
    spacing(hdr_dev, 1);
    hdr_pill = box(screen, 0, 6, 100, 22, CARD, 11);
    hdr_dot = box(hdr_pill, 10, 7, 8, 8, POW, 4);
    hdr_text = text(hdr_pill, "", 24, 0, F12B, POW);
    hdr_fault_box = button(screen, 440, 3, 32, 28, CARD, 8, fault_click, NULL);
    hdr_fault_icon = icon(hdr_fault_box, &hmi_icon_checkc_17, 7, 5, DIM);
    lv_obj_set_clickable(hdr_fault_icon, false);

    banner_box = button(screen, 52, 34, 428, 18, OFFBG, 0, fault_click, NULL);
    banner_icon = icon(banner_box, &hmi_icon_warn_14, 0, 0, SUN);
    banner_text = text(banner_box, "", 0, 0, F12B, SUN);
    lv_obj_set_clickable(banner_icon, false);
    lv_obj_set_clickable(banner_text, false);
    lv_obj_set_hidden(banner_box, true);
}

static void update_chrome(const rcc_snapshot_t *m, const char *banner, kind_t banner_kind)
{
    const fault_state_t fs = fault_state(m);
    const uint32_t badge = fs == FS_FAULT ? HEART : SUN;
    for (int i = 0; i < 5; ++i) {
        const bool on = i == (int)page;
        lv_obj_set_style_image_recolor(rail_icon[i], C(on ? WHITE : DIM), 0);
        lv_obj_set_style_text_color(rail_label[i], C(on ? WHITE : DIM), 0);
        if (on) lv_obj_set_hidden(rail_mark[i], false); else lv_obj_set_hidden(rail_mark[i], true);
        lv_obj_set_pos(rail_icon[i], (51 - 22) / 2 + 1, 12);
        lv_obj_set_pos(rail_label[i], (51 - width_of(rail_label[i])) / 2 + 1, 40);
    }
    lv_obj_set_style_bg_color(rail_badge[FAULTS], C(badge), 0);
    if (fs == FS_FAULT || fs == FS_INHIBIT) lv_obj_set_hidden(rail_badge[FAULTS], false);
    else lv_obj_set_hidden(rail_badge[FAULTS], true);

    char b[40], t[24];
    uint32_t pc = LABEL;
    if (!link_started) { snprintf(b, sizeof(b), "UART FAILED"); pc = HEART; }
    else if (!m->online) { fmt_age(t, sizeof(t), m->reply_age_ms); snprintf(b, sizeof(b), "OFFLINE \xC2\xB7 %s", t); }
    else { fmt_age(t, sizeof(t), m->reply_age_ms); snprintf(b, sizeof(b), "ONLINE \xC2\xB7 %s", t); pc = POW; }
    lv_label_set_text(hdr_text, b);
    lv_obj_set_style_text_color(hdr_text, C(pc), 0);
    lv_obj_set_style_bg_color(hdr_dot, C(pc), 0);
    const int tw = width_of(hdr_text), pw = 24 + tw + 10;
    lv_obj_set_size(hdr_pill, pw, 22);
    lv_obj_set_pos(hdr_text, 24, (22 - (int)lv_font_get_line_height(F12B)) / 2);
    const int dev_w = width_of(hdr_dev);
    lv_obj_set_x(hdr_pill, 60 + dev_w + (412 - dev_w - pw - 32) / 2);

    const uint32_t fbg = fs == FS_FAULT ? FAULTBG : fs == FS_INHIBIT ? WARNBG : CARD;
    lv_obj_set_style_bg_color(hdr_fault_box, C(fbg), 0);
    lv_image_set_src(hdr_fault_icon, fs == FS_OK ? &hmi_icon_checkc_17 : &hmi_icon_warn_17);
    lv_obj_set_style_image_recolor(hdr_fault_icon, C(fs == FS_FAULT ? HEART : fs == FS_INHIBIT ? SUN : fs == FS_UNKNOWN ? LABEL : DIM), 0);
    lv_obj_set_pos(hdr_fault_icon, 7, 5);

    if (banner) {
        const uint32_t fg = banner_kind == K_FAULT ? HEART : banner_kind == K_WARN ? SUN : LABEL;
        const uint32_t bg = banner_kind == K_FAULT ? FAULTBG : banner_kind == K_WARN ? WARNBG : OFFBG;
        lv_obj_set_hidden(banner_box, false);
        lv_obj_set_style_bg_color(banner_box, C(bg), 0);
        lv_label_set_text(banner_text, banner);
        lv_obj_set_style_text_color(banner_text, C(fg), 0);
        lv_obj_set_style_image_recolor(banner_icon, C(fg), 0);
        const int w = 14 + 6 + width_of(banner_text), x0 = (428 - w) / 2;
        lv_obj_set_pos(banner_icon, x0, 2);
        lv_obj_set_pos(banner_text, x0 + 14 + 6, (18 - (int)lv_font_get_line_height(F12B)) / 2);
    } else lv_obj_set_hidden(banner_box, true);
}

/* ------------------------------------------------------------------ refresh */
static void rebuild(void)
{
    rcc_snapshot_t m;
    rcc_link_snapshot(&m);
    const char *banner = NULL;
    kind_t bk = K_NORMAL;
    char btext[48];
    if (!link_started) { banner = "UART INIT FAILED"; bk = K_FAULT; }
    else if (!m.online) { banner = "LINK LOST \xC2\xB7 CHARGING MAY CONTINUE"; bk = K_OFF; }
    else if (no_authority(&m) && m.command_age_ms < 15000u) { banner = "HMI HAS NO CONTROL AUTHORITY"; bk = K_WARN; }
    else if (fresh(&m) && m.top_state == 6) { snprintf(btext, sizeof(btext), "FAULT ACTIVE \xC2\xB7 TAP TO VIEW"); banner = btext; bk = K_FAULT; }
    update_chrome(&m, banner, bk);

    bar_spec_t spec; bar_spec(&m, &spec);
    char sig[sizeof(bar_signature)];
    snprintf(sig, sizeof(sig), "%d|%d|%d|%s|%s|%u", (int)spec.kind, (int)spec.button, (int)page, spec.title, spec.sub, (unsigned)spec.title_color);
    if (strcmp(sig, bar_signature) != 0) { memcpy(bar_signature, sig, sizeof(sig)); build_bar(&spec); }

    lv_obj_clean(content);
    const int top = banner ? 56 : 38;
    const int bottom = spec.kind == BAR_NONE ? 312 : 262;
    if (page == OVERVIEW) {
        int reason = 0;
        if (spec.kind == BAR_FULL && spec.button == B_STOP && !start_allowed(&m)) reason = !fresh(&m) ? 1 : (m.top_state == 6 ? 2 : 0);
        page_overview(content, &m, banner != NULL, reason);
    } else {
        lv_obj_t *p = obj(content, 60, top, 412, bottom - top);
        switch (page) {
        case MEASURE: page_measure(p, &m); break;
        case FAULTS: page_faults(p, &m, bottom - top); break;
        case SESSION: page_session(p, &m, bottom - top); break;
        default: page_device(p, &m); break;
        }
    }
    dirty = false;
}
static void redraw_cb(void *unused) { (void)unused; rebuild(); }

static void refresh(lv_timer_t *timer)
{
    (void)timer;
    for (lv_indev_t *i = lv_indev_get_next(NULL); i; i = lv_indev_get_next(i))
        if (lv_indev_get_state(i) == LV_INDEV_STATE_PRESSED) return;   /* never rebuild under a finger */
    if (modal) return;
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
    bar_box = obj(screen, 60, 268, 412, 44);
    content = obj(screen, 0, 0, 480, 320);
    lv_obj_move_background(content);
    lv_timer_create(refresh, 1000, NULL);
    rebuild();
}
