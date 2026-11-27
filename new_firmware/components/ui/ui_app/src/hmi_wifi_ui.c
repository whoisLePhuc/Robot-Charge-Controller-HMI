/*
 * Wi-Fi flow and on-screen keyboard: full-screen overlay opened from Device > Settings > Wi-Fi (hand-written C).
 * Talks to hmi_wifi / hmi_ota only; the shell is reached through its overlay service (ui/hmi_shell.h).
 */
#include "hmi_wifi_ui.h"

#include <stdio.h>
#include <string.h>

#include "hmi_icons.h"
#include "hmi_ota.h"
#include "hmi_wifi.h"
#include "hmi_shell.h"
#include "hmi_theme.h"
#include "hmi_widgets.h"

static lv_obj_t *wifi_layer;
static hmi_wifi_status_t ws;

/* ------------------------------------------------------------------ Wi-Fi screens
 * Full-screen overlay (no nav rail) opened from Device > Settings > Wi-Fi. Views: network list,
 * password entry (on-screen keyboard), connecting, connected info, failure. The overlay polls
 * hmi_wifi_get() every 250 ms and re-renders only when the backend version or the view changes. */
typedef enum { WV_LIST, WV_PASS, WV_BUSY, WV_INFO, WV_FAIL } wifi_view_t;
enum { KEY_SHIFT = 256, KEY_BKSP, KEY_SYM, KEY_GO, KEY_NONE };
enum { SLOT_LIST };   /* scroll slot of the network list */

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
    if (wifi_layer) { hmi_shell_overlay_close(); wifi_layer = NULL; }
    memset(wifi_pw, 0, sizeof(wifi_pw)); wifi_pw_len = 0;
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
static void wifi_ota_cb(lv_event_t *e)
{
    (void)e;
    hmi_ota_status_t ota;
    hmi_ota_get_status(&ota);
    if (ota.armed) hmi_ota_disarm(); else hmi_ota_arm();
    wifi_redraw();
}
/* Re-render on Wi-Fi changes and OTA arm/disarm/expiry, not on upload progress. */
static uint32_t wifi_signature(void)
{
    hmi_ota_status_t ota;
    hmi_ota_get_status(&ota);
    return ws.version * 16u + (uint32_t)wifi_view * 4u + (ws.state == HMI_WIFI_CONNECTING ? ws.step : 0u) +
           ws.state * 1000003u + ota.arm_seq * 7919u;
}

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
    lv_obj_t *b = hw_button(l, 8, 8, 34, 30, HC_SURFACE2, 8, wifi_back_cb, NULL);
    lv_obj_set_style_border_width(b, 1, 0);
    lv_obj_set_style_border_color(b, HC(HC_LINE), 0);
    lv_obj_set_style_pad_all(b, -1, 0);
    hw_icon_c(b, &hmi_icon_back_14, 17, 15, HC_TXT2);
    hw_text_in(l, title, 52, 8, 330, 17, HF_14B, HC_WHITE, LV_TEXT_ALIGN_LEFT);
    hw_text_in(l, sub, 52, 25, 330, 13, HF_10, HC_LABEL, LV_TEXT_ALIGN_LEFT);
}
static void wifi_bars(lv_obj_t *p, int x, int bottom, int level)
{
    static const int hh[4] = {4, 7, 10, 13};
    const uint32_t col = level >= 3 ? HC_OK_COL : level == 2 ? HC_WARN_COL : HC_AMP_COL;
    for (int i = 0; i < 4; ++i) hw_box(p, x + i * 5, bottom - hh[i], 3, hh[i], i < level ? col : HC_TRACK, 1);
}
static lv_obj_t *wifi_btn(lv_obj_t *l, int x, int y, int w, const char *s, uint32_t bg, uint32_t fg, bool border, lv_event_cb_t cb)
{
    lv_obj_t *b = hw_button(l, x, y, w, 40, bg, 10, cb, NULL);
    if (border) {
        lv_obj_set_style_border_width(b, 1, 0);
        lv_obj_set_style_border_color(b, HC(HC_LINE), 0);
        lv_obj_set_style_pad_all(b, -1, 0);
    }
    lv_obj_t *t = hw_text_in(b, s, 0, 0, w, 40, HF_12B, fg, LV_TEXT_ALIGN_CENTER);
    hw_spacing(t, 1);
    return b;
}
static void wifi_cell(lv_obj_t *l, int x, int y, const char *k, const char *v)
{
    lv_obj_t *c = hw_card_box(l, x, y, 229, 44, HC_CARD, 10, HC_LINE);
    lv_obj_t *kl = hw_text(c, k, 12, 7, HF_10, HC_LABEL);
    hw_spacing(kl, 1);
    hw_text_in(c, v, 12, 22, 205, 17, HF_12B, HC_WHITE, LV_TEXT_ALIGN_LEFT);
}
static void wifi_spinner(lv_obj_t *p, const lv_image_dsc_t *d, int cx, int cy, uint32_t col)
{
    lv_obj_t *ic = hw_icon_c(p, d, cx, cy, col);
    lv_image_set_pivot(ic, d->header.w / 2, d->header.h / 2);
    lv_anim_t an; lv_anim_init(&an);
    lv_anim_set_var(&an, ic); lv_anim_set_exec_cb(&an, hw_spin_anim);
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

    lv_obj_t *sc = hw_button(l, 400, 10, 72, 26, HC_NAV_BOX, 8, wifi_scan_cb, NULL);
    lv_obj_set_style_border_width(sc, 1, 0);
    lv_obj_set_style_border_color(sc, HC(HC_NAV_EDGE), 0);
    lv_obj_set_style_pad_all(sc, -1, 0);
    lv_obj_t *sl = hw_text(sc, "SCAN", 0, 0, HF_10, HC_NAV_CUR);
    hw_spacing(sl, 1);
    const int sw = hw_width_of(sl), tot = 12 + 6 + sw;
    hw_icon_row(sc, &hmi_icon_refresh_11, (72 - tot) / 2, 0, 26, HC_NAV_CUR);
    lv_obj_set_pos(sl, (72 - tot) / 2 + 17, (26 - hw_lh(HF_10)) / 2);

    const bool conn = ws.state == HMI_WIFI_CONNECTED, un = ws.state == HMI_WIFI_UNAVAILABLE;
    lv_obj_t *st = hw_card_box(l, 8, 46, 464, 26, conn ? HC_OK_BG : un ? HC_WARN_BG : HC_CARD, 8, conn ? HC_OK_COL : un ? HC_WARN_COL : HC_LINE);
    hw_box(st, 10, 10, 6, 6, conn ? HC_OK_COL : un ? HC_WARN_COL : ws.state == HMI_WIFI_SCANNING ? HC_NAV_CUR : HC_DIM, 3);
    if (conn) {
        lv_obj_t *a = hw_text(st, "Connected to", 24, 0, HF_10, HC_TXT2);
        const int aw = hw_width_of(a);
        lv_obj_set_y(a, (26 - hw_lh(HF_10)) / 2);
        hw_text_in(st, ws.ssid, 24 + aw + 5, 0, 200, 26, HF_10, HC_WHITE, LV_TEXT_ALIGN_LEFT);
        lv_obj_t *db = hw_button(st, 360, 0, 104, 26, HC_CARD, 8, wifi_disc_cb, NULL);
        lv_obj_set_style_bg_opa(db, LV_OPA_TRANSP, 0);
        lv_obj_t *dl = hw_text_in(db, "DISCONNECT", 0, 0, 94, 26, HF_10, HC_LABEL, LV_TEXT_ALIGN_RIGHT);
        hw_spacing(dl, 1);
    } else {
        hw_text_in(st, un ? "Wi-Fi driver is not available in this build" : ws.state == HMI_WIFI_SCANNING ? "Scanning for networks..." : "Not connected",
                   24, 0, 420, 26, HF_10, un ? HC_WARN_COL : HC_LABEL, LV_TEXT_ALIGN_LEFT);
    }

    if (!ws.ap_count) {
        hw_text_in(l, un ? "Wi-Fi is not available on this controller" : "No networks found - tap SCAN", 0, 150, 480, 17, HF_12, HC_LABEL, LV_TEXT_ALIGN_CENTER);
        return;
    }
    lv_obj_t *a = hw_scroll_area(l, 8, 78, 464, 214, SLOT_LIST);
    for (int i = 0; i < ws.ap_count; ++i) {
        const hmi_wifi_ap_t *ap = &ws.ap[i];
        const bool cur = conn && !strcmp(ap->ssid, ws.ssid);
        lv_obj_t *b = hw_button(a, 0, i * 44, 464, 40, cur ? HC_NAV_BOX : HC_CARD, 10, wifi_row_cb, (void *)(uintptr_t)i);
        lv_obj_set_style_border_width(b, 1, 0);
        lv_obj_set_style_border_color(b, HC(cur ? HC_NAV_EDGE : HC_LINE), 0);
        lv_obj_set_style_pad_all(b, -1, 0);
        hw_text_in(b, ap->ssid, 12, 6, ap->secure ? 330 : 345, 15, HF_12B, HC_WHITE, LV_TEXT_ALIGN_LEFT);
        char s2[40];
        if (cur) snprintf(s2, sizeof(s2), "Connected \xC2\xB7 %s", ws.ip);
        else snprintf(s2, sizeof(s2), "%s", ap->secure ? "Secured" : "Open network");
        hw_text_in(b, s2, 12, 21, 330, 13, HF_10, cur ? HC_TXT2 : HC_LABEL, LV_TEXT_ALIGN_LEFT);
        wifi_bars(b, 434, 27, rssi_level(ap->rssi));
        char rs[16]; snprintf(rs, sizeof(rs), "%d dBm", (int)ap->rssi);
        hw_text_in(b, rs, 374, 0, 50, 40, HF_10, HC_LABEL, LV_TEXT_ALIGN_RIGHT);
        if (ap->secure) hw_icon_row(b, &hmi_icon_lock_11, 353, 0, 40, HC_DIM);
    }
    hw_scroll_restore(a, SLOT_LIST);
    hw_text_in(l, "Drag to scroll \xC2\xB7 tap a network to connect", 0, 299, 480, 13, HF_10, HC_DIM, LV_TEXT_ALIGN_CENTER);
}

/* ---- password view */
static void wifi_go_style(void)
{
    if (!wifi_go_key) return;
    const bool ok = !wifi_target_secure || wifi_pw_len >= HMI_WIFI_PASS_MIN;
    lv_obj_set_style_bg_color(wifi_go_key, HC(ok ? HC_NAV_CUR : HC_SURFACE2), 0);
    lv_obj_set_style_border_color(wifi_go_key, HC(ok ? HC_NAV_CUR : HC_LINE), 0);
    lv_obj_t *lab = lv_obj_get_child(wifi_go_key, 0);
    if (lab) lv_obj_set_style_text_color(lab, HC(ok ? HC_BG : HC_DIM), 0);
}
static void wifi_field_draw(void)
{
    if (!wifi_field) return;
    lv_obj_clean(wifi_field);
    const int maxw = 400;
    int cx = 12;
    if (!wifi_pw_len) {
        hw_text_in(wifi_field, "Enter password", 12, 0, 300, 34, HF_12, HC_DIM, LV_TEXT_ALIGN_LEFT);
    } else if (!wifi_show_pw) {
        const int vis = wifi_pw_len > 34 ? 34 : wifi_pw_len;
        for (int i = 0; i < vis; ++i) hw_box(wifi_field, 12 + i * 11, 14, 6, 6, HC_WHITE, 3);
        cx = 12 + vis * 11;
    } else {
        const char *s = wifi_pw;
        lv_obj_t *t = hw_text(wifi_field, s, 12, 0, HF_14B, HC_WHITE);
        while (*s && hw_width_of(t) > maxw) { ++s; lv_label_set_text(t, s); }
        lv_obj_set_y(t, (34 - hw_lh(HF_14B)) / 2);
        cx = 12 + hw_width_of(t) + 2;
    }
    hw_box(wifi_field, cx, 9, 2, 16, HC_NAV_CUR, 1);
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

/* ~50 keyboard keys share these (no per-object style allocation) to stay inside LVGL's 64 KB heap. */
typedef struct {
    lv_style_t key, key_pr, key_lbl;          /* single keys */
    lv_style_t mx_main, mx_item, mx_item_pr;  /* button-matrix rows */
} kb_styles_t;
static kb_styles_t kb_st;
static bool kb_st_ready;

static kb_styles_t *kb_styles(void)
{
    if (kb_st_ready) return &kb_st;
    kb_st_ready = true;
    lv_style_init(&kb_st.key);
    lv_style_set_bg_color(&kb_st.key, HC(HC_SURFACE2)); lv_style_set_bg_opa(&kb_st.key, LV_OPA_COVER);
    lv_style_set_radius(&kb_st.key, 6);
    lv_style_set_border_width(&kb_st.key, 1); lv_style_set_border_color(&kb_st.key, HC(HC_LINE));
    lv_style_set_pad_all(&kb_st.key, -1);
    lv_style_init(&kb_st.key_pr);
    lv_style_set_bg_opa(&kb_st.key_pr, LV_OPA_70);
    lv_style_init(&kb_st.key_lbl);
    lv_style_set_text_font(&kb_st.key_lbl, HF_12B); lv_style_set_text_color(&kb_st.key_lbl, HC(HC_TXT2));
    lv_style_set_text_align(&kb_st.key_lbl, LV_TEXT_ALIGN_CENTER);

    lv_style_init(&kb_st.mx_main);
    lv_style_set_bg_opa(&kb_st.mx_main, LV_OPA_TRANSP); lv_style_set_border_width(&kb_st.mx_main, 0);
    lv_style_set_pad_all(&kb_st.mx_main, 0); lv_style_set_pad_gap(&kb_st.mx_main, 3); lv_style_set_radius(&kb_st.mx_main, 0);
    lv_style_init(&kb_st.mx_item);
    lv_style_set_bg_color(&kb_st.mx_item, HC(HC_SURFACE2)); lv_style_set_bg_opa(&kb_st.mx_item, LV_OPA_COVER);
    lv_style_set_radius(&kb_st.mx_item, 6);
    lv_style_set_border_width(&kb_st.mx_item, 1); lv_style_set_border_color(&kb_st.mx_item, HC(HC_LINE));
    lv_style_set_text_font(&kb_st.mx_item, HF_12B); lv_style_set_text_color(&kb_st.mx_item, HC(HC_TXT2));
    lv_style_init(&kb_st.mx_item_pr);
    lv_style_set_bg_opa(&kb_st.mx_item_pr, LV_OPA_70);
    return &kb_st;
}

typedef struct { int code; int w; } kkey_t;
static void kb_btn(lv_obj_t *l, int x, int y, int w, int code)
{
    const bool go = code == KEY_GO, shift = code == KEY_SHIFT, space = code == ' ';
    const bool shift_on = shift && wifi_shift;
    /* ~50 keys: share styles (no per-object style allocation) to stay inside LVGL's 64 KB heap */
    kb_styles_t *st = kb_styles();
    lv_obj_t *b = lv_button_create(l);
    lv_obj_remove_style_all(b);
    lv_obj_set_pos(b, x, y);
    lv_obj_set_size(b, w, 38);
    lv_obj_set_scrollable(b, false);
    lv_obj_add_style(b, &st->key, 0);
    lv_obj_add_style(b, &st->key_pr, LV_STATE_PRESSED);
    lv_obj_add_event_cb(b, wifi_key_cb, LV_EVENT_CLICKED, (void *)(uintptr_t)code);
    if (go) { lv_obj_set_style_bg_color(b, HC(HC_NAV_CUR), 0); lv_obj_set_style_border_color(b, HC(HC_NAV_CUR), 0); }
    if (shift_on) { lv_obj_set_style_bg_color(b, HC(HC_NAV_BOX), 0); lv_obj_set_style_border_color(b, HC(HC_NAV_CUR), 0); }
    if (code == KEY_SHIFT) hw_icon_c(b, &hmi_icon_shift_14, w / 2, 19, wifi_sym ? HC_TRACK : shift_on ? HC_NAV_CUR : HC_LABEL);
    else if (code == KEY_BKSP) hw_icon_c(b, &hmi_icon_bksp_16, w / 2, 19, HC_LABEL);
    else {
        char s[12];
        bool small = false; uint32_t fg = HC_TXT2;
        if (code == KEY_SYM) { snprintf(s, sizeof(s), "%s", wifi_sym ? "ABC" : "?123"); small = true; fg = HC_LABEL; }
        else if (go) { snprintf(s, sizeof(s), "Connect"); fg = HC_BG; }
        else if (space) { snprintf(s, sizeof(s), "space"); small = true; fg = HC_DIM; }
        else {
            int ch = code;
            if (wifi_shift && ch >= 'a' && ch <= 'z') ch -= 32;
            snprintf(s, sizeof(s), "%c", ch);
        }
        lv_obj_t *t = lv_label_create(b);
        lv_label_set_text(t, s);
        lv_obj_add_style(t, &st->key_lbl, 0);
        if (small) lv_obj_set_style_text_font(t, HF_10, 0);
        if (fg != HC_TXT2) lv_obj_set_style_text_color(t, HC(fg), 0);
        lv_obj_set_size(t, w, hw_lh(small ? HF_10 : HF_12B));
        lv_obj_set_pos(t, 0, (38 - hw_lh(small ? HF_10 : HF_12B)) / 2);
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
    kb_styles_t *st = kb_styles();
    const int n = (int)strlen(chars);
    for (int i = 0; i < n; ++i) {
        kb_txt[row][i][0] = (wifi_shift && chars[i] >= 'a' && chars[i] <= 'z') ? (char)(chars[i] - 32) : chars[i];
        kb_txt[row][i][1] = '\0';
        kb_map[row][i] = kb_txt[row][i];
    }
    kb_map[row][n] = "";
    lv_obj_t *m = lv_buttonmatrix_create(l);
    lv_obj_remove_style_all(m);
    lv_obj_add_style(m, &st->mx_main, 0);
    lv_obj_add_style(m, &st->mx_item, LV_PART_ITEMS);
    lv_obj_add_style(m, &st->mx_item_pr, LV_PART_ITEMS | LV_STATE_PRESSED);
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
    lv_obj_t *pl = hw_text(l, "PASSWORD", 8, 46, HF_10, HC_LABEL);
    hw_spacing(pl, 1);
    char c[16]; snprintf(c, sizeof(c), "%d / %d", wifi_pw_len, HMI_WIFI_PASS_MAX);
    wifi_count_lbl = hw_text_in(l, c, 372, 46, 100, 13, HF_10, HC_DIM, LV_TEXT_ALIGN_RIGHT);
    lv_obj_t *f = hw_card_box(l, 8, 62, 464, 34, HC_CARD, 10, HC_NAV_CUR);
    wifi_field = hw_obj(f, 0, 0, 440, 34);
    lv_obj_t *ey = hw_button(f, 420, 0, 44, 34, HC_CARD, 10, wifi_eye_cb, NULL);
    lv_obj_set_style_bg_opa(ey, LV_OPA_TRANSP, 0);
    hw_icon_c(ey, wifi_show_pw ? &hmi_icon_eyeoff_14 : &hmi_icon_eye_14, 22, 17, wifi_show_pw ? HC_NAV_CUR : HC_LABEL);

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
    wifi_spinner(l, &hmi_icon_spin_44, 240, 72, HC_NAV_CUR);
    hw_text_in(l, "Joining network...", 0, 104, 480, 17, HF_14B, HC_WHITE, LV_TEXT_ALIGN_CENTER);
    hw_text_in(l, "Please wait, usually under 10 s", 0, 127, 480, 13, HF_10, HC_LABEL, LV_TEXT_ALIGN_CENTER);
    static const char *const names[3] = {"Associating", "Authenticating", "Obtaining IP address"};
    const int step = ws.state == HMI_WIFI_CONNECTING ? ws.step : 0;
    for (int k = 0; k < 3; ++k) {
        const int y = 168 + k * 22;
        if (k < step) {
            hw_box(l, 90, y, 16, 16, HC_OK_BG, 8);
            hw_icon_c(l, &hmi_icon_check_11, 98, y + 8, HC_OK_COL);
        } else if (k == step) {
            wifi_spinner(l, &hmi_icon_spin_16, 98, y + 8, HC_NAV_CUR);
        } else {
            lv_obj_t *o = hw_obj(l, 90, y, 16, 16);
            lv_obj_set_style_radius(o, 8, 0);
            lv_obj_set_style_border_width(o, 2, 0);
            lv_obj_set_style_border_color(o, HC(HC_TRACK), 0);
        }
        hw_text_in(l, names[k], 116, y, 270, 16, k == step ? HF_12B : HF_12, k < step ? HC_TXT2 : k == step ? HC_WHITE : HC_DIM, LV_TEXT_ALIGN_LEFT);
    }
    (void)wifi_btn(l, 150, 268, 180, "CANCEL", HC_SURFACE2, HC_TXT2, true, wifi_back_cb);
}
static void wifi_view_info(lv_obj_t *l)
{
    wifi_header(l, "Wi-Fi", "Connected");
    lv_obj_t *c = hw_card_box(l, 8, 48, 464, 62, HC_OK_BG, 12, HC_OK_COL);
    hw_box(c, 14, 13, 36, 36, HC_OK_DEEP, 18);
    hw_icon_c(c, &hmi_icon_wifi_22, 32, 31, HC_OK_COL);
    hw_text_in(c, ws.ssid, 62, 15, 290, 17, HF_14B, HC_WHITE, LV_TEXT_ALIGN_LEFT);
    hw_text_in(c, "Station connected", 62, 34, 290, 13, HF_10, HC_TXT2, LV_TEXT_ALIGN_LEFT);
    hw_chip(c, 450, 22, "ONLINE", HC_OK_COL, HC_OK_DEEP);
    char sg[40]; snprintf(sg, sizeof(sg), "%d dBm \xC2\xB7 %s", (int)ws.rssi, rssi_word(ws.rssi));
    wifi_cell(l, 8, 118, "IP ADDRESS", ws.ip);
    wifi_cell(l, 243, 118, "GATEWAY", ws.gateway);
    wifi_cell(l, 8, 168, "SIGNAL", sg);
    wifi_cell(l, 243, 168, "MAC", ws.mac);

    hmi_ota_status_t ota;
    hmi_ota_get_status(&ota);
    char ota_val[64];
    if (ota.armed) snprintf(ota_val, sizeof(ota_val), "PIN %s \xC2\xB7 /update", ota.pin);
    else snprintf(ota_val, sizeof(ota_val), "Locked");
    char slot_str[80];
    snprintf(slot_str, sizeof(slot_str), "%s (Target: %s)", ota.running_slot, ota.next_slot);
    wifi_cell(l, 8, 218, ota.armed ? "WEB OTA \xC2\xB7 10 MIN, ONE USE" : "WEB OTA", ota_val);
    wifi_cell(l, 243, 218, "DUAL-OTA SLOT", slot_str);

    (void)wifi_btn(l, 8, 268, 148, "FORGET", HC_SURFACE2, HC_TXT2, true, wifi_forget_cb);
    (void)wifi_btn(l, 166, 268, 148, ota.armed ? "LOCK OTA" : "ENABLE OTA", ota.armed ? HC_WARN_BG : HC_SURFACE2,
                   ota.armed ? HC_WARN_COL : HC_TXT2, true, wifi_ota_cb);
    (void)wifi_btn(l, 324, 268, 148, "DONE", HC_START_COL, HC_HERO_COL, false, wifi_done_cb);
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
    lv_obj_t *c = hw_card_box(l, 8, 48, 464, 62, HC_FAULT_BG, 12, HC_FAULT_COL);
    hw_box(c, 14, 13, 36, 36, HC_FAULT_DEEP, 18);
    hw_icon_c(c, &hmi_icon_warn_20, 32, 31, HC_FAULT_COL);
    hw_text_in(c, title[r], 62, 9, 390, 17, HF_14B, HC_WHITE, LV_TEXT_ALIGN_LEFT);
    lv_obj_t *m = hw_text(c, msg[r], 62, 28, HF_10, HC_TXT2);
    lv_label_set_long_mode(m, LV_LABEL_LONG_MODE_WRAP);
    lv_obj_set_width(m, 388);
    char at[16]; snprintf(at, sizeof(at), "%u", (unsigned)(ws.attempt ? ws.attempt : 1));
    char sg[24]; const int tr = target_rssi();
    if (tr) snprintf(sg, sizeof(sg), "%d dBm", tr); else snprintf(sg, sizeof(sg), "\xE2\x80\x94");
    wifi_cell(l, 8, 118, "ATTEMPT", at);
    wifi_cell(l, 243, 118, "SIGNAL", sg);
    if (r == HMI_WIFI_FAIL_PASSWORD)
        hw_text_in(l, "Passwords are case-sensitive. WPA2 passwords have 8 to 63 characters.", 12, 172, 456, 14, HF_10, HC_LABEL, LV_TEXT_ALIGN_LEFT);
    (void)wifi_btn(l, 8, 268, 228, "CHOOSE OTHER", HC_SURFACE2, HC_TXT2, true, wifi_done_cb);
    (void)wifi_btn(l, 244, 268, 228, "RETRY", HC_START_COL, HC_HERO_COL, false, wifi_retry_cb);
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
    wifi_sig = wifi_signature();
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
    const uint32_t sig = wifi_signature();
    if (sig == wifi_sig) return;
    for (lv_indev_t *i = lv_indev_get_next(NULL); i; i = lv_indev_get_next(i))
        if (lv_indev_get_state(i) == LV_INDEV_STATE_PRESSED) return;
    wifi_render();
}

void hmi_wifi_ui_open(void)
{
    if (wifi_layer) return;
    hw_scroll_reset(SLOT_LIST);
    wifi_view = WV_LIST;
    wifi_layer = hmi_shell_overlay_open();
    if (!wifi_layer) return;
    hmi_wifi_scan();
    hmi_wifi_get(&ws);
    wifi_render();
    wifi_timer = lv_timer_create(wifi_tick, 250, NULL);
}
#ifdef HMI_SIM
void hmi_wifi_ui_inject(const char *s) { for (; *s; ++s) wifi_type((unsigned char)*s); wifi_field_draw(); wifi_count_update(); }
#endif
