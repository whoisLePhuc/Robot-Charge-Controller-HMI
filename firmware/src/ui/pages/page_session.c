/*
 * Session page: session summary cards and the controller event ring buffer list.
 */
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "hmi_icons.h"
#include "hmi_nav.h"
#include "hmi_pages.h"
#include "hmi_present.h"
#include "hmi_theme_present.h"
#include "hmi_widgets.h"

enum { SLOT_EVENTS };   /* scroll slot of the event list */

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

static void sync_click(lv_event_t *e) { (void)e; hmi_nav_request_redraw(); }   /* re-read the snapshot now */

void page_session_build(lv_obj_t *p, const rcc_snapshot_t *m, const hmi_page_ctx_t *ctx)
{
    const int H = ctx->height;
    hp_state_info_t s; hp_state_info(m, &s);

    const bool in_session = hp_in_session(m);

    static const char *const k[4] = {"SESSION", "DURATION", "LAST CMD", "END REASON"};
    char v[4][40];
    if (hp_fresh(m) && m->session_id) snprintf(v[0], sizeof(v[0]), "#%" PRIu32 " \xC2\xB7 %s", m->session_id, m->simulated_measurements ? "SIMULATED V/I" : s.name);
    else snprintf(v[0], sizeof(v[0]), "%s", s.name);
    if (m->end_valid && !in_session && (!m->session_id || m->end_session_id == m->session_id)) hp_fmt_duration(v[1], sizeof(v[1]), m->end_duration_ms);
    else if (in_session) snprintf(v[1], sizeof(v[1]), "In progress");
    else snprintf(v[1], sizeof(v[1]), "\xE2\x80\x94");
    hp_command_text(v[2], sizeof(v[2]), m);
    snprintf(v[3], sizeof(v[3]), "%s", m->end_valid ? (m->end_reason < 10 ? hp_end_names[m->end_reason] : "?") : "\xE2\x80\x94");
    for (int i = 0; i < 4; ++i) {
        const int bx = i * 1035 / 10;       /* 4 columns, 6 px gaps */
        lv_obj_t *c = hw_card_box(p, bx, 0, 98, 39, HC_CARD, 8, HC_LINE);
        hw_text_in(c, k[i], 9, 5, 80, 13, HF_10, HC_LABEL, LV_TEXT_ALIGN_LEFT);
        hw_text_in(c, v[i], 9, 18, 80, 16, HF_12B, HC_WHITE, LV_TEXT_ALIGN_LEFT);
    }

    const int ev_h = H - 45;
    lv_obj_t *ev = hw_card_box(p, 0, 45, 408, ev_h, HC_CARD, 10, HC_LINE);
    char head[64];
    if (m->online && m->device_valid) {
        if (m->event_missed) snprintf(head, sizeof(head), "EVENT RING BUFFER (%u held \xC2\xB7 %" PRIu32 " rotated)", (unsigned)m->event_count, m->event_missed);
        else snprintf(head, sizeof(head), "EVENT RING BUFFER (%u held)", (unsigned)m->event_count);
    } else {
        snprintf(head, sizeof(head), "EVENT RING BUFFER (offline)");
    }
    lv_obj_t *hl = hw_text(ev, head, 11, 6 + (22 - hw_lh(HF_10)) / 2, HF_10, HC_LABEL);
    hw_spacing(hl, 1);

    lv_obj_t *sy = hw_button(ev, 330, 6, 67, 22, HC_NAV_BOX, 8, sync_click, NULL);
    lv_obj_set_style_border_width(sy, 1, 0);
    lv_obj_set_style_border_color(sy, HC(HC_NAV_EDGE), 0);
    lv_obj_t *sl = hw_text(sy, "SYNC", 0, 0, HF_10, HC_NAV_CUR);
    hw_spacing(sl, 1);
    lv_obj_set_clickable(sl, false);
    const int sw = hw_width_of(sl), tot = 11 + 6 + sw;
    hw_icon_row(sy, &hmi_icon_refresh_11, (65 - tot) / 2, 0, 20, HC_NAV_CUR);
    lv_obj_set_pos(sl, (65 - tot) / 2 + 17, (20 - hw_lh(HF_10)) / 2);

    if (!m->online) {
        hw_icon_row(ev, &hmi_icon_warn_16, 90, 34, ev_h - 34, HC_WARN_COL);
        hw_text_in(ev, "Event log unavailable \xE2\x80\x94 UART link lost", 112, 34, 290, ev_h - 34, HF_12B, HC_WARN_COL, LV_TEXT_ALIGN_LEFT);
        return;
    }
    if (!m->event_count) {
        hw_text_in(ev, "No events held by the controller", 11, 34, 386, 20, HF_12, HC_LABEL, LV_TEXT_ALIGN_LEFT);
        return;
    }
    lv_obj_t *a = hw_scroll_area(ev, 0, 34, 408, ev_h - 34 - 3, SLOT_EVENTS);
    for (int i = 0; i < m->event_count; ++i) {
        const int n = (int)m->event_count - 1 - i, ry = i * 20;
        char det[64], since[16];
        hp_fmt_since_boot(since, sizeof(since), m->event_us[n]);
        hw_text_in(a, since, 11, ry, 44, 20, HF_10, HC_LABEL, LV_TEXT_ALIGN_LEFT);
        hw_box(a, 57, ry + 7, 6, 6, ht_kind_color(hp_event_kind(m->event_codes[n])), 3);
        lv_obj_t *nl = hw_text(a, hp_event_name(m->event_codes[n]), 70, 0, HF_12B, HC_WHITE);
        const int nw = hw_width_of(nl);
        lv_obj_set_pos(nl, 70, ry + (20 - hw_lh(HF_12B)) / 2);
        hp_event_detail(det, sizeof(det), m->event_codes[n], m->event_args[n]);
        detail_right(a, 397, ry, 20, det, 70 + nw + 8);
        if (i + 1 < m->event_count) hw_hline(a, 11, ry + 19, 386, HC_LINE);
    }
    hw_scroll_restore(a, SLOT_EVENTS);
}
