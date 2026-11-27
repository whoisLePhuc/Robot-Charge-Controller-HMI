/*
 * Faults page (retained): fault state banner, primary fault card and the scrollable fault/inhibit bit lists.
 * The summary card is bound to hm_fault_sum. Below it is a body whose layout class (hp_fault_body) decides what exists;
 * the body is rebuilt only when that class changes, the primary card is bound to hm_fault_pri and each bit list's
 * rows are refilled only when its mask changes (the scroll offset is kept, also across a body rebuild).
 */
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "hmi_icons.h"
#include "hmi_lvgl_headers.h"
#include "rcc_faults_mask_gen.h"
#include "rcc_faults_primary_gen.h"
#include "rcc_faults_register_gen.h"
#include "rcc_faults_row_gen.h"
#include "rcc_faults_summary_gen.h"
#include "hmi_model.h"
#include "hmi_pages.h"
#include "hmi_present.h"
#include "hmi_theme_present.h"
#include "hmi_widgets.h"

typedef struct {
    lv_obj_t *card, *hdr, *empty, *scr;     /* card, hex mask caption, "None (0x0)", row scroll box */
    int w, slot;                            /* slot: hw_scroll_area slot, keeps the offset across a body rebuild */
    uint32_t mask;
    bool filled;                            /* rows reflect `mask` */
} list_ui_t;

static struct {
    lv_obj_t *root, *sum, *icon, *title, *read, *body;
    int H;
    int bk;                                 /* body class built (hp_fault_body_t), -1 none */
    lv_obj_t *pri, *pri_nm, *pri_id, *pri_act;
    list_ui_t list[2];                      /* FAULTS, INHIBITS */
} fl;

static void apply_sum(lv_observer_t *o, lv_subject_t *s)
{
    (void)o;
    const hp_fault_sum_view_t *v = lv_subject_get_pointer(s);
    const hp_fault_state_t fs = v->fs;
    const uint32_t tc = ht_fault_color(fs);
    lv_obj_set_style_bg_color(fl.sum, HC(ht_fault_bg(fs)), 0);
    lv_obj_set_style_border_color(fl.sum, HC(fs == HP_FS_OK ? HC_LINE : tc), 0);
    const lv_image_dsc_t *d = fs == HP_FS_OK ? &hmi_icon_checkc_18 : &hmi_icon_warn_18;
    lv_image_set_src(fl.icon, d);
    lv_obj_set_style_image_recolor(fl.icon, HC(tc), 0);
    hw_label_set(fl.title, hp_fault_title(fs));
    lv_obj_set_style_text_color(fl.title, HC(fs == HP_FS_OK ? HC_WHITE : tc), 0);
    hw_label_set(fl.read, v->read);
}

static void apply_pri(lv_observer_t *o, lv_subject_t *s)
{
    (void)o;
    const hp_fault_pri_view_t *v = lv_subject_get_pointer(s);
    const uint32_t pc = v->fault ? HC_FAULT_COL : HC_WARN_COL;
    lv_obj_set_style_bg_color(fl.pri, HC(v->fault ? HC_FAULT_BG : HC_WARN_BG), 0);
    lv_obj_set_style_border_color(fl.pri, HC(pc), 0);
    hw_label_set(fl.pri_nm, v->nm);
    lv_obj_set_style_text_color(fl.pri_nm, HC(pc), 0);
    hw_label_set(fl.pri_id, v->id);
    hw_label_set(fl.pri_act, v->act);
}

static void mask_list(list_ui_t *u, int slot, lv_obj_t *p, int x, int y, int h, const char *title)
{
    u->slot = slot;
    u->filled = false;
    u->card = rcc_faults_mask_create(p, x, y, h, title, (lv_font_t *)HF_10, (lv_font_t *)HF_12);
    u->hdr = lv_obj_find_by_name(u->card, "fault_mask_value");
    u->empty = lv_obj_find_by_name(u->card, "fault_mask_empty");
    lv_obj_t *viewport = lv_obj_find_by_name(u->card, "fault_mask_viewport");
    lv_obj_update_layout(viewport);
    const int sx = lv_obj_get_x_aligned(viewport), sy = lv_obj_get_y_aligned(viewport);
    u->w = lv_obj_get_width(viewport);
    const int sh = lv_obj_get_height(viewport);
    lv_obj_delete(viewport);
    u->scr = hw_scroll_area(u->card, sx, sy, u->w, sh, slot);
}

/* Refill one list for `mask`: caption, empty text or rows. The scroll offset survives (LVGL clamps it); a list
 * just created by a body rebuild takes it from its scroll slot. */
static void fill_list(list_ui_t *u, uint32_t mask, const char *const *names, int count)
{
    char hs[16]; hp_mask_caption(hs, sizeof(hs), mask);
    hw_label_set(u->hdr, hs);
    const int n = hp_mask_bits(mask);
    lv_obj_set_hidden(u->empty, n != 0);
    lv_obj_set_hidden(u->scr, n == 0);
    const int keep = n && u->filled ? (int)lv_obj_get_scroll_y(u->scr) : -1;
    lv_obj_clean(u->scr);
    int r = 0;
    for (int b = 0; b < 32; ++b) {
        if (!(mask & (1u << b))) continue;
        const int ry = r * 18;
        char nm[40], bl[8];
        hp_mask_row(nm, sizeof(nm), bl, sizeof(bl), b, names, count);
        rcc_faults_row_create(u->scr, 13, ry, u->w - 50, nm, HC(HC_WHITE),
                              LV_TEXT_ALIGN_LEFT, (lv_font_t *)HF_10);
        rcc_faults_row_create(u->scr, u->w - 37, ry, 24, bl, HC(HC_LABEL),
                              LV_TEXT_ALIGN_RIGHT, (lv_font_t *)HF_10);
        ++r;
    }
    if (n && keep >= 0) hw_scroll_to(u->scr, keep);
    else if (n) hw_scroll_restore(u->scr, u->slot);
    u->mask = mask; u->filled = true;
}

static void build_body(hp_fault_body_t cls)
{
    lv_obj_clean(fl.body);
    memset(&fl.list, 0, sizeof(fl.list));
    fl.bk = (int)cls;
    const int H = fl.H;
    lv_obj_t *p = fl.body;
    if (cls == HP_FB_OK || cls == HP_FB_UNKNOWN) {
        const bool unk = cls == HP_FB_UNKNOWN;
        for (int k = 0; k < 2; ++k) {
            rcc_faults_register_create(p, k * 208, H - 46, k ? "INHIBIT REGISTER" : "FAULT REGISTER",
                                       unk ? "—" : "0x0000", unk ? "Unreadable" : "0 active bits",
                                       unk ? "Link offline" : (k ? "No inhibit bits set" : "No fault bits set"),
                                       HC(unk ? HC_DIM : HC_OK_COL), (lv_font_t *)HF_10, (lv_font_t *)HF_14B);
        }
        return;
    }
    int y = 46;
    if (cls == HP_FB_PRIMARY_LISTS) {
        fl.pri = rcc_faults_primary_create(p, (lv_font_t *)HF_10, (lv_font_t *)HF_12, (lv_font_t *)HF_12B);
        fl.pri_nm = lv_obj_find_by_name(fl.pri, "fault_primary_name");
        fl.pri_id = lv_obj_find_by_name(fl.pri, "fault_primary_id");
        fl.pri_act = lv_obj_find_by_name(fl.pri, "fault_primary_action");
        lv_subject_add_observer_obj(hm_fault_pri, apply_pri, fl.pri, NULL);   /* dies with the card on a body rebuild */
        y += 50;
    }
    mask_list(&fl.list[0], 0, p, 0, y, H - y, "FAULTS");
    mask_list(&fl.list[1], 1, p, 208, y, H - y, "INHIBITS");
}

/* Make the body match `m`: rebuild on a class change, else refill only what differs. */
static void sync_body(const rcc_snapshot_t *m)
{
    const hp_fault_body_t cls = hp_fault_body(m);
    if (fl.bk != (int)cls) build_body(cls);
    if (cls == HP_FB_LISTS || cls == HP_FB_PRIMARY_LISTS) {
        if (!fl.list[0].filled || fl.list[0].mask != m->active_fault_mask) fill_list(&fl.list[0], m->active_fault_mask, hp_fault_names, HP_FAULT_COUNT);
        if (!fl.list[1].filled || fl.list[1].mask != m->inhibit_mask) fill_list(&fl.list[1], m->inhibit_mask, hp_inhibit_names, HP_INHIBIT_COUNT);
    }
}

static void enter(lv_obj_t *p, const rcc_snapshot_t *m, const hmi_page_ctx_t *ctx)
{
    memset(&fl, 0, sizeof(fl));
    fl.root = p; fl.H = ctx->height; fl.bk = -1;
    fl.sum = rcc_faults_summary_create(p, (lv_font_t *)HF_10, (lv_font_t *)HF_14B, &hmi_icon_checkc_18);
    fl.icon = lv_obj_find_by_name(fl.sum, "fault_summary_icon");
    fl.title = lv_obj_find_by_name(fl.sum, "fault_summary_title");
    fl.read = lv_obj_find_by_name(fl.sum, "fault_summary_age");
    lv_subject_add_observer_obj(hm_fault_sum, apply_sum, fl.sum, NULL);
    fl.body = hw_obj(p, 0, 0, 408, fl.H);
    sync_body(m);
}

static void update(const rcc_snapshot_t *m, const hmi_page_ctx_t *ctx)
{
    (void)ctx;
    sync_body(m);
}

static void leave(void) { memset(&fl, 0, sizeof(fl)); }

const hmi_page_ops_t page_faults_ops = {.enter = enter, .update = update, .leave = leave};
