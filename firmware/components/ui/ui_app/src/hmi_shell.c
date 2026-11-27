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
#include "hmi_power.h"
#include "hmi_config.h"
#include "hmi_icons.h"
#include "hmi_model.h"
#include "hmi_nav.h"
#include "hmi_ota.h"
#include "hmi_pages.h"
#include "hmi_present.h"
#include "hmi_theme.h"
#include "hmi_theme_present.h"
#include "hmi_widgets.h"
#include "pages/page_device.h"
#include "rcc_link.h"

#include "hmi_lvgl_headers.h"
#include "rcc_shell_action_gen.h"
#include "rcc_shell_action_status_gen.h"
#include "rcc_shell_banner_gen.h"
#include "rcc_shell_confirm_gen.h"
#include "rcc_shell_frame_gen.h"
#include "rcc_shell_header_fault_gen.h"
#include "rcc_shell_header_link_gen.h"
#include "rcc_shell_header_node_gen.h"
#include "rcc_shell_no_authority_gen.h"
#include "rcc_shell_ota_gen.h"
#include "rcc_shell_overlay_gen.h"
#include "rcc_shell_rail_gen.h"
#include "rcc_shell_rail_badge_gen.h"
#include "rcc_shell_rail_tab_gen.h"

/* The Editor uses mutable font pointers in generated APIs. LVGL only reads these. */
#define LAYOUT_FONT(font) ((lv_font_t *)(font))

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

/* The retained page on screen (ops != NULL), identified by what forces a re-enter: page, geometry, nav epoch. */
static struct { int page, h, top; unsigned epoch; } live = {.page = -1};
static void drop_live(void)   /* before content is cleaned: let the page forget its widgets */
{
    if (live.page >= 0 && hmi_pages[live.page].ops && hmi_pages[live.page].ops->leave) hmi_pages[live.page].ops->leave();
    live.page = -1;
}

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
    char state[48], link[48], measure[48], fault[48], age[24];
    const bool simulated = m->simulated_measurements;
    hp_controller_summary(state, sizeof(state), m);
    hp_fmt_age(age, sizeof(age), m->reply_age_ms);
    snprintf(link, sizeof(link), "ONLINE \xC2\xB7 %s ago", age);
    if (hp_v_valid(m) && hp_i_valid(m))
        snprintf(measure, sizeof(measure), "Fresh \xC2\xB7 %" PRIu32 " ms", m->measurement_sample_age_ms);
    else
        snprintf(measure, sizeof(measure), "Not valid");

    const hp_fault_state_t fs = hp_fault_state(m);
    if (fs == HP_FS_FAULT) snprintf(fault, sizeof(fault), "%s", hp_fault_name(m->primary_fault));
    else if (fs == HP_FS_INHIBIT) hp_mask_text(fault, sizeof(fault), m->inhibit_mask, hp_inhibit_names, 3);
    else snprintf(fault, sizeof(fault), "0x00000000 (Clear)");

    const char *title = simulated ? "Start with SIMULATED V/I" : "Confirm Charge Start";
    const char *measurement_label = simulated ? "SIMULATED V/I" : "ADC telemetry";
    const char *footnote = simulated ? "REAL relay command; contacts are not measured." :
        "Controller validates VREQ & precheck before closing relay.";
    const uint32_t fault_color = fs == HP_FS_OK ? HC_WHITE : fs == HP_FS_FAULT ? HC_FAULT_COL : HC_WARN_COL;
    lv_obj_set_style_opa(bar_box, 77, 0);
    modal = rcc_shell_confirm_create(screen, title, state, link, measurement_label, measure, fault,
                                    HC(fault_color), footnote, LAYOUT_FONT(HF_16B), LAYOUT_FONT(HF_12),
                                    LAYOUT_FONT(HF_12B), LAYOUT_FONT(HF_10));
    lv_obj_add_event_cb(modal, cancel_click, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(lv_obj_find_by_name(modal, "confirm_cancel"), cancel_click, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(lv_obj_find_by_name(modal, "confirm_send"), send_start_click, LV_EVENT_CLICKED, NULL);
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
    lv_obj_t *b = rcc_shell_action_create(p, x, w, label, glyph, HC(bg), HC(fg), LAYOUT_FONT(f));
    lv_obj_add_event_cb(b, action_click, LV_EVENT_CLICKED, (void *)(uintptr_t)(kind == HP_B_START ? 1 : 0));
    lv_obj_t *tx = lv_obj_find_by_name(b, "action_label");
    lv_obj_t *ic = lv_obj_find_by_name(b, "action_icon");
    const int iw = glyph->header.w, tw = hw_width_of(tx), total = iw + 8 + tw;
    lv_obj_set_pos(ic, (w - total) / 2, (42 - (int)glyph->header.h) / 2);
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
        rcc_shell_no_authority_create(bar_box, &hmi_icon_warn_20, LAYOUT_FONT(HF_12B), LAYOUT_FONT(HF_10));
        return;
    }
    if (b->kind == HP_BAR_FULL) { bar_button(bar_box, 0, 408, b->button, HF_16B); return; }
    const int msg_w = b->kind == HP_BAR_SPLIT ? 256 : 408;
    rcc_shell_action_status_create(bar_box, msg_w, b->title, b->sub, HC(ht_kind_color(b->title_kind)),
                                   LAYOUT_FONT(HF_12B), LAYOUT_FONT(HF_10));
    if (b->kind == HP_BAR_SPLIT) bar_button(bar_box, 266, 142, b->button, HF_14B);
}


/* ------------------------------------------------------------------ Overlay service */
/* A full-screen layer above the page and chrome (the Wi-Fi flow). The page underneath is freed (LVGL's heap is
 * small) and the 1 s rebuild pauses until the overlay is closed. */
lv_obj_t *hmi_shell_overlay_open(void)
{
    if (overlay) return NULL;
    close_modal();
    drop_live();
    lv_obj_clean(content);
    overlay = rcc_shell_overlay_create(screen);
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
    lv_obj_t *rail = rcc_shell_rail_create(screen);
    const int pitch = 315 / hmi_page_count > 63 ? 63 : 315 / hmi_page_count;   /* 63 px up to five tabs */
    for (int i = 0; i < hmi_page_count; ++i) {
        lv_obj_t *tab = rcc_shell_rail_tab_create(rail, 4 + i * pitch, pitch - 3,
                                                hmi_pages[i].name, hmi_pages[i].icon, LAYOUT_FONT(HF_10));
        rail_tab[i] = lv_obj_find_by_name(tab, "tab_hit");
        rail_mark[i] = lv_obj_find_by_name(tab, "tab_mark");
        rail_icon[i] = lv_obj_find_by_name(tab, "tab_icon");
        rail_label[i] = lv_obj_find_by_name(tab, "tab_label");
        lv_obj_add_event_cb(rail_tab[i], nav_click, LV_EVENT_CLICKED, (void *)(uintptr_t)i);
        rail_badge[i] = hmi_pages[i].fault_badge ? rcc_shell_rail_badge_create(rail_tab[i]) : NULL;
    }

    hdr_dev = rcc_shell_header_node_create(screen, LAYOUT_FONT(HF_12B));
    lv_obj_add_event_cb(hdr_dev, dev_click, LV_EVENT_CLICKED, NULL);
    lv_obj_t *n1 = lv_obj_find_by_name(hdr_dev, "node_prefix");
    lv_obj_t *n2 = lv_obj_find_by_name(hdr_dev, "node_name");
    const int nw = hw_width_of(n1);
    lv_obj_set_x(n2, lv_obj_get_x_aligned(n1) + nw + 8);
    hdr_dev_w = lv_obj_get_x_aligned(n2) + hw_width_of(n2);
    lv_obj_set_width(hdr_dev, hdr_dev_w);

    hdr_pill = rcc_shell_header_link_create(screen, LAYOUT_FONT(HF_12B));
    hdr_dot = lv_obj_find_by_name(hdr_pill, "link_dot");
    hdr_text = lv_obj_find_by_name(hdr_pill, "link_text");
    lv_obj_add_event_cb(hdr_pill, link_click, LV_EVENT_CLICKED, NULL);

    hdr_fault_box = rcc_shell_header_fault_create(screen, &hmi_icon_checkc_15);
    hdr_fault_icon = lv_obj_find_by_name(hdr_fault_box, "fault_icon");
    lv_obj_add_event_cb(hdr_fault_box, fault_click, LV_EVENT_CLICKED, NULL);

    banner_box = rcc_shell_banner_create(screen, &hmi_icon_warn_12, LAYOUT_FONT(HF_10));
    banner_icon = lv_obj_find_by_name(banner_box, "banner_icon");
    banner_text = lv_obj_find_by_name(banner_box, "banner_text");
    lv_obj_add_event_cb(banner_box, banner_click, LV_EVENT_CLICKED, NULL);
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
        const int label_width = hw_width_of(rail_label[i]);
        lv_obj_set_x(rail_label[i], (lv_obj_get_width(rail_tab[i]) - label_width) / 2);
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

    const int top = banner ? (two_line_banner ? 70 : 56) : 34;
    const int bottom = (spec.kind == HP_BAR_NONE) ? 312 : 264;
    const hmi_page_def_t *pd = &hmi_pages[page];
    const hmi_page_ctx_t ctx = {.height = bottom - top, .link_started = link_started, .bar = &spec};
    if (pd->ops) hmi_model_update(&m);                    /* only retained pages observe it; observers run before any leave() */
    if (!pd->ops) {                                       /* legacy page: rebuilt from the snapshot every refresh */
        drop_live();
        lv_obj_clean(content);
        pd->build(hw_obj(content, 64, top, 408, bottom - top), &m, &ctx);
    } else if (live.page != page || live.h != ctx.height || live.top != top || live.epoch != hmi_nav_epoch()) {
        drop_live();                                      /* entered, moved or resized: create the widgets once */
        lv_obj_clean(content);
        live.page = page; live.h = ctx.height; live.top = top; live.epoch = hmi_nav_epoch();
        pd->ops->enter(hw_obj(content, 64, top, 408, bottom - top), &m, &ctx);
    } else {
        if (pd->ops->update) pd->ops->update(&m, &ctx);   /* observers already redrew what changed */
    }
}
static void redraw_cb(void *unused) { (void)unused; rebuild(); }

static lv_obj_t *s_ota_modal = NULL;
static lv_obj_t *s_ota_bar = NULL;
static lv_obj_t *s_ota_label = NULL;
static lv_obj_t *s_ota_track = NULL;

static void update_ota_overlay(void)
{
    hmi_ota_status_t ota;
    hmi_ota_get_status(&ota);
    if (ota.state == HMI_OTA_RECEIVING || ota.state == HMI_OTA_VERIFYING || ota.state == HMI_OTA_SUCCESS) {
        if (!s_ota_modal) {
            s_ota_modal = rcc_shell_ota_create(screen, LAYOUT_FONT(HF_16B), LAYOUT_FONT(HF_12),
                                              LAYOUT_FONT(HF_12B), LAYOUT_FONT(HF_14B));
            s_ota_track = lv_obj_find_by_name(s_ota_modal, "ota_track");
            s_ota_bar = lv_obj_find_by_name(s_ota_modal, "ota_fill");
            s_ota_label = lv_obj_find_by_name(s_ota_modal, "ota_status");
            lv_obj_move_foreground(s_ota_modal);
            lv_obj_update_layout(s_ota_track);
        }
        if (s_ota_modal) {
            int w = (ota.progress_pct * lv_obj_get_width(s_ota_track)) / 100;
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
    hmi_power_tick(!in_session);

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
    lv_obj_t *active = lv_screen_active();
    lv_obj_remove_style_all(active);
    lv_obj_set_scrollable(active, false);
    screen = rcc_shell_frame_create(active);
    bar_box = lv_obj_find_by_name(screen, "action_host");
    content = lv_obj_find_by_name(screen, "page_host");

    hmi_model_init();
    build_chrome();
    lv_obj_move_foreground(bar_box);
    lv_obj_move_background(content);
    hmi_nav_bind(redraw_cb, close_modal);
    lv_timer_create(refresh, 1000, NULL);
    rebuild();
}

void hmi_shell_select_page(int page_idx, int sub_tab)
{
    hmi_nav_goto(page_idx, sub_tab);
}
