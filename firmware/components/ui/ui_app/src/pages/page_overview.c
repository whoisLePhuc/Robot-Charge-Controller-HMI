/*
 * Overview page (retained): state hero with stage strip, system rows, and the volt/current/power cards.
 * Widgets are created once in enter(); the model subjects (hm_hero, hm_sys, hm_tel[]) redraw them when the data changes.
 */
#include <string.h>

#include "hmi_data_widgets.h"
#include "hmi_icons.h"
#include "hmi_model.h"
#include "hmi_nav.h"
#include "hmi_pages.h"
#include "hmi_present.h"
#include "hmi_theme_present.h"
#include "hmi_widgets.h"
#include "page_device.h"

#include "hmi_lvgl_headers.h"
#include "rcc_overview_block_gen.h"
#include "rcc_overview_connector_gen.h"
#include "rcc_overview_hero_gen.h"
#include "rcc_overview_step_gen.h"
#include "rcc_overview_system_gen.h"
#include "rcc_telemetry_card_gen.h"

typedef struct {
    lv_obj_t *val, *unit, *fill, *sub, *ic;
    int ix, iw, unit_gap;
    lv_color_t value_color;
    uint32_t col;                           /* icon accent */
} tel_ui_t;

static struct {
    lv_obj_t *root, *name, *sub, *strip;
    int state_x, summary_gap, summary_edge;
    lv_color_t state_color, summary_color;
    int built_stage;                        /* stage the strip shows (-2: not built yet) */
    bool has_stage;                         /* the layout was built for a hero with a stage strip (74 px) */
    struct { lv_obj_t *ic, *key, *val; int value_gap, value_edge; } row[4];
    tel_ui_t tel[HP_TEL_COUNT];
    lv_obj_t *sb, *sbl;                     /* start-block strip (NULL while START is not blocked) */
} ov;

/* Connected stepper, rebuilt inside `st` (coordinates of the hero card) only when the stage changes. */
static void build_strip(lv_obj_t *st, int stage)
{
    static const char *const stg[5] = {"IDLE", "PRE", "RELAY", "VERIFY", "CHG"};
    int step_width[5], sum = 0;
    for (int i = 0; i < 5; ++i) {
        step_width[i] = hmi_data_ascii_width(stg[i], HF_10, 0) + 24 + (stage == i ? 2 : 0);
        sum += step_width[i];
    }

    const int conn = (382 - sum) / 4;
    int x = 13;
    for (int i = 0; i < 5; ++i) {
        const bool cur = stage == i, done = stage > i;
        rcc_overview_step_create(st, x, step_width[i], cur ? 1 : 0, stg[i],
                                 HC(cur ? HC_NAV_CUR : done ? HC_TXT2 : HC_DIM),
                                 HC(cur ? HC_NAV_CUR : done ? HC_OK_COL : HC_DIM),
                                 cur ? LV_OPA_COVER : LV_OPA_TRANSP, HMI_DATA_FONT(HF_10));
        if (i < 4)
            rcc_overview_connector_create(st, x + step_width[i] + 3, conn - 6, HC(done ? HC_NAV_EDGE : HC_LINE));
        x += step_width[i] + conn;
    }
}

static void apply_hero(lv_observer_t *o, lv_subject_t *s)
{
    (void)o;
    const hp_hero_view_t *v = lv_subject_get_pointer(s);
    const lv_color_t color = HC(ht_kind_color(v->kind));
    const bool normal = v->kind == HP_K_NORMAL;
    hw_label_set(ov.name, v->name);
    lv_obj_set_style_text_color(ov.name, normal ? ov.state_color : color, 0);
    const int sx = ov.state_x + hw_width_of(ov.name) + ov.summary_gap;
    hw_label_set(ov.sub, v->sub);
    lv_obj_set_style_text_color(ov.sub, normal ? ov.summary_color : color, 0);
    lv_obj_set_width(ov.sub, ov.summary_edge - sx);
    lv_obj_set_x(ov.sub, sx);
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
    lv_obj_set_y(ov.row[1].ic, lv_obj_get_y_aligned(ov.row[1].key) - 4 + (22 - (int)ic1->header.h) / 2);
    lv_obj_set_style_image_recolor(ov.row[0].ic, HC(v->online ? HC_OK_COL : HC_OFF_COL), 0);
    lv_obj_set_style_image_recolor(ov.row[1].ic, HC(fc), 0);
    hw_label_set(ov.row[3].key, v->sim ? "Session (SIM V/I)" : "Sess");
    for (int i = 0; i < 4; ++i) {
        lv_obj_t *l = ov.row[i].val;
        hw_label_set(l, val[i]);
        lv_obj_set_style_text_color(l, HC(vc[i]), 0);
        const int x = lv_obj_get_x_aligned(ov.row[i].key) + hw_width_of(ov.row[i].key) + ov.row[i].value_gap;
        lv_obj_set_width(l, ov.row[i].value_edge - x);
        lv_obj_set_x(l, x);
    }
}

static void apply_tel(lv_observer_t *o, lv_subject_t *s)
{
    tel_ui_t *u = lv_observer_get_user_data(o);
    const hp_tel_view_t *v = lv_subject_get_pointer(s);
    hw_label_set(u->val, v->val);
    lv_obj_set_style_text_color(u->val, v->stale ? HC(HC_DIM) : u->value_color, 0);
    const int vw = hw_width_of(u->val);
    lv_obj_set_hidden(u->unit, v->dash);
    lv_obj_set_x(u->unit, u->ix + vw + u->unit_gap);
    lv_obj_set_style_image_recolor(u->ic, HC(v->stale ? HC_DIM : u->col), 0);
    int fw = u->iw * v->permille / 1000;
    if (fw > u->iw) fw = u->iw;
    lv_obj_set_hidden(u->fill, v->stale || fw <= 0);
    lv_obj_set_width(u->fill, fw < 3 ? 3 : fw);
    hw_label_set(u->sub, v->sub);
}

static void tel_card(tel_ui_t *u, lv_obj_t *parent, lv_subject_t *subject, int x, int y, int height,
                     const char *title, const char *unit, uint32_t accent, const char *reading,
                     int unit_x, const lv_image_dsc_t *icon)
{
    lv_obj_t *card = rcc_telemetry_card_create(parent, x, y, height, title, unit, HC(accent),
                                              reading, "", 3, unit_x, HC(HC_WHITE), false, true,
                                              HMI_DATA_FONT(HF_10), HMI_DATA_FONT(HF_22B),
                                              HMI_DATA_FONT(HF_12B), icon);
    u->val = hmi_data_widget(card, "voltage_value");
    u->unit = hmi_data_widget(card, "voltage_unit");
    u->fill = hmi_data_widget(card, "voltage_fill");
    u->sub = hmi_data_widget(card, "voltage_caption");
    u->ic = hmi_data_widget(card, "voltage_icon");
    const lv_obj_t *track = hmi_data_widget(card, "voltage_track");

    const int sample_width = hw_width_of(u->val);
    u->ix = lv_obj_get_x_aligned(u->val);
    u->unit_gap = lv_obj_get_x_aligned(u->unit) - u->ix - sample_width;
    u->iw = lv_obj_get_width(track);
    u->value_color = lv_obj_get_style_text_color(u->val, 0);
    u->col = lv_color_to_u32(lv_obj_get_style_image_recolor(u->ic, 0));
    hmi_nav_make_tappable(card, HMI_PAGE_MEASURE, -1);
    lv_subject_add_observer_obj(subject, apply_tel, card, u);
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
        ov.sb = rcc_overview_block_create(ov.root, ctx->height, HMI_DATA_FONT(HF_10));
        ov.sbl = hmi_data_widget(ov.sb, "reason");
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
    lv_obj_t *hero = rcc_overview_hero_create(p, hero_h, HMI_DATA_FONT(HF_28H), HMI_DATA_FONT(HF_10),
                                              "CHARGING", "", 186);
    ov.name = hmi_data_widget(hero, "state");
    ov.sub = hmi_data_widget(hero, "summary");
    ov.strip = hmi_data_widget(hero, "steps");
    const int sample_width = hw_width_of(ov.name);
    const int summary_x = lv_obj_get_x_aligned(ov.sub);
    ov.state_x = lv_obj_get_x_aligned(ov.name);
    ov.summary_gap = summary_x - ov.state_x - sample_width;
    ov.summary_edge = summary_x + lv_obj_get_width(ov.sub);
    ov.state_color = lv_obj_get_style_text_color(ov.name, 0);
    ov.summary_color = lv_obj_get_style_text_color(ov.sub, 0);
    lv_subject_add_observer_obj(hm_hero, apply_hero, hero, NULL);

    /* System card and firmware navigation targets. */
    const int gy = hero_h + 6, gh = H - gy;
    lv_obj_t *sys = rcc_overview_system_create(p, gy, gh, HMI_DATA_FONT(HF_10), HMI_DATA_FONT(HF_12B),
                                               &hmi_icon_link_13, &hmi_icon_check_12, &hmi_icon_relay_13,
                                               &hmi_icon_clock_13);
    static const char *const icon_names[] = {"uart_icon_view", "fault_icon_view", "relay_icon_view", "session_icon_view"};
    static const char *const key_names[] = {"uart_key", "fault_key", "relay_key", "session_key"};
    static const char *const value_names[] = {"uart_value", "fault_value", "relay_value", "session_value"};
    for (int i = 0; i < 4; ++i) {
        ov.row[i].ic = hmi_data_widget(sys, icon_names[i]);
        ov.row[i].key = hmi_data_widget(sys, key_names[i]);
        ov.row[i].val = hmi_data_widget(sys, value_names[i]);
        const int key_width = hw_width_of(ov.row[i].key);
        const int value_x = lv_obj_get_x_aligned(ov.row[i].val);
        ov.row[i].value_gap = value_x - lv_obj_get_x_aligned(ov.row[i].key) - key_width;
        ov.row[i].value_edge = value_x + lv_obj_get_width(ov.row[i].val);
    }
    hmi_nav_make_tappable(hmi_data_widget(sys, "uart_hit"), HMI_PAGE_DEVICE, HMI_DEV_LINK);
    hmi_nav_make_tappable(hmi_data_widget(sys, "fault_hit"), HMI_PAGE_FAULTS, -1);
    hmi_nav_make_tappable(hmi_data_widget(sys, "session_hit"), HMI_PAGE_SESSION, -1);
    lv_subject_add_observer_obj(hm_sys, apply_sys, sys, NULL);

    tel_card(&ov.tel[HP_TEL_V], p, hm_tel[HP_TEL_V], 164, gy, gh, "VOLT", "V", HC_VOLT_COL,
             "48.2", 60, &hmi_icon_bolt_13);
    tel_card(&ov.tel[HP_TEL_A], p, hm_tel[HP_TEL_A], 247, gy, gh, "CURR.", "A", HC_AMP_COL,
             "8.0", 47, &hmi_icon_wave_13);
    tel_card(&ov.tel[HP_TEL_W], p, hm_tel[HP_TEL_W], 331, gy, gh, "POWER", "W", HC_WATT_COL,
             "386", 53, &hmi_icon_power_13);

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
