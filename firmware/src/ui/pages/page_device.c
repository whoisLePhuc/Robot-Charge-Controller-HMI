/*
 * Device page: Controller / Comm link / Settings tabs (display and sleep presets, language, Wi-Fi entry).
 */
#include <inttypes.h>
#include <stdio.h>

#include "hmi_backlight.h"
#include "hmi_config.h"
#include "hmi_icons.h"
#include "hmi_nav.h"
#include "hmi_ota.h"
#include "hmi_present.h"
#include "hmi_settings.h"
#include "hmi_widgets.h"
#include "hmi_wifi.h"
#include "hmi_wifi_ui.h"
#include "page_device.h"

/* Unlike the other pages this one reads and changes shared services directly: hmi_settings (display, sleep,
 * language), hmi_wifi and hmi_ota (status line). Controller data still comes only from the snapshot. */
static int cur_tab = HMI_DEV_CONTROLLER;   /* visible tab, kept across rebuilds */

void page_device_set_tab(int tab)
{
    if (tab >= HMI_DEV_CONTROLLER && tab <= HMI_DEV_SETTINGS) cur_tab = tab;
}

static void tab_click(lv_event_t *e)
{
    page_device_set_tab((int)(uintptr_t)lv_event_get_user_data(e));
    hw_scroll_reset_all();
    hmi_nav_request_redraw();
}

static void set_brightness_cb(lv_event_t *e)
{
    hmi_backlight_set_brightness((uint8_t)(uintptr_t)lv_event_get_user_data(e));
    hmi_nav_request_redraw();
}

static void set_sleep_cb(lv_event_t *e)
{
    const uint16_t sec = (uint16_t)(uintptr_t)lv_event_get_user_data(e);
    hmi_settings_set_sleep_timeout(sec);
    hmi_nav_request_redraw();
}

static void wifi_open_cb(lv_event_t *e) { (void)e; hmi_wifi_ui_open(); }

static void set_lang_cb(lv_event_t *e)
{
    const uint8_t l = (uint8_t)(uintptr_t)lv_event_get_user_data(e);
    hmi_settings_set_language(l);
    hmi_nav_request_redraw();
}

void page_device_build(lv_obj_t *p, const rcc_snapshot_t *m, const hmi_page_ctx_t *ctx)
{
    const int H = ctx->height, device_tab = cur_tab;
    const bool rd = m->online && m->device_valid;
    const bool provisional = rd && (m->build_flags & 1u);
    const uint32_t nc = rd ? HC_WARN_COL : HC_OFF_COL;

    lv_obj_t *nt = hw_card_box(p, 0, 0, 408, 24, rd ? HC_WARN_BG : HC_OFF_BG, 8, nc);
    if (rd) hw_icon_row(nt, &hmi_icon_warn_13, 11, 0, 24, HC_WARN_COL);
    lv_obj_t *nl = hw_text_in(nt, rd ? (provisional ? "BENCH IMAGE \xE2\x80\x94 NOT A PRODUCT BUILD" : "PRODUCT BUILD")
                                     : "CONTROLLER INFO NOT READ", rd ? 32 : 11, 0, 250, 24, HF_10, nc, LV_TEXT_ALIGN_LEFT);
    hw_spacing(nl, 0);
    hw_text_in(nt, rd ? ((m->build_flags & 2u) ? "" : "Relay locked") : "Link lost", 280, 0, 115, 24, HF_10, HC_TXT2, LV_TEXT_ALIGN_RIGHT);

    static const char *const tab_names[3] = {"CONTROLLER", "COMM LINK", "SETTINGS"};
    lv_obj_t *seg = hw_card_box(p, 0, 30, 408, 32, HC_CARD, 8, HC_LINE);
    const int tab_w = 134;
    for (int i = 0; i < 3; ++i) {
        const bool on = (i == device_tab);
        lv_obj_t *bt = hw_button(seg, 3 + i * tab_w, 3, tab_w, 26, on ? HC_NAV_BOX : HC_CARD, 6, tab_click, (void *)(uintptr_t)i);
        if (on) { lv_obj_set_style_border_width(bt, 1, 0); lv_obj_set_style_border_color(bt, HC(HC_NAV_EDGE), 0); }
        lv_obj_t *l = hw_text_in(bt, tab_names[i], 0, 0, tab_w, 26, HF_10, on ? HC_NAV_CUR : HC_DIM, LV_TEXT_ALIGN_CENTER);
        hw_spacing(l, 1);
        lv_obj_set_clickable(l, false);
    }

    const int ry = 68, ch = H - ry, ih = ch - 14;
    char b[64], t[24];
    if (device_tab == HMI_DEV_CONTROLLER) {
        lv_obj_t *c1 = hw_card_box(p, 0, ry, 200, ch, HC_CARD, 10, HC_LINE);
        hw_section(c1, 13, 7, 174, "FIRMWARE", NULL, 0);
        const hp_fw_compat_t fw = hp_fw_compat(m);
        hw_chip(c1, 187, 7, fw == HP_FW_UNREAD ? "UNREAD" : fw == HP_FW_COMPATIBLE ? "COMPATIBLE" : "INCOMPAT.",
                fw == HP_FW_UNREAD ? HC_OFF_COL : fw == HP_FW_COMPATIBLE ? HC_OK_COL : HC_FAULT_COL,
                fw == HP_FW_UNREAD ? HC_SURFACE2 : fw == HP_FW_COMPATIBLE ? HC_OK_BG : HC_FAULT_BG);
        const int g1 = (ih - 16 - 72) / 4;
        const char *k1[4] = {"Product", "HW / FW", "Image CRC", "Boot ID"};
        char v1[4][24];
        if (rd) {
            snprintf(v1[0], 24, "0x%04X \xC2\xB7 v%u", (unsigned)m->product_id, (unsigned)m->protocol_version);
            snprintf(v1[1], 24, "0x%02" PRIX32 " / 0x%02" PRIX32, m->hardware_revision, m->firmware_revision);
            snprintf(v1[2], 24, "%02X%02X\xE2\x80\xA6%02X%02X", m->image_id[0], m->image_id[1], m->image_id[6], m->image_id[7]);
            snprintf(v1[3], 24, "\xE2\x80\xA6%04X", (unsigned)(m->boot_id & 0xffffu));
        } else {
            for (int i = 0; i < 4; ++i) snprintf(v1[i], 24, "\xE2\x80\x94");
        }
        for (int i = 0; i < 4; ++i) hw_kv(c1, 13, 7 + 16 + (i + 1) * g1 + i * 18, 174, 18, k1[i], v1[i], HC_WHITE);

        lv_obj_t *c2 = hw_card_box(p, 208, ry, 200, ch, HC_CARD, 10, HC_LINE);
        hw_section(c2, 13, 7, 174, "BUILD FLAGS & BUSES", NULL, 0);
        const int g2 = (ih - 16 - 54 - 20) / 4;
        /*
         * Row 3 is control authority from GET_STATUS (controller CR-01): the
         * controller's CONFIG names one interface; "THIS HMI" when it is ours.
         * Build-flag bit2 is reserved on current controllers and not shown.
         */
        static const char *const fl[3] = {"Provisional", "Relay enable", "Control"};
        static const char *const ifn[5] = {"NONE", "CAN", "RS485", "UART", "SVC"};
        for (int i = 0; i < 3; ++i) {
            const int yy = 7 + 16 + (i + 1) * g2 + i * 18;
            hw_text_in(c2, fl[i], 13, yy, 120, 18, HF_12, HC_LABEL, LV_TEXT_ALIGN_LEFT);
            const bool known = (i < 2) ? rd : (rd && m->status_valid);
            if (!known) {
                hw_text_in(c2, "\xE2\x80\x94", 13, yy, 174, 18, HF_12B, HC_DIM, LV_TEXT_ALIGN_RIGHT);
            } else if (i == 2) {
                const char *who = m->is_control ? "THIS HMI"
                                  : (m->control_interface < 5u ? ifn[m->control_interface] : "?");
                hw_chip(c2, 187, yy, who, m->is_control ? HC_OK_COL : HC_LABEL, HC_SURFACE2);
            } else {
                const bool on = (m->build_flags >> i) & 1u;
                hw_chip(c2, 187, yy, on ? "ON" : "OFF", on ? HC_WARN_COL : HC_LABEL, on ? HC_WARN_BG : HC_SURFACE2);
            }
        }
        static const char *const ln[4] = {"CAN", "RS485", "UART", "SVC"};
        int lw[4], tot = 0;
        for (int i = 0; i < 4; ++i) {
            lv_obj_t *tmp = hw_text(c2, ln[i], 0, 0, HF_10, HC_DIM);
            lw[i] = hw_width_of(tmp) + 14; tot += lw[i];
            lv_obj_delete(tmp);
        }
        const int cg = (174 - tot) / 3, cy = 7 + 16 + 4 * g2 + 54 + 1;
        int cx = 13;
        for (int i = 0; i < 4; ++i) {
            const bool on = rd && ((m->running_links >> i) & 1u);
            hw_chip_at(c2, cx, cy, false, ln[i], on ? HC_NAV_CUR : HC_DIM, on ? HC_NAV_BOX : HC_SURFACE2);
            cx += lw[i] + cg;
        }
    } else if (device_tab == HMI_DEV_LINK) {
        lv_obj_t *c1 = hw_card_box(p, 0, ry, 233, ch, HC_CARD, 10, HC_LINE);
        const bool on = m->online && ctx->link_started;
        const uint32_t c = on ? HC_OK_COL : HC_OFF_COL;
        hw_box(c1, 13, 7 + 7, 7, 7, c, 4);
        hw_text_in(c1, on ? "ONLINE" : "OFFLINE", 26, 7, 80, 20, HF_14B, c, LV_TEXT_ALIGN_LEFT);
        hp_fmt_age(t, sizeof(t), m->reply_age_ms); snprintf(b, sizeof(b), "Reply %s ago", t);
        hw_text_in(c1, b, 110, 7, 110, 20, HF_10, HC_LABEL, LV_TEXT_ALIGN_RIGHT);
        const int y0 = ch - 7 - 54;
        snprintf(b, sizeof(b), "%" PRIu32, m->timeouts); hw_kv(c1, 13, y0, 98, 18, "Timeouts", b, HC_WHITE);
        snprintf(b, sizeof(b), "%" PRIu32, m->rx_bad_frames); hw_kv(c1, 13, y0 + 18, 98, 18, "Bad frames", b, HC_WHITE);
        snprintf(b, sizeof(b), "%" PRIu32, m->event_missed); hw_kv(c1, 13, y0 + 36, 98, 18, "Missed ev.", b, HC_WHITE);
        hp_fmt_age(t, sizeof(t), m->status_age_ms); hw_kv(c1, 122, y0, 98, 18, "Status age", t, HC_WHITE);
        hp_fmt_age(t, sizeof(t), m->measurement_age_ms); hw_kv(c1, 122, y0 + 18, 98, 18, "Meas. age", t, HC_WHITE);
        hp_fmt_age(t, sizeof(t), m->faults_age_ms); hw_kv(c1, 122, y0 + 36, 98, 18, "Fault age", t, HC_WHITE);

        lv_obj_t *c2 = hw_card_box(p, 241, ry, 167, ch, HC_CARD, 10, HC_LINE);
        hw_section(c2, 13, 7, 141, "HMI PORT", NULL, 0);
        const int g = (ih - 16 - 54) / 3;
        snprintf(b, sizeof(b), "UART%d", RCC_LINK_UART_PORT);
        hw_kv(c2, 13, 7 + 16 + g, 141, 18, "Bus", b, HC_WHITE);
        snprintf(b, sizeof(b), "%d 8N1", RCC_LINK_BAUD);
        hw_kv(c2, 13, 7 + 16 + 2 * g + 18, 141, 18, "Baud", b, HC_WHITE);
        snprintf(b, sizeof(b), "GPIO%d / %d", RCC_LINK_TX_GPIO, RCC_LINK_RX_GPIO);
        hw_kv(c2, 13, 7 + 16 + 3 * g + 36, 141, 18, "Pins", b, HC_WHITE);
    } else {
        const hmi_settings_t *cfg = hmi_settings_get();

        /* Card 1: display and sleep. Rows are packed so everything fits the shortest layout (ch = 162). */
        lv_obj_t *c1 = hw_card_box(p, 0, ry, 200, ch, HC_CARD, 10, HC_LINE);
        hw_section(c1, 13, 7, 174, "DISPLAY & POWER", NULL, 0);

        char br_label[32];
        snprintf(br_label, sizeof(br_label), "Backlight: %u%%", cfg->brightness_pct);
        hw_text_in(c1, br_label, 13, 24, 174, 16, HF_12, HC_LABEL, LV_TEXT_ALIGN_LEFT);

        static const char *const br_names[4] = {"25%", "50%", "75%", "100%"};
        for (int i = 0; i < 4; ++i) {
            const bool sel = hp_brightness_preset_selected(cfg->brightness_pct, i);
            lv_obj_t *bt = hw_button(c1, 13 + i * 44, 42, 40, 22, sel ? HC_NAV_BOX : HC_SURFACE2, 6,
                                     set_brightness_cb, (void *)(uintptr_t)hp_brightness_presets[i]);
            if (sel) {
                lv_obj_set_style_border_width(bt, 1, 0);
                lv_obj_set_style_border_color(bt, HC(HC_NAV_CUR), 0);
            }
            lv_obj_t *l = hw_text_in(bt, br_names[i], 0, 0, 40, 22, HF_10, sel ? HC_NAV_CUR : HC_WHITE, LV_TEXT_ALIGN_CENTER);
            lv_obj_set_clickable(l, false);
        }

        char sl_label[32];
        if (cfg->sleep_timeout_s == 0) snprintf(sl_label, sizeof(sl_label), "Sleep timeout: off");
        else snprintf(sl_label, sizeof(sl_label), "Sleep timeout: %u min", (unsigned)(cfg->sleep_timeout_s / 60));
        hw_text_in(c1, sl_label, 13, 70, 174, 16, HF_12, HC_LABEL, LV_TEXT_ALIGN_LEFT);

        static const uint16_t sl_presets[4] = {0, 60, 180, 300};
        static const char *const sl_names[4] = {"OFF", "1m", "3m", "5m"};
        for (int i = 0; i < 4; ++i) {
            const bool sel = (cfg->sleep_timeout_s == sl_presets[i]);
            lv_obj_t *bt = hw_button(c1, 13 + i * 44, 88, 40, 22, sel ? HC_NAV_BOX : HC_SURFACE2, 6,
                                     set_sleep_cb, (void *)(uintptr_t)sl_presets[i]);
            if (sel) {
                lv_obj_set_style_border_width(bt, 1, 0);
                lv_obj_set_style_border_color(bt, HC(HC_NAV_CUR), 0);
            }
            lv_obj_t *l = hw_text_in(bt, sl_names[i], 0, 0, 40, 22, HF_10, sel ? HC_NAV_CUR : HC_WHITE, LV_TEXT_ALIGN_CENTER);
            lv_obj_set_clickable(l, false);
        }

        char dim[24]; snprintf(dim, sizeof(dim), "%u%%", (unsigned)cfg->dim_pct);
        hw_kv(c1, 13, 118, 174, 18, "Idle dim level", dim, HC_WHITE);
        hw_kv(c1, 13, 136, 174, 18, "Wake method", "Touch", HC_WHITE);

        /* Card 2: language and system info */
        lv_obj_t *c2 = hw_card_box(p, 208, ry, 200, ch, HC_CARD, 10, HC_LINE);
        hw_section(c2, 13, 7, 100, "SYSTEM", NULL, 0);
        hw_chip(c2, 187, 7, "ACTIVE", HC_OK_COL, HC_OK_BG);

        hw_text_in(c2, "Language", 13, 24, 174, 16, HF_12, HC_LABEL, LV_TEXT_ALIGN_LEFT);
        static const char *const lang_names[2] = {"English", "Ti\xE1\xBA\xBFng Vi\xE1\xBB\x87t"};
        for (int i = 0; i < 2; ++i) {
            const bool sel = (cfg->language == (uint8_t)i);
            const int lw = 84;
            lv_obj_t *bt = hw_button(c2, 13 + i * (lw + 6), 42, lw, 22, sel ? HC_NAV_BOX : HC_SURFACE2, 6,
                                     set_lang_cb, (void *)(uintptr_t)i);
            if (sel) {
                lv_obj_set_style_border_width(bt, 1, 0);
                lv_obj_set_style_border_color(bt, HC(HC_NAV_CUR), 0);
            }
            lv_obj_t *l = hw_text_in(bt, lang_names[i], 0, 0, lw, 22, HF_10, sel ? HC_NAV_CUR : HC_WHITE, LV_TEXT_ALIGN_CENTER);
            lv_obj_set_clickable(l, false);
        }

        snprintf(b, sizeof(b), "%u.%u s", (unsigned)(HMI_TWDT_TIMEOUT_MS / 1000), (unsigned)(HMI_TWDT_TIMEOUT_MS % 1000 / 100));
        hw_kv(c2, 13, 74, 174, 18, "Task watchdog", b, HC_WHITE);
        hw_kv(c2, 13, 92, 174, 18, "LCD driver", "ST77922 QSPI", HC_WHITE);
        hmi_ota_status_t ota;
        hmi_ota_get_status(&ota);
        hw_kv(c2, 13, 110, 174, 18, "Firmware", ota.running_version[0] ? ota.running_version : "unknown", HC_OK_COL);

        static hmi_wifi_status_t ws;   /* ~0.5 KB: kept off the LVGL task stack */
        hmi_wifi_get(&ws);
        lv_obj_t *wr = hw_button(c2, 13, 132, 174, 22, HC_SURFACE2, 6, wifi_open_cb, NULL);
        hw_icon_row(wr, &hmi_icon_wifi_13, 8, 0, 22, HC_NAV_CUR);
        hw_text_in(wr, "Wi-Fi", 28, 0, 40, 22, HF_10, HC_WHITE, LV_TEXT_ALIGN_LEFT);
        const char *wst = ws.state == HMI_WIFI_UNAVAILABLE ? "Unavailable" :
                          ws.state == HMI_WIFI_CONNECTED ? ws.ssid : ws.state == HMI_WIFI_CONNECTING ? "Connecting..." : "Not connected";
        hw_text_in(wr, wst, 66, 0, 84, 22, HF_10, ws.state == HMI_WIFI_CONNECTED ? HC_OK_COL : HC_LABEL, LV_TEXT_ALIGN_RIGHT);
        hw_icon_row(wr, &hmi_icon_chev_10, 156, 0, 22, HC_DIM);
    }
}
