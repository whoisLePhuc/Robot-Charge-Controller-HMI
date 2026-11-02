/*
 * Session page (retained): session summary cards (hm_sess) and the controller event ring buffer list. The event rows are
 * rebuilt only when hp_events_sig() changes; the scroll box and its offset persist (a re-enter restores it from the slot).
 */
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "hmi_icons.h"
#include "hmi_model.h"
#include "hmi_nav.h"
#include "hmi_pages.h"
#include "hmi_present.h"
#include "hmi_theme_present.h"
#include "hmi_widgets.h"

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
        lv_obj_t *rl = hw_text(p, r, 0, 0, HF_10, HC_TXT2);
        const int wr = hw_width_of(rl);
        lv_obj_t *ll = hw_text(p, l, 0, 0, HF_10, HC_TXT2);
        const int wl = hw_width_of(ll);
        const int xr = right - wr, xc = xr - 4 - 10, xl = xc - 4 - wl;
        if (xl >= min_x) {
            lv_obj_set_pos(rl, xr, y + (h - hw_lh(HF_10)) / 2);
            lv_obj_set_pos(ll, xl, y + (h - hw_lh(HF_10)) / 2);
            hw_icon_row(p, &hmi_icon_chev_10, xc, y, h, HC_LABEL);
            return;
        }
        lv_obj_delete(ll);
        lv_obj_delete(rl);
        hw_text_in(p, r, min_x, y, right - min_x, h, HF_10, HC_TXT2, LV_TEXT_ALIGN_RIGHT);
        return;
    }
    hw_text_in(p, s, min_x, y, right - min_x, h, HF_10, HC_TXT2, LV_TEXT_ALIGN_RIGHT);
}

enum { SLOT_EVENTS };   /* scroll slot of the event list: keeps the offset when the shell re-enters the page (banner moved) */

static void sync_click(lv_event_t *e) { (void)e; hmi_nav_request_redraw(); }   /* re-read the snapshot now */

static struct {
    lv_obj_t *val[4], *head;
    lv_obj_t *offl_ic, *offl_tx, *empty, *scr;   /* the event region: offline message, "no events" text, row scroll box */
    uint64_t sig;
    bool sig_ok;                                 /* rows reflect `sig` */
} ss;

static void apply_sess(lv_observer_t *o, lv_subject_t *s)
{
    (void)o;
    const hp_sess_view_t *v = lv_subject_get_pointer(s);
    for (int i = 0; i < 4; ++i) hw_label_set(ss.val[i], v->v[i]);
    hw_label_set(ss.head, v->head);
}

/* Show the region state for `m` and refill the rows when their signature changed (the scroll offset is kept). */
static void sync_events(const rcc_snapshot_t *m)
{
    const uint64_t sig = hp_events_sig(m);
    if (ss.sig_ok && sig == ss.sig) return;
    const hp_ev_region_t rg = hp_events_region(m);
    const bool list = rg == HP_EVR_LIST;
    lv_obj_set_hidden(ss.offl_ic, rg != HP_EVR_OFFLINE);
    lv_obj_set_hidden(ss.offl_tx, rg != HP_EVR_OFFLINE);
    lv_obj_set_hidden(ss.empty, rg != HP_EVR_EMPTY);
    lv_obj_set_hidden(ss.scr, !list);
    const int keep = list && ss.sig_ok ? (int)lv_obj_get_scroll_y(ss.scr) : -1;   /* -1: fresh box, take the slot */
    lv_obj_clean(ss.scr);
    if (list) {
        for (int i = 0; i < m->event_count; ++i) {
            const int n = (int)m->event_count - 1 - i, ry = i * 20;
            char det[64], since[16];
            hp_fmt_since_boot(since, sizeof(since), m->event_us[n]);
            hw_text_in(ss.scr, since, 11, ry, 44, 20, HF_10, HC_LABEL, LV_TEXT_ALIGN_LEFT);
            hw_box(ss.scr, 57, ry + 7, 6, 6, ht_kind_color(hp_event_kind(m->event_codes[n])), 3);
            lv_obj_t *nl = hw_text(ss.scr, hp_event_name(m->event_codes[n]), 70, 0, HF_12B, HC_WHITE);
            const int nw = hw_width_of(nl);
            lv_obj_set_pos(nl, 70, ry + (20 - hw_lh(HF_12B)) / 2);
            hp_event_detail(det, sizeof(det), m->event_codes[n], m->event_args[n]);
            detail_right(ss.scr, 397, ry, 20, det, 70 + nw + 8);
            if (i + 1 < m->event_count) hw_hline(ss.scr, 11, ry + 19, 386, HC_LINE);
        }
        if (keep >= 0) hw_scroll_to(ss.scr, keep);
        else hw_scroll_restore(ss.scr, SLOT_EVENTS);
    }
    ss.sig = sig; ss.sig_ok = true;
}

static void enter(lv_obj_t *p, const rcc_snapshot_t *m, const hmi_page_ctx_t *ctx)
{
    memset(&ss, 0, sizeof(ss));
    const int H = ctx->height;
    static const char *const k[4] = {"SESSION", "DURATION", "LAST CMD", "END REASON"};
    for (int i = 0; i < 4; ++i) {
        const int bx = i * 1035 / 10;       /* 4 columns, 6 px gaps */
        lv_obj_t *c = hw_card_box(p, bx, 0, 98, 39, HC_CARD, 8, HC_LINE);
        hw_text_in(c, k[i], 9, 5, 80, 13, HF_10, HC_LABEL, LV_TEXT_ALIGN_LEFT);
        ss.val[i] = hw_text_in(c, "", 9, 18, 80, 16, HF_12B, HC_WHITE, LV_TEXT_ALIGN_LEFT);
    }

    const int ev_h = H - 45;
    lv_obj_t *ev = hw_card_box(p, 0, 45, 408, ev_h, HC_CARD, 10, HC_LINE);
    ss.head = hw_text(ev, "", 11, 6 + (22 - hw_lh(HF_10)) / 2, HF_10, HC_LABEL);
    hw_spacing(ss.head, 1);

    lv_obj_t *sy = hw_button(ev, 330, 6, 67, 22, HC_NAV_BOX, 8, sync_click, NULL);
    lv_obj_set_style_border_width(sy, 1, 0);
    lv_obj_set_style_border_color(sy, HC(HC_NAV_EDGE), 0);
    lv_obj_t *sl = hw_text(sy, "SYNC", 0, 0, HF_10, HC_NAV_CUR);
    hw_spacing(sl, 1);
    lv_obj_set_clickable(sl, false);
    const int sw = hw_width_of(sl), tot = 11 + 6 + sw;
    hw_icon_row(sy, &hmi_icon_refresh_11, (65 - tot) / 2, 0, 20, HC_NAV_CUR);
    lv_obj_set_pos(sl, (65 - tot) / 2 + 17, (20 - hw_lh(HF_10)) / 2);

    ss.offl_ic = hw_icon_row(ev, &hmi_icon_warn_16, 90, 34, ev_h - 34, HC_WARN_COL);
    ss.offl_tx = hw_text_in(ev, "Event log unavailable \xE2\x80\x94 UART link lost", 112, 34, 290, ev_h - 34, HF_12B, HC_WARN_COL, LV_TEXT_ALIGN_LEFT);
    ss.empty = hw_text_in(ev, "No events held by the controller", 11, 34, 386, 20, HF_12, HC_LABEL, LV_TEXT_ALIGN_LEFT);
    ss.scr = hw_scroll_area(ev, 0, 34, 408, ev_h - 34 - 3, SLOT_EVENTS);
    lv_subject_add_observer_obj(hm_sess, apply_sess, ev, NULL);   /* ev and the value labels all die with the page root */
    sync_events(m);
}

static void update(const rcc_snapshot_t *m, const hmi_page_ctx_t *ctx)
{
    (void)ctx;
    sync_events(m);
}

static void leave(void) { memset(&ss, 0, sizeof(ss)); }

const hmi_page_ops_t page_session_ops = {.enter = enter, .update = update, .leave = leave};
