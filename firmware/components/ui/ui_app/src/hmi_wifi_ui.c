/*
 * Wi-Fi flow and on-screen keyboard: full-screen overlay opened from Device > Settings > Wi-Fi.
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

#include "hmi_lvgl_headers.h"
#include "rcc_wifi_busy_gen.h"
#include "rcc_wifi_fail_gen.h"
#include "rcc_wifi_header_gen.h"
#include "rcc_wifi_info_gen.h"
#include "rcc_wifi_list_gen.h"
#include "rcc_wifi_pass_gen.h"
#include "rcc_wifi_row_gen.h"

static lv_obj_t *wifi_layer;
static hmi_wifi_status_t ws;

/* ------------------------------------------------------------------ Wi-Fi screens
 * Full-screen overlay (no nav rail) opened from Device > Settings > Wi-Fi. Views: network list,
 * password entry (on-screen keyboard), connecting, connected info, failure. The overlay polls
 * hmi_wifi_get() every 250 ms and re-renders only when the backend version or the view changes. */
typedef enum { WV_LIST, WV_PASS, WV_BUSY, WV_INFO, WV_FAIL } wifi_view_t;
enum { KEY_SHIFT = 256, KEY_BKSP, KEY_SYM, KEY_GO };
static int32_t wifi_list_scroll;

static lv_obj_t *wifi_field, *wifi_go_key, *wifi_go_label;
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

/* ---- generated layout bindings */
static lv_obj_t *wifi_widget(lv_obj_t *root, const char *name)
{
    lv_obj_t *widget = lv_obj_find_by_name(root, name);
    LV_ASSERT_MSG(widget != NULL, name);
    return widget;
}

static void wifi_bind(lv_obj_t *root, const char *name, lv_event_cb_t callback, void *user_data)
{
    lv_obj_add_event_cb(wifi_widget(root, name), callback, LV_EVENT_CLICKED, user_data);
}

static void wifi_header(lv_obj_t *parent, const char *title, const char *subtitle)
{
    lv_obj_t *header = rcc_wifi_header_create(parent, title, subtitle, (lv_font_t *)HF_14B,
                                             (lv_font_t *)HF_10, &hmi_icon_back_14);
    wifi_bind(header, "back", wifi_back_cb, NULL);
}

static void wifi_spinner_start(lv_obj_t *icon)
{
    const lv_image_dsc_t *image = lv_image_get_src(icon);
    lv_image_set_pivot(icon, image->header.w / 2, image->header.h / 2);
    lv_anim_t animation;
    lv_anim_init(&animation);
    lv_anim_set_var(&animation, icon);
    lv_anim_set_exec_cb(&animation, hw_spin_anim);
    lv_anim_set_values(&animation, 0, 3600);
    lv_anim_set_duration(&animation, 1000);
    lv_anim_set_repeat_count(&animation, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&animation);
}

static void wifi_save_list_scroll(lv_event_t *event)
{
    wifi_list_scroll = lv_obj_get_scroll_y(lv_event_get_target_obj(event));
}

/* ---- list view */
static void wifi_view_list(lv_obj_t *parent)
{
    char subtitle[32];
    const bool connected = ws.state == HMI_WIFI_CONNECTED;
    const bool unavailable = ws.state == HMI_WIFI_UNAVAILABLE;
    const bool scanning = ws.state == HMI_WIFI_SCANNING;
    if (unavailable) snprintf(subtitle, sizeof(subtitle), "Wi-Fi unavailable");
    else if (scanning) snprintf(subtitle, sizeof(subtitle), "Scanning...");
    else snprintf(subtitle, sizeof(subtitle), "%u network%s found", (unsigned)ws.ap_count, ws.ap_count == 1 ? "" : "s");
    wifi_header(parent, "Wi-Fi", subtitle);

    lv_obj_t *view = rcc_wifi_list_create(parent, (lv_font_t *)HF_10, (lv_font_t *)HF_12, &hmi_icon_refresh_11);
    wifi_bind(view, "scan", wifi_scan_cb, NULL);
    wifi_bind(view, "disconnect", wifi_disc_cb, NULL);
    lv_obj_t *status = wifi_widget(view, "status");
    lv_obj_t *status_dot = wifi_widget(view, "status_dot");
    lv_obj_t *status_text = wifi_widget(view, "status_text");
    lv_obj_t *status_ssid = wifi_widget(view, "status_ssid");
    lv_obj_set_hidden(status_ssid, !connected);
    lv_obj_set_hidden(wifi_widget(view, "disconnect"), !connected);
    if (connected) {
        lv_label_set_text(status_ssid, ws.ssid);
    } else {
        lv_obj_set_style_bg_color(status, HC(unavailable ? HC_WARN_BG : HC_CARD), 0);
        lv_obj_set_style_border_color(status, HC(unavailable ? HC_WARN_COL : HC_LINE), 0);
        lv_obj_set_style_bg_color(status_dot, HC(unavailable ? HC_WARN_COL : scanning ? HC_NAV_CUR : HC_DIM), 0);
        lv_obj_update_layout(status);
        lv_obj_set_width(status_text, lv_obj_get_width(status) - 44);
        lv_obj_set_style_text_color(status_text, HC(unavailable ? HC_WARN_COL : HC_LABEL), 0);
        lv_label_set_text(status_text, unavailable ? "Wi-Fi driver is not available in this build" :
                          scanning ? "Scanning for networks..." : "Not connected");
    }

    const bool have_networks = ws.ap_count > 0;
    lv_obj_t *empty_hint = wifi_widget(view, "empty_hint");
    lv_obj_t *networks = wifi_widget(view, "networks");
    lv_obj_set_hidden(empty_hint, have_networks);
    lv_obj_set_hidden(networks, !have_networks);
    lv_obj_set_hidden(wifi_widget(view, "list_tip"), !have_networks);
    if (!have_networks) {
        if (unavailable) lv_label_set_text(empty_hint, "Wi-Fi is not available on this controller");
        return;
    }

    lv_obj_set_scroll_dir(networks, LV_DIR_VER);
    for (int i = 0; i < ws.ap_count; ++i) {
        const hmi_wifi_ap_t *ap = &ws.ap[i];
        const bool current = connected && !strcmp(ap->ssid, ws.ssid);
        char details[40], signal[16];
        if (current) snprintf(details, sizeof(details), "Connected \xC2\xB7 %s", ws.ip);
        else snprintf(details, sizeof(details), "%s", ap->secure ? "Secured" : "Open network");
        snprintf(signal, sizeof(signal), "%d dBm", (int)ap->rssi);
        const int level = rssi_level(ap->rssi);
        const lv_color_t lit = HC(level >= 3 ? HC_OK_COL : level == 2 ? HC_WARN_COL : HC_AMP_COL);
        const lv_color_t unlit = HC(HC_TRACK);
        lv_obj_t *row = rcc_wifi_row_create(networks, i, ap->ssid, details, signal, ap->secure != 0,
                                           ap->secure ? 330 : 345, HC(current ? HC_NAV_BOX : HC_CARD),
                                           HC(current ? HC_NAV_EDGE : HC_LINE), HC(current ? HC_TXT2 : HC_LABEL),
                                           lit, level > 1 ? lit : unlit, level > 2 ? lit : unlit,
                                           level > 3 ? lit : unlit, (lv_font_t *)HF_10, (lv_font_t *)HF_12B,
                                           &hmi_icon_lock_11);
        lv_obj_add_event_cb(row, wifi_row_cb, LV_EVENT_CLICKED, (void *)(uintptr_t)i);
    }
    hw_scroll_to(networks, wifi_list_scroll);
    lv_obj_add_event_cb(networks, wifi_save_list_scroll, LV_EVENT_SCROLL, NULL);
}

/* ---- password view */
static void wifi_go_style(void)
{
    if (!wifi_go_key) return;
    const bool ok = !wifi_target_secure || wifi_pw_len >= HMI_WIFI_PASS_MIN;
    lv_obj_set_style_bg_color(wifi_go_key, HC(ok ? HC_NAV_CUR : HC_SURFACE2), 0);
    lv_obj_set_style_border_color(wifi_go_key, HC(ok ? HC_NAV_CUR : HC_LINE), 0);
    lv_obj_set_style_text_color(wifi_go_label, HC(ok ? HC_BG : HC_DIM), 0);
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

/* One matrix for each letter/digit row keeps the keyboard inside LVGL's heap. */
static char kb_txt[4][10][2];
static const char *kb_map[4][11];

static void kb_matrix_cb(lv_event_t *e)
{
    lv_obj_t *matrix = lv_event_get_target_obj(e);
    const uint32_t id = lv_buttonmatrix_get_selected_button(matrix);
    if (id == LV_BUTTONMATRIX_BUTTON_NONE) return;
    const char *text = lv_buttonmatrix_get_button_text(matrix, id);
    if (text && text[0]) wifi_key((unsigned char)text[0]);
}

static void kb_matrix_bind(lv_obj_t *matrix, int row, const char *chars)
{
    const int size = (int)strlen(chars);
    for (int i = 0; i < size; ++i) {
        const char key = chars[i];
        kb_txt[row][i][0] = (wifi_shift && key >= 'a' && key <= 'z') ? (char)(key - 32) : key;
        kb_txt[row][i][1] = '\0';
        kb_map[row][i] = kb_txt[row][i];
    }
    kb_map[row][size] = "";
    lv_buttonmatrix_set_map(matrix, kb_map[row]);
    lv_obj_add_event_cb(matrix, kb_matrix_cb, LV_EVENT_VALUE_CHANGED, NULL);
}

static void wifi_view_pass(lv_obj_t *parent)
{
    char subtitle[48], count[16];
    const int rssi = target_rssi();
    if (rssi) snprintf(subtitle, sizeof(subtitle), "Secured \xC2\xB7 Signal %d dBm", rssi);
    else snprintf(subtitle, sizeof(subtitle), "Secured network");
    wifi_header(parent, wifi_target, subtitle);
    snprintf(count, sizeof(count), "%d / %d", wifi_pw_len, HMI_WIFI_PASS_MAX);

    lv_obj_t *view = rcc_wifi_pass_create(parent, wifi_sym, wifi_shift, count, wifi_sym ? "ABC" : "?123",
                                         (lv_font_t *)HF_10, (lv_font_t *)HF_12B,
                                         wifi_show_pw ? &hmi_icon_eyeoff_14 : &hmi_icon_eye_14,
                                         &hmi_icon_shift_14, &hmi_icon_bksp_16);
    wifi_count_lbl = wifi_widget(view, "password_count");
    wifi_field = wifi_widget(view, "password_field");
    wifi_go_key = wifi_widget(view, "key_go");
    wifi_go_label = wifi_widget(view, "key_go_label");
    wifi_bind(view, "password_eye", wifi_eye_cb, NULL);
    if (wifi_show_pw)
        lv_obj_set_style_image_recolor(wifi_widget(view, "password_eye_icon"), HC(HC_NAV_CUR), 0);
    if (wifi_sym || wifi_shift)
        lv_obj_set_style_image_recolor(wifi_widget(view, "shift_image"), HC(wifi_sym ? HC_TRACK : HC_NAV_CUR), 0);

    static const char *const normal[4] = {"1234567890", "qwertyuiop", "asdfghjkl", "zxcvbnm"};
    static const char *const symbols[4] = {"!@#$%^&*()", "-_=+[]{}\\|", ";:'\"/?<>~", "`"};
    static const char *const matrix_ids[4] = {"matrix_0", "matrix_1", "matrix_2", "matrix_3"};
    const char *const *rows = wifi_sym ? symbols : normal;
    for (int i = 0; i < 4; ++i) kb_matrix_bind(wifi_widget(view, matrix_ids[i]), i, rows[i]);

    static const struct { const char *name; int code; } keys[] = {
        {"key_shift", KEY_SHIFT}, {"key_erase", KEY_BKSP}, {"key_symbols", KEY_SYM},
        {"key_comma", ','}, {"key_space", ' '}, {"key_period", '.'}, {"key_go", KEY_GO},
    };
    for (unsigned i = 0; i < sizeof(keys) / sizeof(keys[0]); ++i)
        wifi_bind(view, keys[i].name, wifi_key_cb, (void *)(uintptr_t)keys[i].code);
    wifi_field_draw();
}

/* ---- busy / info / fail views */
static void wifi_view_busy(lv_obj_t *parent)
{
    wifi_header(parent, "Connecting", wifi_target);
    const int step = ws.state == HMI_WIFI_CONNECTING ? ws.step : 0;
    lv_obj_t *view = rcc_wifi_busy_create(parent, (lv_font_t *)HF_14B, (lv_font_t *)HF_10,
                                         (lv_font_t *)HF_12, (lv_font_t *)HF_12B,
                                         &hmi_icon_spin_44, &hmi_icon_spin_16, &hmi_icon_check_11);
    wifi_spinner_start(wifi_widget(view, "spinner"));
    static const struct { const char *marker, *check, *spinner, *label; } steps[] = {
        {"step_marker_0", "step_check_0", "step_spinner_0", "step_0"},
        {"step_marker_1", "step_check_1", "step_spinner_1", "step_1"},
        {"step_marker_2", "step_check_2", "step_spinner_2", "step_2"},
    };
    for (int i = 0; i < 3; ++i) {
        const bool done = i < step, active = i == step;
        lv_obj_t *marker = wifi_widget(view, steps[i].marker);
        lv_obj_t *spinner = wifi_widget(view, steps[i].spinner);
        lv_obj_t *label = wifi_widget(view, steps[i].label);
        lv_obj_set_hidden(marker, active);
        lv_obj_set_hidden(wifi_widget(view, steps[i].check), !done);
        lv_obj_set_hidden(spinner, !active);
        if (active) wifi_spinner_start(spinner);
        lv_obj_set_style_bg_opa(marker, done ? LV_OPA_COVER : LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(marker, done ? 0 : 2, 0);
        lv_obj_set_style_text_color(label, HC(done ? HC_TXT2 : active ? HC_WHITE : HC_DIM), 0);
        lv_obj_set_style_text_font(label, active ? HF_12B : HF_12, 0);
        if (active) lv_obj_set_y(label, lv_obj_get_y_aligned(label) + 1);
    }
    wifi_bind(view, "cancel", wifi_back_cb, NULL);
}

static void wifi_view_info(lv_obj_t *parent)
{
    wifi_header(parent, "Wi-Fi", "Connected");
    char signal[40];
    snprintf(signal, sizeof(signal), "%d dBm \xC2\xB7 %s", (int)ws.rssi, rssi_word(ws.rssi));
    hmi_ota_status_t ota;
    hmi_ota_get_status(&ota);
    char ota_value[64], slot_text[80];
    if (ota.armed) snprintf(ota_value, sizeof(ota_value), "PIN %s \xC2\xB7 /update", ota.pin);
    else snprintf(ota_value, sizeof(ota_value), "Locked");
    snprintf(slot_text, sizeof(slot_text), "%s (Target: %s)", ota.running_slot, ota.next_slot);

    lv_obj_t *view = rcc_wifi_info_create(parent, ws.ssid, ws.ip, ws.gateway, signal, ws.mac,
                                         ota.armed ? "WEB OTA \xC2\xB7 10 MIN, ONE USE" : "WEB OTA",
                                         ota_value, slot_text, ota.armed ? "LOCK OTA" : "ENABLE OTA",
                                         ota.armed, (lv_font_t *)HF_10, (lv_font_t *)HF_12B,
                                         (lv_font_t *)HF_14B, &hmi_icon_wifi_22);
    wifi_bind(view, "forget", wifi_forget_cb, NULL);
    wifi_bind(view, "ota", wifi_ota_cb, NULL);
    wifi_bind(view, "done", wifi_done_cb, NULL);
}

static void wifi_view_fail(lv_obj_t *parent)
{
    static const char *const title[5] = {"Connection failed", "Wrong password", "Network not found", "No IP address received", "Connection failed"};
    static const char *const msg[5] = {
        "The Wi-Fi driver reported an error. Try again.",
        "The access point rejected the credentials. Check the password and try again.",
        "The access point is out of range or was renamed. Move closer or scan again.",
        "Joined the access point, but DHCP did not answer. Check the router.",
        "The Wi-Fi driver reported an error. Try again."};
    const int reason = ws.fail_reason < 5 ? ws.fail_reason : 4;
    wifi_header(parent, "Connection failed", wifi_target);
    char attempt[16], signal[24];
    snprintf(attempt, sizeof(attempt), "%u", (unsigned)(ws.attempt ? ws.attempt : 1));
    const int rssi = target_rssi();
    if (rssi) snprintf(signal, sizeof(signal), "%d dBm", rssi);
    else snprintf(signal, sizeof(signal), "\xE2\x80\x94");

    lv_obj_t *view = rcc_wifi_fail_create(parent, title[reason], msg[reason], attempt, signal,
                                         reason == HMI_WIFI_FAIL_PASSWORD, (lv_font_t *)HF_10,
                                         (lv_font_t *)HF_12B, (lv_font_t *)HF_14B, &hmi_icon_warn_20);
    wifi_bind(view, "choose_other", wifi_done_cb, NULL);
    wifi_bind(view, "retry", wifi_retry_cb, NULL);
}

static void wifi_render(void)
{
    if (!wifi_layer) return;
    lv_obj_clean(wifi_layer);
    wifi_field = NULL; wifi_go_key = NULL; wifi_go_label = NULL; wifi_count_lbl = NULL;
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
    wifi_list_scroll = 0;
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
