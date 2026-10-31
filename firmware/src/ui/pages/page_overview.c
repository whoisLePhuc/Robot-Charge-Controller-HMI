/*
 * Overview page: state hero with stage strip, system rows, and the volt/current/power cards.
 */
#include <stdio.h>
#include <string.h>

#include "hmi_icons.h"
#include "hmi_nav.h"
#include "hmi_pages.h"
#include "hmi_present.h"
#include "hmi_theme_present.h"
#include "hmi_widgets.h"
#include "page_device.h"

static void tel_card(lv_obj_t *p, int x, int y, int w, int h, const char *label, const char *val,
                     const char *unit, int permille, const char *maxs, uint32_t col,
                     const lv_image_dsc_t *ic, bool stale)
{
    lv_obj_t *c = hw_card_box(p, x, y, w, h, HC_CARD, 10, HC_LINE);
    hmi_nav_make_tappable(c, HMI_PAGE_MEASURE, -1);
    lv_obj_set_style_border_color(c, HC(HC_PRESS_BORDER), LV_STATE_PRESSED);
    const int ix = 9, iw = w - 18, T = 8, B = h - 8;
    const int gap = (B - T - (13 + 27 + 5 + 13)) / 3;
    hw_text(c, label, ix, T + (13 - hw_lh(HF_10)) / 2, HF_10, HC_LABEL);
    hw_icon(c, ic, ix + iw - (int)ic->header.w, T, stale ? HC_DIM : col);

    const int vy = T + 13 + gap + 1;
    lv_obj_t *vl = hw_text(c, val, ix, 0, HF_22B, stale ? HC_DIM : HC_WHITE);
    const int vw = hw_width_of(vl);
    lv_obj_set_pos(vl, ix, vy + (27 - hw_lh(HF_22B)) / 2);
    const bool dash = (unsigned char)val[0] == 0xE2 && (unsigned char)val[1] == 0x80;
    if (!dash) hw_text(c, unit, ix + vw + 2, vy + 7, HF_12B, HC_LABEL);

    const int by = T + 13 + gap + 27 + gap;
    hw_box(c, ix, by, iw, 5, HC_SURFACE2, 3);
    int fw = iw * permille / 1000;
    if (fw > iw) fw = iw;
    if (!stale && fw > 0) hw_box(c, ix, by, fw < 3 ? 3 : fw, 5, col, 3);
    lv_obj_t *tick = hw_box(c, ix + iw * 8 / 10, by, 2, 5, HC_HERO_COL, 0);
    lv_obj_set_style_bg_opa(tick, 115, 0);

    char sub[24];
    if (stale) snprintf(sub, sizeof(sub), "STALE DATA");
    else snprintf(sub, sizeof(sub), "%d%%/%s", (permille + 5) / 10, maxs);
    hw_text_in(c, sub, ix, B - 13, iw, 13, HF_10, HC_DIM, LV_TEXT_ALIGN_LEFT);
}

void page_overview_build(lv_obj_t *p, const rcc_snapshot_t *m, const hmi_page_ctx_t *ctx)
{
    const int H = ctx->height;
    hp_state_info_t s; hp_state_info(m, &s);
    const uint32_t kc = s.kind == HP_K_NORMAL ? HC_HERO_COL : ht_kind_color(s.kind);

    /* Hero card with the connected stepper. */
    const int hero_h = s.stage >= 0 ? 74 : 72;
    lv_obj_t *hero = hw_card_box(p, 0, 0, 408, hero_h, HC_CARD, 10, HC_LINE);
    lv_obj_t *st = hw_text(hero, s.name, 0, 0, HF_28H, kc);
    const int sw = hw_width_of(st);
    lv_obj_set_pos(st, 13, 10 + (30 - hw_lh(HF_28H)) / 2);
    hw_text_in(hero, s.sub, 13 + sw + 8, 22, 374 - sw, 14, HF_10, s.kind == HP_K_NORMAL ? HC_LABEL : kc, LV_TEXT_ALIGN_RIGHT);
    hw_hline(hero, 13, 42, 382, HC_LINE);

    static const char *const stg[5] = {"IDLE", "PRE", "RELAY", "VERIFY", "CHG"};
    lv_obj_t *tl[5]; int tw[5], sum = 0;
    for (int i = 0; i < 5; ++i) {
        const bool cur = s.stage == i, done = s.stage > i;
        tl[i] = hw_text(hero, stg[i], 0, 0, HF_10, cur ? HC_NAV_CUR : done ? HC_TXT2 : HC_DIM);
        tw[i] = hw_width_of(tl[i]);
        sum += tw[i] + 24 + (cur ? 2 : 0);
    }
    const int conn = (382 - sum) / 4;
    int x = 13;
    for (int i = 0; i < 5; ++i) {
        const bool cur = s.stage == i, done = s.stage > i;
        const int nw = tw[i] + 24 + (cur ? 2 : 0), o = cur ? 1 : 0;
        if (cur) hw_card_box(hero, x, 48, nw, 19, HC_NAV_BOX, 5, HC_NAV_EDGE);
        hw_box(hero, x + 7 + o, 54, 6, 6, cur ? HC_NAV_CUR : done ? HC_OK_COL : HC_DIM, 3);
        lv_obj_move_foreground(tl[i]);
        lv_obj_set_pos(tl[i], x + 17 + o, 57 - hw_lh(HF_10) / 2);
        if (i < 4) hw_hline(hero, x + nw + 3, 57, conn - 6, done ? HC_NAV_EDGE : HC_LINE);
        x += nw + conn;
    }

    /* System card. */
    const int gy = hero_h + 6, gh = H - gy;
    lv_obj_t *sys = hw_card_box(p, 0, gy, 156, gh, HC_CARD, 10, HC_LINE);
    const int gap = (gh - 14 - 88) / 3;
    const hp_fault_state_t fs = hp_fault_state(m);
    const bool online = m->online;
    struct { const lv_image_dsc_t *ic; uint32_t ic_col; const char *k; char v[40]; uint32_t vc; } row[4];
    row[0].ic = &hmi_icon_link_13; row[0].ic_col = online ? HC_OK_COL : HC_OFF_COL; row[0].k = "UART";
    hp_fmt_age(row[0].v, sizeof(row[0].v), m->reply_age_ms); row[0].vc = online ? HC_WHITE : HC_OFF_COL;
    row[1].ic = fs == HP_FS_OK ? &hmi_icon_check_12 : &hmi_icon_warn_14; row[1].k = "Fault";
    row[1].ic_col = ht_fault_color(fs);
    if (fs == HP_FS_FAULT) snprintf(row[1].v, sizeof(row[1].v), "%s", hp_fault_short(m->primary_fault));
    else if (fs == HP_FS_INHIBIT) snprintf(row[1].v, sizeof(row[1].v), "Inhibited");
    else if (fs == HP_FS_UNKNOWN) snprintf(row[1].v, sizeof(row[1].v), "Unreadable");
    else snprintf(row[1].v, sizeof(row[1].v), "No faults");
    row[1].vc = fs == HP_FS_OK ? HC_TXT2 : row[1].ic_col;
    row[2].ic = &hmi_icon_relay_13; row[2].ic_col = HC_LABEL; row[2].k = "Relay cmd";
    snprintf(row[2].v, sizeof(row[2].v), "%s", hp_fresh(m) ? (m->relay_command ? "CLOSED" : "OPEN") : "\xE2\x80\x94");
    row[2].vc = HC_WHITE;
    row[3].ic = &hmi_icon_clock_13; row[3].ic_col = HC_LABEL; row[3].k = "Sess";
    hp_session_line(row[3].v, sizeof(row[3].v), m); row[3].vc = HC_TXT2;
    if (m->simulated_measurements) row[3].k = "Session (SIM V/I)";
    for (int i = 0; i < 4; ++i) {
        const int ry = 7 + i * (22 + gap);
        if (i != 2) {
            lv_obj_t *hit = hw_obj(sys, 2, ry - 3, 152, 28);
            hmi_nav_make_tappable(hit, i == 0 ? HMI_PAGE_DEVICE : i == 1 ? HMI_PAGE_FAULTS : HMI_PAGE_SESSION, i == 0 ? HMI_DEV_LINK : -1);
            lv_obj_set_style_bg_color(hit, HC(HC_SURFACE2), LV_STATE_PRESSED);
            lv_obj_set_style_bg_opa(hit, LV_OPA_COVER, LV_STATE_PRESSED);
            lv_obj_set_style_radius(hit, 6, 0);
        }
        hw_icon_row(sys, row[i].ic, 11, ry, 22, row[i].ic_col);
        lv_obj_t *kl = hw_text(sys, row[i].k, 0, 0, HF_10, HC_LABEL);
        const int kw = hw_width_of(kl);
        lv_obj_set_pos(kl, 29, ry + (22 - hw_lh(HF_10)) / 2);
        const int vx = 29 + kw + 6;
        hw_text_in(sys, row[i].v, vx, ry, 145 - vx, 22, HF_12B, row[i].vc, LV_TEXT_ALIGN_RIGHT);
    }

    /* Telemetry cards. */
    const bool stale = hp_meas_stale(m);
    char vs[16] = "\xE2\x80\x94", as[16] = "\xE2\x80\x94", ws[16] = "\xE2\x80\x94";
    hp_gauge_t g; hp_gauge(m, &g);
    if (hp_v_valid(m)) {
        hp_fmt_milli(vs, sizeof(vs), (int32_t)m->vout_mv, hp_v_floor(m));
        if (hp_v_floor(m)) memmove(vs + 3, vs + 4, strlen(vs + 4) + 1);   /* "\xE2\x89\xA4 3.4" -> "\xE2\x89\xA43.4" */
    }
    if (hp_i_valid(m)) hp_fmt_milli(as, sizeof(as), m->iout_ma, false);
    if (g.power_valid) snprintf(ws, sizeof(ws), "%d", g.power_w);
    tel_card(p, 164, gy, 77, gh, "VOLT", vs, "V", g.v_pm, HP_V_FULL_TXT, HC_VOLT_COL, &hmi_icon_bolt_13, stale);
    tel_card(p, 247, gy, 77, gh, "CURR.", as, "A", g.a_pm, HP_I_FULL_TXT, HC_AMP_COL, &hmi_icon_wave_13, stale);
    tel_card(p, 331, gy, 77, gh, "POWER", ws, "W", g.w_pm, HP_P_FULL_TXT, HC_WATT_COL, &hmi_icon_power_13, stale);

    if (ctx->bar->start_block != HP_SB_NONE) {
        const uint32_t pc = HC_WARN_COL;
        lv_obj_t *r = hw_card_box(p, 164, H - 2 - 19, 244, 19, HC_WARN_BG, 6, pc);
        hw_text_in(r, hp_start_block_text(ctx->bar->start_block), 0, 0, 244, 19, HF_10, pc, LV_TEXT_ALIGN_CENTER);
    }
}
