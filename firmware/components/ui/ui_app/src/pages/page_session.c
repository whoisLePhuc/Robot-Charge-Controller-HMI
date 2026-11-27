/*
 * Session page (retained): session summary cards (hm_sess) and the controller event ring buffer list. The event rows are
 * rebuilt only when hp_events_sig() changes; the scroll box and its offset persist (a re-enter restores it from the slot).
 */
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "hmi_data_widgets.h"
#include "hmi_icons.h"
#include "hmi_model.h"
#include "hmi_nav.h"
#include "hmi_pages.h"
#include "hmi_present.h"
#include "hmi_theme_present.h"
#include "hmi_widgets.h"

#include "hmi_lvgl_headers.h"
#include "rcc_session_detail_gen.h"
#include "rcc_session_detail_icon_gen.h"
#include "rcc_session_event_row_gen.h"
#include "rcc_session_events_gen.h"
#include "rcc_session_summary_gen.h"

/* Right-aligned event detail; "A -> B" is drawn with a chevron between the parts. */
static void detail_right(lv_obj_t *p, int right, const char *s, int min_x)
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
        lv_obj_t *rl = rcc_session_detail_create(p, 0, 3, LV_SIZE_CONTENT, r, HMI_DATA_FONT(HF_10));
        const int wr = hw_width_of(rl);
        lv_obj_t *ll = rcc_session_detail_create(p, 0, 3, LV_SIZE_CONTENT, l, HMI_DATA_FONT(HF_10));
        const int wl = hw_width_of(ll);
        const int xr = right - wr, xc = xr - 4 - 10, xl = xc - 4 - wl;
        if (xl >= min_x) {
            lv_obj_set_x(rl, xr);
            lv_obj_set_x(ll, xl);
            rcc_session_detail_icon_create(p, xc, 5, &hmi_icon_chev_10);
            return;
        }
        lv_obj_delete(ll);
        lv_obj_delete(rl);
        rcc_session_detail_create(p, min_x, 3, right - min_x, r, HMI_DATA_FONT(HF_10));
        return;
    }
    rcc_session_detail_create(p, min_x, 3, right - min_x, s, HMI_DATA_FONT(HF_10));
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
            const bool divider = i + 1 < m->event_count;
            lv_obj_t *row = rcc_session_event_row_create(ss.scr, ry, divider ? 20 : 16, since,
                                                         hp_event_name(m->event_codes[n]),
                                                         HC(ht_kind_color(hp_event_kind(m->event_codes[n]))),
                                                         HMI_DATA_FONT(HF_10), HMI_DATA_FONT(HF_12B), divider);
            lv_obj_t *name = hmi_data_widget(row, "event_name");
            const int min_x = lv_obj_get_x_aligned(name) + hw_width_of(name) + 8;
            hp_event_detail(det, sizeof(det), m->event_codes[n], m->event_args[n]);
            detail_right(row, 397, det, min_x);
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
        lv_obj_t *card = rcc_session_summary_create(p, bx, k[i], "", HMI_DATA_FONT(HF_10), HMI_DATA_FONT(HF_12B));
        ss.val[i] = hmi_data_widget(card, "summary_value");
    }

    lv_obj_t *ev = rcc_session_events_create(p, H - 45, HMI_DATA_FONT(HF_10), HMI_DATA_FONT(HF_12B),
                                             HMI_DATA_FONT(HF_12), &hmi_icon_refresh_11, &hmi_icon_warn_16, 8, 25);
    ss.head = hmi_data_widget(ev, "event_heading");
    lv_obj_t *sync = hmi_data_widget(ev, "sync");
    lv_obj_add_event_cb(sync, sync_click, LV_EVENT_CLICKED, NULL);
    ss.offl_ic = hmi_data_widget(ev, "event_offline_icon");
    ss.offl_tx = hmi_data_widget(ev, "event_offline_text");
    ss.empty = hmi_data_widget(ev, "event_empty");

    lv_obj_t *rows = hmi_data_widget(ev, "event_rows");
    lv_obj_update_layout(rows);
    ss.scr = hw_scroll_area(rows, 0, 0, lv_obj_get_width(rows), lv_obj_get_height(rows), SLOT_EVENTS);
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
