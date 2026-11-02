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
    lv_obj_set_y(fl.icon, (40 - (int)d->header.h) / 2);
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

static void mask_list(list_ui_t *u, int slot, lv_obj_t *p, int x, int y, int w, int h, const char *title)
{
    u->w = w; u->slot = slot; u->filled = false;
    u->card = hw_card_box(p, x, y, w, h, HC_CARD, 10, HC_LINE);
    u->hdr = hw_section(u->card, 13, 7, w - 26, title, "", HC_LABEL);
    u->empty = hw_text_in(u->card, "None (0x0)", 13, 23, w - 26, 18, HF_12, HC_LABEL, LV_TEXT_ALIGN_LEFT);
    u->scr = hw_scroll_area(u->card, 0, 23, w, h - 23 - 3, slot);
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
        hw_text_in(u->scr, nm, 13, ry, u->w - 26 - 24, 18, HF_10, HC_WHITE, LV_TEXT_ALIGN_LEFT);
        hw_text_in(u->scr, bl, u->w - 13 - 24, ry, 24, 18, HF_10, HC_LABEL, LV_TEXT_ALIGN_RIGHT);
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
        const int ch = H - 46, ih = ch - 14;
        const int gap = (ih - 16 - 17 - 13) / 2;
        for (int k = 0; k < 2; ++k) {
            lv_obj_t *c = hw_card_box(p, k * 208, 46, 200, ch, HC_CARD, 10, HC_LINE);
            hw_section(c, 13, 7, 174, k ? "INHIBIT REGISTER" : "FAULT REGISTER", unk ? "\xE2\x80\x94" : "0x0000", HC_LABEL);
            hw_text_in(c, unk ? "Unreadable" : "0 active bits", 13, 7 + 16 + gap, 174, 17, HF_14B,
                       unk ? HC_DIM : HC_OK_COL, LV_TEXT_ALIGN_LEFT);
            hw_text_in(c, unk ? "Link offline" : (k ? "No inhibit bits set" : "No fault bits set"),
                       13, ch - 7 - 13, 174, 13, HF_10, HC_LABEL, LV_TEXT_ALIGN_LEFT);
        }
        return;
    }
    int y = 46;
    if (cls == HP_FB_PRIMARY_LISTS) {
        fl.pri = hw_card_box(p, 0, y, 408, 44, HC_WARN_BG, 10, HC_WARN_COL);
        fl.pri_nm = hw_text_in(fl.pri, "", 13, 6, 300, 18, HF_12B, HC_WARN_COL, LV_TEXT_ALIGN_LEFT);
        fl.pri_id = hw_text_in(fl.pri, "", 320, 6, 75, 18, HF_12, HC_LABEL, LV_TEXT_ALIGN_RIGHT);
        fl.pri_act = hw_text_in(fl.pri, "", 13, 23, 382, 14, HF_10, HC_TXT2, LV_TEXT_ALIGN_LEFT);
        lv_subject_add_observer_obj(hm_fault_pri, apply_pri, fl.pri, NULL);   /* dies with the card on a body rebuild */
        y += 50;
    }
    mask_list(&fl.list[0], 0, p, 0, y, 200, H - y, "FAULTS");
    mask_list(&fl.list[1], 1, p, 208, y, 200, H - y, "INHIBITS");
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
    fl.sum = hw_card_box(p, 0, 0, 408, 40, HC_CARD, 10, HC_LINE);
    fl.icon = hw_icon(fl.sum, &hmi_icon_checkc_18, 13, 0, HC_OK_COL);
    fl.title = hw_text_in(fl.sum, "", 41, 0, 250, 40, HF_14B, HC_WHITE, LV_TEXT_ALIGN_LEFT);
    fl.read = hw_text_in(fl.sum, "", 290, 0, 105, 40, HF_10, HC_LABEL, LV_TEXT_ALIGN_RIGHT);
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
