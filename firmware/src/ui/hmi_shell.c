/*
 * HMI shell: chrome (rail/header/banner), action bar + confirm modal, OTA overlay, overlay service, rebuild/refresh timer.
 * Pages come from the registry in hmi_pages.h and navigation state is in hmi_nav.c; the Wi-Fi flow (hmi_wifi_ui.c)
 * is an overlay opened through the overlay service.
 * docs/mockups/hmi-ui-landscape.html is a browser port of this UI; keep the two in step.
 * Called only under the LVGL port lock. Palette/fonts: hmi_theme.h; text and derived state: hmi_present.h.
 */
#include "hmi_shell.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "esp_task_wdt.h"
#include "hmi_backlight.h"
#include "hmi_config.h"
#include "hmi_icons.h"
#include "hmi_nav.h"
#include "hmi_ota.h"
#include "hmi_pages.h"
#include "hmi_present.h"
#include "hmi_theme.h"
#include "hmi_theme_present.h"
#include "hmi_widgets.h"
#include "pages/page_device.h"
#include "rcc_link.h"

static bool link_started;
static lv_obj_t *screen, *content, *bar_box, *modal, *overlay;
static lv_obj_t *rail_tab[HMI_PAGES_MAX], *rail_mark[HMI_PAGES_MAX], *rail_badge[HMI_PAGES_MAX];
static lv_obj_t *rail_icon[HMI_PAGES_MAX], *rail_label[HMI_PAGES_MAX];
static lv_obj_t *hdr_dev, *hdr_pill, *hdr_dot, *hdr_text, *hdr_fault_box, *hdr_fault_icon;
static lv_obj_t *banner_box, *banner_icon, *banner_text;
static char bar_signature[200];
static int hdr_dev_w = 99;
static int banner_target = -1;

static void redraw_cb(void *unused);

/* ------------------------------------------------------------------ Action bar */

static void close_modal(void)
{
    if (modal) { lv_obj_delete_async(modal); modal = NULL; lv_obj_set_style_opa(bar_box, LV_OPA_COVER, 0); }
}
static void cancel_click(lv_event_t *e) { (void)e; close_modal(); }
static void send_start_click(lv_event_t *e) { (void)e; (void)rcc_link_request_start(); close_modal(); }

static void open_modal(const rcc_snapshot_t *m)
{
    if (modal) return;
    char b[48], t[24];
    lv_obj_set_style_opa(bar_box, 77, 0);
    modal = hw_obj(screen, 56, 32, 424, 288);
    lv_obj_set_clickable(modal, true);
    lv_obj_set_style_bg_color(modal, HC(HC_INK), 0);
    lv_obj_set_style_bg_opa(modal, 209, 0);
    lv_obj_add_event_cb(modal, cancel_click, LV_EVENT_CLICKED, NULL);

    lv_obj_t *card = hw_card_box(modal, 36, 14, 352, 208, HC_CARD, 12, HC_NAV_CUR);
    lv_obj_set_clickable(card, true);                       /* taps inside the card must not cancel */
    hw_text_in(card, m->simulated_measurements ? "Start with SIMULATED V/I" : "Confirm Charge Start", 17, 12, 318, 19, HF_16B, HC_WHITE, LV_TEXT_ALIGN_LEFT);

    hp_controller_summary(b, sizeof(b), m);
    hw_kv(card, 17, 38, 318, 21, "Controller state", b, HC_WHITE);

    hp_fmt_age(t, sizeof(t), m->reply_age_ms); snprintf(b, sizeof(b), "ONLINE \xC2\xB7 %s ago", t);
    hw_kv(card, 17, 59, 318, 21, "UART link", b, HC_WHITE);

    if (hp_v_valid(m) && hp_i_valid(m)) snprintf(b, sizeof(b), "Fresh \xC2\xB7 %" PRIu32 " ms", m->measurement_sample_age_ms);
    else snprintf(b, sizeof(b), "Not valid");
    hw_kv(card, 17, 80, 318, 21, m->simulated_measurements ? "SIMULATED V/I" : "ADC telemetry", b, HC_WHITE);

    const hp_fault_state_t fs = hp_fault_state(m);
    if (fs == HP_FS_FAULT) snprintf(b, sizeof(b), "%s", hp_fault_name(m->primary_fault));
    else if (fs == HP_FS_INHIBIT) hp_mask_text(b, sizeof(b), m->inhibit_mask, hp_inhibit_names, 3);
    else snprintf(b, sizeof(b), "0x00000000 (Clear)");
    hw_kv(card, 17, 101, 318, 21, "Fault / Inhibit", b, fs == HP_FS_OK ? HC_WHITE : fs == HP_FS_FAULT ? HC_FAULT_COL : HC_WARN_COL);

    hw_text_in(card, m->simulated_measurements ? "REAL relay command; contacts are not measured." : "Controller validates VREQ & precheck before closing relay.", 17, 128, 318, 13, HF_10, HC_LABEL, LV_TEXT_ALIGN_LEFT);

    lv_obj_t *c1 = hw_button(card, 17, 157, 154, 38, HC_SURFACE2, 10, cancel_click, NULL);
    lv_obj_t *l1 = hw_text_in(c1, "CANCEL", 0, 0, 154, 38, HF_12B, HC_WHITE, LV_TEXT_ALIGN_CENTER); hw_spacing(l1, 1);
    lv_obj_set_clickable(l1, false);
    lv_obj_t *c2 = hw_button(card, 181, 157, 154, 38, HC_START_COL, 10, send_start_click, NULL);
    lv_obj_t *l2 = hw_text_in(c2, "SEND START", 0, 0, 154, 38, HF_12B, HC_INK, LV_TEXT_ALIGN_CENTER); hw_spacing(l2, 1);
    lv_obj_set_clickable(l2, false);
}

static void action_click(lv_event_t *e)
{
    const bool start = (uintptr_t)lv_event_get_user_data(e) == 1u;
    if (modal) return;
    if (start) { rcc_snapshot_t m; rcc_link_snapshot(&m); if (hp_start_allowed(&m)) open_modal(&m); }
    else (void)rcc_link_request_stop();
}

static void bar_button(lv_obj_t *p, int x, int w, hp_bar_button_t kind, const lv_font_t *f)
{
    const char *label = kind == HP_B_START ? "START" : kind == HP_B_STOPPING ? "STOPPING\xE2\x80\xA6" : "STOP";
    const lv_image_dsc_t *glyph = kind == HP_B_START ? &hmi_icon_play_16 : kind == HP_B_STOPPING ? &hmi_icon_spin_16 : &hmi_icon_stop_16;
    const uint32_t bg = kind == HP_B_START ? HC_START_COL : kind == HP_B_STOPPING ? HC_STOPPING_BG : HC_STOP_COL;
    const uint32_t fg = kind == HP_B_START ? HC_INK : kind == HP_B_STOPPING ? HC_STOPPING_FG : HC_HERO_COL;
    lv_obj_t *b = hw_button(p, x, 0, w, 42, bg, 10, action_click, (void *)(uintptr_t)(kind == HP_B_START ? 1 : 0));
    lv_obj_set_style_border_width(b, 1, 0);
    lv_obj_set_style_border_color(b, HC(HC_HERO_COL), 0);
    lv_obj_set_style_border_opa(b, 36, 0);
    lv_obj_t *tx = hw_text(b, label, 0, 0, f, fg);
    hw_spacing(tx, 1);
    lv_obj_set_clickable(tx, false);
    const int iw = glyph->header.w, tw = hw_width_of(tx), total = iw + 8 + tw;
    lv_obj_t *ic = hw_icon(b, glyph, (w - total) / 2, (42 - (int)glyph->header.h) / 2, fg);
    lv_obj_set_pos(tx, (w - total) / 2 + iw + 8, (42 - hw_lh(f)) / 2);
    if (kind == HP_B_STOPPING) {
        lv_image_set_pivot(ic, glyph->header.w / 2, glyph->header.h / 2);
        lv_anim_t an; lv_anim_init(&an);
        lv_anim_set_var(&an, ic); lv_anim_set_exec_cb(&an, hw_spin_anim);
        lv_anim_set_values(&an, 0, 3600); lv_anim_set_duration(&an, 1000);
        lv_anim_set_repeat_count(&an, LV_ANIM_REPEAT_INFINITE);
        lv_anim_start(&an);
    }
}

static void build_bar(const hp_bar_spec_t *b)
{
    lv_obj_clean(bar_box);
    if (b->kind == HP_BAR_NONE) return;
    if (b->kind == HP_BAR_NOAUTH) {
        lv_obj_t *nb = hw_card_box(bar_box, 0, 0, 408, 42, HC_WARN_BG, 10, HC_WARN_COL);
        hw_icon_row(nb, &hmi_icon_warn_20, 12, 0, 42, HC_WARN_COL);
        hw_text_in(nb, "No control authority \xE2\x80\x94 monitoring only", 42, 6, 354, 17, HF_12B, HC_WHITE, LV_TEXT_ALIGN_LEFT);
        hw_text_in(nb, "Use physical E-stop. Grant control on the controller service port.", 42, 22, 354, 13, HF_10, HC_LABEL, LV_TEXT_ALIGN_LEFT);
        return;
    }
    if (b->kind == HP_BAR_FULL) { bar_button(bar_box, 0, 408, b->button, HF_16B); return; }
    const int msg_w = b->kind == HP_BAR_SPLIT ? 256 : 408;
    lv_obj_t *mb = hw_card_box(bar_box, 0, 0, msg_w, 42, HC_CARD, 10, HC_LINE);
    hw_text_in(mb, b->title, 13, 7, msg_w - 26, 15, HF_12B, ht_kind_color(b->title_kind), LV_TEXT_ALIGN_LEFT);
    hw_text_in(mb, b->sub, 13, 23, msg_w - 26, 13, HF_10, HC_LABEL, LV_TEXT_ALIGN_LEFT);
    if (b->kind == HP_BAR_SPLIT) bar_button(bar_box, 266, 142, b->button, HF_14B);
}


/* ------------------------------------------------------------------ Overlay service */
/* A full-screen layer above the page and chrome (the Wi-Fi flow). The page underneath is freed (LVGL's heap is
 * small) and the 1 s rebuild pauses until the overlay is closed. */
lv_obj_t *hmi_shell_overlay_open(void)
{
    if (overlay) return NULL;
    close_modal();
    lv_obj_clean(content);
    overlay = hw_box(screen, 0, 0, 480, 320, HC_BG, 0);
    lv_obj_set_clickable(overlay, true);
    lv_obj_move_foreground(overlay);
    return overlay;
}
void hmi_shell_overlay_close(void)
{
    if (overlay) { lv_obj_delete_async(overlay); overlay = NULL; }
    hmi_nav_request_redraw();
}

/* ------------------------------------------------------------------ Chrome */
static void nav_click(lv_event_t *e) { hmi_nav_goto((int)(uintptr_t)lv_event_get_user_data(e), -1); }
static void fault_click(lv_event_t *e) { (void)e; hmi_nav_goto(HMI_PAGE_FAULTS, -1); }
static void dev_click(lv_event_t *e) { (void)e; hmi_nav_goto(HMI_PAGE_DEVICE, HMI_DEV_CONTROLLER); }
static void link_click(lv_event_t *e) { (void)e; hmi_nav_goto(HMI_PAGE_DEVICE, HMI_DEV_LINK); }
static void banner_click(lv_event_t *e)
{
    if (banner_target == HMI_PAGE_FAULTS) fault_click(e);
    else if (banner_target == HMI_PAGE_DEVICE) link_click(e);
}

static void build_chrome(void)
{
    hw_box(screen, 0, 0, 56, 320, HC_INK, 0);
    hw_box(screen, 55, 0, 1, 320, HC_LINE, 0);

    const int pitch = 315 / hmi_page_count > 63 ? 63 : 315 / hmi_page_count;   /* 63 px up to five tabs */
    for (int i = 0; i < hmi_page_count; ++i) {
        rail_tab[i] = hw_button(screen, 4, 4 + i * pitch, 47, pitch - 3, HC_INK, 10, nav_click, (void *)(uintptr_t)i);
        lv_obj_set_style_bg_opa(rail_tab[i], LV_OPA_TRANSP, 0);
        rail_mark[i] = hw_box(screen, 0, 4 + i * pitch + 14, 3, 32, HC_NAV_CUR, 2);
        rail_icon[i] = hw_icon(rail_tab[i], hmi_pages[i].icon, 14, 12, HC_DIM);
        rail_label[i] = hw_text(rail_tab[i], hmi_pages[i].name, 0, 35, HF_10, HC_DIM);
        lv_obj_set_clickable(rail_label[i], false);
        rail_badge[i] = NULL;
        if (hmi_pages[i].fault_badge) {
            rail_badge[i] = hw_box(rail_tab[i], 31, 8, 8, 8, HC_WARN_COL, 4);
            lv_obj_set_style_outline_width(rail_badge[i], 2, 0);
            lv_obj_set_style_outline_color(rail_badge[i], HC(HC_INK), 0);
        }
    }

    hdr_dev = hw_button(screen, 64, 4, 110, 24, HC_BG, 6, dev_click, NULL);
    lv_obj_set_style_bg_opa(hdr_dev, LV_OPA_TRANSP, 0);
    lv_obj_t *n1 = hw_text(hdr_dev, "NODE", 0, 0, HF_12B, HC_LABEL);
    hw_spacing(n1, 1);
    lv_obj_set_clickable(n1, false);
    const int nw = hw_width_of(n1);
    lv_obj_set_pos(n1, 0, (24 - hw_lh(HF_12B)) / 2);
    lv_obj_t *n2 = hw_text(hdr_dev, "RCC-01", nw + 8, 0, HF_12B, HC_TXT2);
    hw_spacing(n2, 1);
    lv_obj_set_clickable(n2, false);
    lv_obj_set_pos(n2, nw + 8, (24 - hw_lh(HF_12B)) / 2);
    hdr_dev_w = nw + 8 + hw_width_of(n2);
    lv_obj_set_width(hdr_dev, hdr_dev_w);

    hdr_pill = hw_button(screen, 239, 5, 123, 23, HC_CARD, 10, link_click, NULL);
    lv_obj_set_style_border_width(hdr_pill, 1, 0);
    lv_obj_set_style_border_color(hdr_pill, HC(HC_LINE), 0);
    lv_obj_set_style_pad_all(hdr_pill, -1, 0);
    hdr_dot = hw_box(hdr_pill, 11, 8, 7, 7, HC_OK_COL, 4);
    hdr_text = hw_text(hdr_pill, "", 24, 0, HF_12B, HC_OK_COL);
    lv_obj_set_clickable(hdr_text, false);

    hdr_fault_box = hw_button(screen, 438, 4, 34, 24, HC_CARD, 8, fault_click, NULL);
    lv_obj_set_style_border_width(hdr_fault_box, 1, 0);
    lv_obj_set_style_border_color(hdr_fault_box, HC(HC_LINE), 0);
    lv_obj_set_style_pad_all(hdr_fault_box, -1, 0);
    hdr_fault_icon = hw_icon(hdr_fault_box, &hmi_icon_checkc_15, 9, 5, HC_DIM);
    lv_obj_set_clickable(hdr_fault_icon, false);

    banner_box = hw_button(screen, 64, 32, 408, 20, HC_OFF_BG, 6, banner_click, NULL);
    lv_obj_set_style_border_width(banner_box, 1, 0);
    lv_obj_set_style_border_color(banner_box, HC(HC_HERO_COL), 0);
    lv_obj_set_style_border_opa(banner_box, 26, 0);
    lv_obj_set_style_pad_all(banner_box, -1, 0);
    banner_icon = hw_icon(banner_box, &hmi_icon_warn_12, 0, 0, HC_WARN_COL);
    banner_text = hw_text(banner_box, "", 0, 0, HF_10, HC_WARN_COL);
    lv_obj_set_clickable(banner_icon, false);
    lv_obj_set_clickable(banner_text, false);
    lv_obj_set_hidden(banner_box, true);
}

static void update_chrome(const rcc_snapshot_t *m, const char *banner, hp_kind_t banner_kind, int target)
{
    const hp_fault_state_t fs = hp_fault_state(m);
    const uint32_t badge = (fs == HP_FS_FAULT) ? HC_FAULT_COL : HC_WARN_COL;
    for (int i = 0; i < hmi_page_count; ++i) {
        const bool on = i == hmi_nav_page();
        lv_obj_set_style_image_recolor(rail_icon[i], HC(on ? HC_NAV_CUR : HC_DIM), 0);
        lv_obj_set_style_text_color(rail_label[i], HC(on ? HC_WHITE : HC_DIM), 0);
        lv_obj_set_hidden(rail_mark[i], !on);
        if (on) {
            lv_obj_set_style_bg_color(rail_tab[i], HC(HC_CARD), 0);
            lv_obj_set_style_bg_opa(rail_tab[i], LV_OPA_COVER, 0);
        } else {
            lv_obj_set_style_bg_opa(rail_tab[i], LV_OPA_TRANSP, 0);
        }
        lv_obj_set_pos(rail_label[i], (47 - hw_width_of(rail_label[i])) / 2, 35 + (13 - hw_lh(HF_10)) / 2);
        if (rail_badge[i]) {
            lv_obj_set_style_bg_color(rail_badge[i], HC(badge), 0);
            lv_obj_set_hidden(rail_badge[i], !(fs == HP_FS_FAULT || fs == HP_FS_INHIBIT));
        }
    }

    char b[40], t[24];
    uint32_t pc = HC_LABEL;
    if (!link_started) { snprintf(b, sizeof(b), "UART FAILED"); pc = HC_FAULT_COL; }
    else if (!m->online) { hp_fmt_age(t, sizeof(t), m->reply_age_ms); snprintf(b, sizeof(b), "OFFLINE \xC2\xB7 %s", t); pc = HC_OFF_COL; }
    else { hp_fmt_age(t, sizeof(t), m->reply_age_ms); snprintf(b, sizeof(b), "ONLINE \xC2\xB7 %s", t); pc = HC_OK_COL; }
    lv_label_set_text(hdr_text, b);
    lv_obj_set_style_text_color(hdr_text, HC(pc), 0);
    lv_obj_set_style_bg_color(hdr_dot, HC(pc), 0);
    const int tw = hw_width_of(hdr_text), pw = tw + 35;
    lv_obj_set_size(hdr_pill, pw, 23);
    lv_obj_set_pos(hdr_text, 24, (23 - hw_lh(HF_12B)) / 2);
    lv_obj_set_x(hdr_pill, 64 + hdr_dev_w + (408 - hdr_dev_w - pw - 34) / 2);

    const uint32_t fbg = fs == HP_FS_UNKNOWN ? HC_CARD : ht_fault_bg(fs);   /* the header chip stays neutral while unreadable */
    lv_obj_set_style_bg_color(hdr_fault_box, HC(fbg), 0);
    lv_image_set_src(hdr_fault_icon, (fs == HP_FS_OK) ? &hmi_icon_checkc_15 : &hmi_icon_warn_15);
    lv_obj_set_style_image_recolor(hdr_fault_icon, HC(ht_fault_color(fs)), 0);

    banner_target = target;
    if (banner) {
        const uint32_t fg = (banner_kind == HP_K_FAULT) ? HC_FAULT_COL : (banner_kind == HP_K_WARN) ? HC_WARN_COL : HC_OFF_COL;
        const uint32_t bg_c = (banner_kind == HP_K_FAULT) ? HC_FAULT_BG : (banner_kind == HP_K_WARN) ? HC_WARN_BG : HC_OFF_BG;
        lv_obj_set_hidden(banner_box, false);
        lv_obj_set_clickable(banner_box, target >= 0);
        lv_obj_set_style_bg_color(banner_box, HC(bg_c), 0);
        lv_label_set_text(banner_text, banner);
        lv_obj_set_style_text_color(banner_text, HC(fg), 0);
        lv_obj_set_style_image_recolor(banner_icon, HC(fg), 0);
        const int bw = 12 + 6 + hw_width_of(banner_text), x0 = (408 - bw) / 2;
        lv_obj_set_pos(banner_icon, x0, (20 - 12) / 2);
        lv_obj_set_pos(banner_text, x0 + 18, (20 - hw_lh(HF_10)) / 2);
    } else {
        lv_obj_set_hidden(banner_box, true);
    }
}

/* ------------------------------------------------------------------ Rebuild */
static void rebuild(void)
{
    rcc_snapshot_t m;
    rcc_link_snapshot(&m);
    hp_banner_t bn; hp_banner(&m, link_started, &bn);
    const char *banner = bn.text;
    const bool two_line_banner = bn.two_line;
    const int target = bn.target == HP_TGT_FAULTS ? HMI_PAGE_FAULTS : bn.target == HP_TGT_LINK ? HMI_PAGE_DEVICE : -1;
    lv_obj_set_height(banner_box, two_line_banner ? 34 : 20);
    update_chrome(&m, banner, bn.kind, target);

    const int page = hmi_nav_page();
    hp_bar_spec_t spec; hp_bar_spec(&m, hmi_pages[page].main_bar, &spec);
    char sig[sizeof(bar_signature)];
    snprintf(sig, sizeof(sig), "%d|%d|%d|%s|%s|%u", (int)spec.kind, (int)spec.button, (int)page, spec.title, spec.sub, (unsigned)ht_kind_color(spec.title_kind));
    if (strcmp(sig, bar_signature) != 0) { memcpy(bar_signature, sig, sizeof(sig)); build_bar(&spec); }

    lv_obj_clean(content);
    const int top = banner ? (two_line_banner ? 70 : 56) : 34;
    const int bottom = (spec.kind == HP_BAR_NONE) ? 312 : 264;
    lv_obj_t *p = hw_obj(content, 64, top, 408, bottom - top);
    const hmi_page_ctx_t ctx = {.height = bottom - top, .link_started = link_started, .bar = &spec};
    hmi_pages[page].build(p, &m, &ctx);
}
static void redraw_cb(void *unused) { (void)unused; rebuild(); }

static lv_obj_t *s_ota_modal = NULL;
static lv_obj_t *s_ota_bar = NULL;
static lv_obj_t *s_ota_label = NULL;

static void update_ota_overlay(void)
{
    hmi_ota_status_t ota;
    hmi_ota_get_status(&ota);
    if (ota.state == HMI_OTA_RECEIVING || ota.state == HMI_OTA_VERIFYING || ota.state == HMI_OTA_SUCCESS) {
        if (!s_ota_modal) {
            s_ota_modal = hw_box(screen, 30, 50, 420, 220, HC_OTA_BG, 14);
            lv_obj_set_style_border_color(s_ota_modal, HC(HC_OTA_EDGE), LV_PART_MAIN);
            lv_obj_set_style_border_width(s_ota_modal, 2, LV_PART_MAIN);
            lv_obj_move_foreground(s_ota_modal);

            hw_text_in(s_ota_modal, "FIRMWARE UPDATE IN PROGRESS", 20, 20, 380, 22, HF_16B, HC_NAV_CUR, LV_TEXT_ALIGN_CENTER);
            hw_text_in(s_ota_modal, "Target slot: writing to flash partition", 20, 50, 380, 16, HF_12, HC_LABEL, LV_TEXT_ALIGN_CENTER);
            hw_text_in(s_ota_modal, "DO NOT DISCONNECT POWER", 20, 75, 380, 18, HF_12B, HC_AMBER, LV_TEXT_ALIGN_CENTER);

            lv_obj_t *bg = hw_box(s_ota_modal, 30, 115, 360, 20, HC_LINE, 8);
            s_ota_bar = hw_box(bg, 0, 0, 4, 20, HC_NAV_CUR, 8);
            s_ota_label = hw_text_in(s_ota_modal, "Writing: 0%", 20, 150, 380, 20, HF_14B, HC_HERO_COL, LV_TEXT_ALIGN_CENTER);
        }
        if (s_ota_modal) {
            int w = (ota.progress_pct * 360) / 100;
            if (w < 4) w = 4;
            lv_obj_set_width(s_ota_bar, w);
            char txt[64];
            if (ota.state == HMI_OTA_SUCCESS) {
                snprintf(txt, sizeof(txt), "Done 100%%! Rebooting in 2s...");
            } else if (ota.state == HMI_OTA_VERIFYING) {
                snprintf(txt, sizeof(txt), "Verifying SHA256 checksum...");
            } else {
                snprintf(txt, sizeof(txt), "Writing flash: %u%% (%zu / %zu KB)",
                         (unsigned)ota.progress_pct, ota.bytes_written / 1024, ota.total_bytes / 1024);
            }
            lv_label_set_text(s_ota_label, txt);
        }
    } else if (s_ota_modal && ota.state == HMI_OTA_IDLE) {
        lv_obj_delete_async(s_ota_modal);
        s_ota_modal = NULL;
    }
}

static void refresh(lv_timer_t *timer)
{
    (void)timer;

    /* Task Watchdog Timer (TWDT) monitoring for LVGL task */
    static bool s_lvgl_wdt_added = false;
    if (!s_lvgl_wdt_added) {
        if (esp_task_wdt_add(NULL) == ESP_OK) {
            s_lvgl_wdt_added = true;
            ESP_LOGI("hmi_ui", "LVGL task subscribed to TWDT (%d ms)", HMI_TWDT_TIMEOUT_MS);
        }
    }
    if (s_lvgl_wdt_added) {
        esp_task_wdt_reset();
    }

    /* Screen saver / backlight sleep check */
    rcc_snapshot_t m_now;
    rcc_link_snapshot(&m_now);
    const bool in_session = hp_in_session(&m_now);
    hmi_backlight_tick(!in_session);

    update_ota_overlay();

    for (lv_indev_t *i = lv_indev_get_next(NULL); i; i = lv_indev_get_next(i)) {
        if (lv_indev_get_state(i) == LV_INDEV_STATE_PRESSED) return;
    }
    if (modal || overlay) return;
    rebuild();
}

void hmi_shell_start(bool started)
{
    link_started = started;
    screen = lv_screen_active();
    lv_obj_remove_style_all(screen);
    lv_obj_set_style_bg_color(screen, HC(HC_BG), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
    lv_obj_set_scrollable(screen, false);

    build_chrome();
    bar_box = hw_obj(screen, 64, 270, 408, 42);
    content = hw_obj(screen, 0, 0, 480, 320);
    lv_obj_move_background(content);
    hmi_nav_bind(redraw_cb, close_modal);
    lv_timer_create(refresh, 1000, NULL);
    rebuild();
}

void hmi_shell_select_page(int page_idx, int sub_tab)
{
    hmi_nav_goto(page_idx, sub_tab);
}
