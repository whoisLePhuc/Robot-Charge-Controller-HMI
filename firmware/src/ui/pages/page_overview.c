/*
 * Overview page (retained): state hero with stage strip, system rows, and the volt/current/power cards.
 * Widgets are created once in enter(); the model subjects (hm_hero, hm_sys, hm_tel[]) redraw them when the data changes.
 */
#include <string.h>

#include "hmi_icons.h"
#include "hmi_model.h"
#include "hmi_nav.h"
#include "hmi_pages.h"
#include "hmi_present.h"
#include "hmi_theme_present.h"
#include "hmi_widgets.h"
#include "page_device.h"

typedef struct {
    lv_obj_t *val, *unit, *fill, *sub, *ic;
    int ix, iw, by, vy;                     /* card geometry (fixed per card) */
    uint32_t col;                           /* accent: icon and gauge fill */
} tel_ui_t;

static struct {
    lv_obj_t *root, *name, *sub, *strip;
    int built_stage;                        /* stage the strip shows (-2: not built yet) */
    bool has_stage;                         /* the layout was built for a hero with a stage strip (74 px) */
    struct { lv_obj_t *ic, *key, *val; int ry, vx; } row[4];
    tel_ui_t tel[HP_TEL_COUNT];
    lv_obj_t *sb, *sbl;                     /* start-block strip (NULL while START is not blocked) */
} ov;

/* Connected stepper, rebuilt inside `st` (coordinates of the hero card) only when the stage changes. */
static void build_strip(lv_obj_t *st, int stage)
{
    static const char *const stg[5] = {"IDLE", "PRE", "RELAY", "VERIFY", "CHG"};
    lv_obj_t *tl[5]; int tw[5], sum = 0;
    for (int i = 0; i < 5; ++i) {
        const bool cur = stage == i, done = stage > i;
        tl[i] = hw_text(st, stg[i], 0, 0, HF_10, cur ? HC_NAV_CUR : done ? HC_TXT2 : HC_DIM);
        tw[i] = hw_width_of(tl[i]);
        sum += tw[i] + 24 + (cur ? 2 : 0);
    }
    const int conn = (382 - sum) / 4;
    int x = 13;
    for (int i = 0; i < 5; ++i) {
        const bool cur = stage == i, done = stage > i;
        const int nw = tw[i] + 24 + (cur ? 2 : 0), o = cur ? 1 : 0;
        if (cur) hw_card_box(st, x, 48, nw, 19, HC_NAV_BOX, 5, HC_NAV_EDGE);
        hw_box(st, x + 7 + o, 54, 6, 6, cur ? HC_NAV_CUR : done ? HC_OK_COL : HC_DIM, 3);
        lv_obj_move_foreground(tl[i]);
        lv_obj_set_pos(tl[i], x + 17 + o, 57 - hw_lh(HF_10) / 2);
        if (i < 4) hw_hline(st, x + nw + 3, 57, conn - 6, done ? HC_NAV_EDGE : HC_LINE);
        x += nw + conn;
    }
}

static void apply_hero(lv_observer_t *o, lv_subject_t *s)
{
    (void)o;
    const hp_hero_view_t *v = lv_subject_get_pointer(s);
    const uint32_t kc = v->kind == HP_K_NORMAL ? HC_HERO_COL : ht_kind_color(v->kind);
    hw_label_set(ov.name, v->name);
    lv_obj_set_style_text_color(ov.name, HC(kc), 0);
    const int sw = hw_width_of(ov.name);
    lv_obj_set_pos(ov.name, 13, 10 + (30 - hw_lh(HF_28H)) / 2);
    hw_label_set(ov.sub, v->sub);
    lv_obj_set_style_text_color(ov.sub, HC(v->kind == HP_K_NORMAL ? HC_LABEL : kc), 0);
    lv_obj_set_size(ov.sub, 374 - sw, hw_lh(HF_10));
    lv_obj_set_pos(ov.sub, 13 + sw + 8, 22 + (14 - hw_lh(HF_10)) / 2);
    if (ov.built_stage != v->stage) {
        lv_obj_clean(ov.strip);
        build_strip(ov.strip, v->stage);
        ov.built_stage = v->stage;
    }
}

static void apply_sys(lv_observer_t *o, lv_subject_t *s)
{
    (void)o;
    const hp_sys_view_t *v = lv_subject_get_pointer(s);
    const uint32_t fc = ht_fault_color(v->fs);
    const char *val[4] = {v->uart, v->fault, v->relay, v->sess};
    const uint32_t vc[4] = {v->online ? HC_WHITE : HC_OFF_COL, v->fs == HP_FS_OK ? HC_TXT2 : fc, HC_WHITE, HC_TXT2};
    const lv_image_dsc_t *ic1 = v->fs == HP_FS_OK ? &hmi_icon_check_12 : &hmi_icon_warn_14;
    lv_image_set_src(ov.row[1].ic, ic1);
    lv_obj_set_y(ov.row[1].ic, ov.row[1].ry + (22 - (int)ic1->header.h) / 2);
    lv_obj_set_style_image_recolor(ov.row[0].ic, HC(v->online ? HC_OK_COL : HC_OFF_COL), 0);
    lv_obj_set_style_image_recolor(ov.row[1].ic, HC(fc), 0);
    hw_label_set(ov.row[3].key, v->sim ? "Session (SIM V/I)" : "Sess");
    ov.row[3].vx = 29 + hw_width_of(ov.row[3].key) + 6;
    for (int i = 0; i < 4; ++i) {
        lv_obj_t *l = ov.row[i].val;
        hw_label_set(l, val[i]);
        lv_obj_set_style_text_color(l, HC(vc[i]), 0);
        lv_obj_set_size(l, 145 - ov.row[i].vx, hw_lh(HF_12B));
        lv_obj_set_pos(l, ov.row[i].vx, ov.row[i].ry + (22 - hw_lh(HF_12B)) / 2);
    }
}

static void apply_tel(lv_observer_t *o, lv_subject_t *s)
{
    tel_ui_t *u = lv_observer_get_user_data(o);
    const hp_tel_view_t *v = lv_subject_get_pointer(s);
    hw_label_set(u->val, v->val);
    lv_obj_set_style_text_color(u->val, HC(v->stale ? HC_DIM : HC_WHITE), 0);
    const int vw = hw_width_of(u->val);
    lv_obj_set_pos(u->val, u->ix, u->vy + (27 - hw_lh(HF_22B)) / 2);
    lv_obj_set_hidden(u->unit, v->dash);
    lv_obj_set_x(u->unit, u->ix + vw + 2);
    lv_obj_set_style_image_recolor(u->ic, HC(v->stale ? HC_DIM : u->col), 0);
    int fw = u->iw * v->permille / 1000;
    if (fw > u->iw) fw = u->iw;
    lv_obj_set_hidden(u->fill, v->stale || fw <= 0);
    lv_obj_set_width(u->fill, fw < 3 ? 3 : fw);
    hw_label_set(u->sub, v->sub);
}

static void tel_card(tel_ui_t *u, lv_obj_t *p, lv_subject_t *subj, int x, int y, int w, int h, const char *label,
                     const char *unit, uint32_t col, const lv_image_dsc_t *ic)
{
    lv_obj_t *c = hw_card_box(p, x, y, w, h, HC_CARD, 10, HC_LINE);
    hmi_nav_make_tappable(c, HMI_PAGE_MEASURE, -1);
    lv_obj_set_style_border_color(c, HC(HC_PRESS_BORDER), LV_STATE_PRESSED);
    const int ix = 9, iw = w - 18, T = 8, B = h - 8;
    const int gap = (B - T - (13 + 27 + 5 + 13)) / 3;
    u->ix = ix; u->iw = iw; u->col = col;
    hw_text(c, label, ix, T + (13 - hw_lh(HF_10)) / 2, HF_10, HC_LABEL);
    u->ic = hw_icon(c, ic, ix + iw - (int)ic->header.w, T, col);

    u->vy = T + 13 + gap + 1;
    u->val = hw_text(c, "", ix, 0, HF_22B, HC_WHITE);
    u->unit = hw_text(c, unit, ix, u->vy + 7, HF_12B, HC_LABEL);

    u->by = T + 13 + gap + 27 + gap;
    hw_box(c, ix, u->by, iw, 5, HC_SURFACE2, 3);
    u->fill = hw_box(c, ix, u->by, 3, 5, col, 3);
    lv_obj_t *tick = hw_box(c, ix + iw * 8 / 10, u->by, 2, 5, HC_HERO_COL, 0);
    lv_obj_set_style_bg_opa(tick, 115, 0);
    u->sub = hw_text_in(c, "", ix, B - 13, iw, 13, HF_10, HC_DIM, LV_TEXT_ALIGN_LEFT);
    lv_subject_add_observer_obj(subj, apply_tel, c, u);
}

/* The start-block strip is not model data (the shell's action-bar spec decides it) and is rarely shown, so it is
 * created on top of the page when needed and deleted when not (nothing sits above it, as in the old builder). */
static void sync_start_block(const hmi_page_ctx_t *ctx)
{
    const hp_start_block_t sb = ctx->bar->start_block;
    if (sb == HP_SB_NONE) {
        if (ov.sb) { lv_obj_delete(ov.sb); ov.sb = ov.sbl = NULL; }
        return;
    }
    if (!ov.sb) {
        ov.sb = hw_card_box(ov.root, 164, ctx->height - 2 - 19, 244, 19, HC_WARN_BG, 6, HC_WARN_COL);
        ov.sbl = hw_text_in(ov.sb, "", 0, 0, 244, 19, HF_10, HC_WARN_COL, LV_TEXT_ALIGN_CENTER);
    }
    hw_label_set(ov.sbl, hp_start_block_text(sb));
}

static void build(lv_obj_t *p, const hmi_page_ctx_t *ctx)
{
    lv_obj_clean(p);
    memset(&ov, 0, sizeof(ov));
    ov.root = p; ov.built_stage = -2;
    const int H = ctx->height;
    const hp_hero_view_t *hv = lv_subject_get_pointer(hm_hero);
    ov.has_stage = hv->stage >= 0;

    /* Hero card with the connected stepper. */
    const int hero_h = ov.has_stage ? 74 : 72;
    lv_obj_t *hero = hw_card_box(p, 0, 0, 408, hero_h, HC_CARD, 10, HC_LINE);
    ov.name = hw_text(hero, "", 0, 0, HF_28H, HC_HERO_COL);
    ov.sub = hw_text_in(hero, "", 13, 22, 374, 14, HF_10, HC_LABEL, LV_TEXT_ALIGN_RIGHT);
    hw_hline(hero, 13, 42, 382, HC_LINE);
    ov.strip = hw_obj(hero, 0, 0, 408, hero_h);
    lv_subject_add_observer_obj(hm_hero, apply_hero, hero, NULL);

    /* System card. */
    const int gy = hero_h + 6, gh = H - gy;
    lv_obj_t *sys = hw_card_box(p, 0, gy, 156, gh, HC_CARD, 10, HC_LINE);
    const int gap = (gh - 14 - 88) / 3;
    static const lv_image_dsc_t *const ics[4] = {&hmi_icon_link_13, &hmi_icon_check_12, &hmi_icon_relay_13, &hmi_icon_clock_13};
    static const char *const keys[4] = {"UART", "Fault", "Relay cmd", "Sess"};
    for (int i = 0; i < 4; ++i) {
        const int ry = 7 + i * (22 + gap);
        ov.row[i].ry = ry;
        if (i != 2) {
            lv_obj_t *hit = hw_obj(sys, 2, ry - 3, 152, 28);
            hmi_nav_make_tappable(hit, i == 0 ? HMI_PAGE_DEVICE : i == 1 ? HMI_PAGE_FAULTS : HMI_PAGE_SESSION, i == 0 ? HMI_DEV_LINK : -1);
            lv_obj_set_style_bg_color(hit, HC(HC_SURFACE2), LV_STATE_PRESSED);
            lv_obj_set_style_bg_opa(hit, LV_OPA_COVER, LV_STATE_PRESSED);
            lv_obj_set_style_radius(hit, 6, 0);
        }
        ov.row[i].ic = hw_icon_row(sys, ics[i], 11, ry, 22, HC_LABEL);
        ov.row[i].key = hw_text(sys, keys[i], 0, 0, HF_10, HC_LABEL);
        lv_obj_set_pos(ov.row[i].key, 29, ry + (22 - hw_lh(HF_10)) / 2);
        ov.row[i].vx = 29 + hw_width_of(ov.row[i].key) + 6;
        ov.row[i].val = hw_text_in(sys, "", ov.row[i].vx, ry, 145 - ov.row[i].vx, 22, HF_12B, HC_WHITE, LV_TEXT_ALIGN_RIGHT);
    }
    lv_subject_add_observer_obj(hm_sys, apply_sys, sys, NULL);

    /* Telemetry cards. */
    tel_card(&ov.tel[HP_TEL_V], p, hm_tel[HP_TEL_V], 164, gy, 77, gh, "VOLT", "V", HC_VOLT_COL, &hmi_icon_bolt_13);
    tel_card(&ov.tel[HP_TEL_A], p, hm_tel[HP_TEL_A], 247, gy, 77, gh, "CURR.", "A", HC_AMP_COL, &hmi_icon_wave_13);
    tel_card(&ov.tel[HP_TEL_W], p, hm_tel[HP_TEL_W], 331, gy, 77, gh, "POWER", "W", HC_WATT_COL, &hmi_icon_power_13);

    sync_start_block(ctx);
}

static void enter(lv_obj_t *p, const rcc_snapshot_t *m, const hmi_page_ctx_t *ctx)
{
    (void)m;
    build(p, ctx);
}

/* Structural change: the hero is 74 px high with a stage strip and 72 without, which moves every card below it. */
static void update(const rcc_snapshot_t *m, const hmi_page_ctx_t *ctx)
{
    (void)m;
    const hp_hero_view_t *hv = lv_subject_get_pointer(hm_hero);
    if ((hv->stage >= 0) != ov.has_stage) build(ov.root, ctx);
    else sync_start_block(ctx);
}

static void leave(void) { memset(&ov, 0, sizeof(ov)); }

const hmi_page_ops_t page_overview_ops = {.enter = enter, .update = update, .leave = leave};
