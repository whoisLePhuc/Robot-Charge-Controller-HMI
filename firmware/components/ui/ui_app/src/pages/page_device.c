/*
 * Device page: Controller / Comm link / Settings tabs (display and sleep presets, language, Wi-Fi entry).
 */
#include <inttypes.h>
#include <stdio.h>

#include "hmi_power.h"
#include "hmi_config.h"
#include "hmi_icons.h"
#include "hmi_lvgl_headers.h"
#include "rcc_device_banner_gen.h"
#include "rcc_device_comm_gen.h"
#include "rcc_device_controller_gen.h"
#include "rcc_device_settings_gen.h"
#include "rcc_device_tabs_gen.h"
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
    hmi_power_set_brightness((uint8_t)(uintptr_t)lv_event_get_user_data(e));
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

static lv_obj_t *widget(lv_obj_t *root, const char *name)
{
    lv_obj_t *obj = lv_obj_find_by_name(root, name);
    LV_ASSERT_NULL(obj);
    return obj;
}

static void text(lv_obj_t *root, const char *name, const char *value)
{
    hw_label_set(widget(root, name), value);
}

static void selected(lv_obj_t *button, bool on, uint32_t inactive, uint32_t edge, uint32_t inactive_text)
{
    lv_obj_set_style_bg_color(button, HC(on ? HC_NAV_BOX : inactive), 0);
    lv_obj_set_style_border_width(button, on ? 1 : 0, 0);
    lv_obj_set_style_border_color(button, HC(edge), 0);
    lv_obj_set_style_text_color(lv_obj_get_child(button, 0), HC(on ? HC_NAV_CUR : inactive_text), 0);
}

typedef struct {
    lv_obj_t *box;
    lv_obj_t *label;
} device_chip_t;

static device_chip_t find_chip(lv_obj_t *root, const char *name)
{
    char id[32];
    snprintf(id, sizeof(id), "%s_box", name);
    lv_obj_t *box = widget(root, id);
    snprintf(id, sizeof(id), "%s_text", name);
    return (device_chip_t){.box = box, .label = widget(root, id)};
}

static int chip_color_and_width(device_chip_t chip, const char *value, uint32_t foreground, uint32_t background)
{
    hw_label_set(chip.label, value);
    lv_obj_set_style_text_color(chip.label, HC(foreground), 0);
    lv_obj_set_style_bg_color(chip.box, HC(background), 0);
    return hw_width_of(chip.label) + 14;
}

static void place_chip(device_chip_t chip, int x, int width)
{
    lv_obj_set_x(chip.box, x);
    lv_obj_set_width(chip.box, width);
    lv_obj_set_x(chip.label, x + 7);
}

static void right_chip(device_chip_t chip, const char *value, uint32_t foreground, uint32_t background)
{
    const int width = chip_color_and_width(chip, value, foreground, background);
    const int right = lv_obj_get_x_aligned(chip.box) + lv_obj_get_width(chip.box);
    place_chip(chip, right - width, width);
}

static void build_banner(lv_obj_t *parent, const rcc_snapshot_t *m, bool rd)
{
    lv_obj_t *banner = rcc_device_banner_create(parent, (lv_font_t *)HF_10, &hmi_icon_warn_13);
    lv_obj_t *title = widget(banner, "device_banner_title");
    lv_obj_t *icon = widget(banner, "device_banner_icon");
    if (!rd) {
        lv_obj_set_style_bg_color(banner, HC(HC_OFF_BG), 0);
        lv_obj_set_style_border_color(banner, HC(HC_OFF_COL), 0);
        lv_obj_set_style_text_color(title, HC(HC_OFF_COL), 0);
        lv_obj_set_x(title, lv_obj_get_x_aligned(icon));
        lv_obj_set_hidden(icon, true);
    }
    hw_label_set(title, rd ? ((m->build_flags & 1u) ? "BENCH IMAGE — NOT A PRODUCT BUILD" : "PRODUCT BUILD")
                           : "CONTROLLER INFO NOT READ");
    text(banner, "device_banner_status", rd ? ((m->build_flags & 2u) ? "" : "Relay locked") : "Link lost");
}

static void build_tabs(lv_obj_t *parent, int tab)
{
    lv_obj_t *tabs = rcc_device_tabs_create(parent, (lv_font_t *)HF_10);
    for (int i = 0; i < 3; ++i) {
        char name[16];
        snprintf(name, sizeof(name), "device_tab%d", i);
        lv_obj_t *button = widget(tabs, name);
        selected(button, i == tab, HC_CARD, HC_NAV_EDGE, HC_DIM);
        lv_obj_add_event_cb(button, tab_click, LV_EVENT_CLICKED, (void *)(uintptr_t)i);
    }
}

static void build_controller(lv_obj_t *parent, const rcc_snapshot_t *m, int height, bool rd)
{
    lv_obj_t *root = rcc_device_controller_create(parent, height, (lv_font_t *)HF_10,
                                                  (lv_font_t *)HF_12, (lv_font_t *)HF_12B);
    const hp_fw_compat_t fw = hp_fw_compat(m);
    right_chip(find_chip(root, "device_compat"),
               fw == HP_FW_UNREAD ? "UNREAD" : fw == HP_FW_COMPATIBLE ? "COMPATIBLE" : "INCOMPAT.",
               fw == HP_FW_UNREAD ? HC_OFF_COL : fw == HP_FW_COMPATIBLE ? HC_OK_COL : HC_FAULT_COL,
               fw == HP_FW_UNREAD ? HC_SURFACE2 : fw == HP_FW_COMPATIBLE ? HC_OK_BG : HC_FAULT_BG);

    static const char *const field_names[] = {"device_product", "device_revision", "device_image", "device_boot"};
    char values[4][24];
    if (rd) {
        snprintf(values[0], sizeof(values[0]), "0x%04X · v%u", (unsigned)m->product_id, (unsigned)m->protocol_version);
        snprintf(values[1], sizeof(values[1]), "0x%02" PRIX32 " / 0x%02" PRIX32, m->hardware_revision, m->firmware_revision);
        snprintf(values[2], sizeof(values[2]), "%02X%02X…%02X%02X", m->image_id[0], m->image_id[1], m->image_id[6], m->image_id[7]);
        snprintf(values[3], sizeof(values[3]), "…%04X", (unsigned)(m->boot_id & 0xffffu));
    }
    for (int i = 0; i < 4; ++i) text(root, field_names[i], rd ? values[i] : "—");

    /* Build-flag bit 2 is reserved. Show control authority from GET_STATUS in the last row. */
    static const char *const interface_names[5] = {"NONE", "CAN", "RS485", "UART", "SVC"};
    for (int i = 0; i < 3; ++i) {
        char name[24];
        snprintf(name, sizeof(name), "device_flag%d", i);
        device_chip_t chip = find_chip(root, name);
        snprintf(name, sizeof(name), "device_flag%d_unknown", i);
        const bool known = rd && (i < 2 || m->status_valid);
        lv_obj_set_hidden(widget(root, name), known);
        lv_obj_set_hidden(chip.box, !known);
        lv_obj_set_hidden(chip.label, !known);
        if (!known) continue;
        if (i == 2) {
            const char *who = m->is_control ? "THIS HMI"
                              : m->control_interface < 5u ? interface_names[m->control_interface] : "?";
            right_chip(chip, who, m->is_control ? HC_OK_COL : HC_LABEL, HC_SURFACE2);
        } else {
            const bool on = (m->build_flags >> i) & 1u;
            right_chip(chip, on ? "ON" : "OFF", on ? HC_WARN_COL : HC_LABEL, on ? HC_WARN_BG : HC_SURFACE2);
        }
    }

    static const char *const bus_names[] = {"CAN", "RS485", "UART", "SVC"};
    device_chip_t bus[4];
    int widths[4], total = 0;
    for (int i = 0; i < 4; ++i) {
        char name[16];
        snprintf(name, sizeof(name), "device_bus%d", i);
        bus[i] = find_chip(root, name);
        const bool on = rd && ((m->running_links >> i) & 1u);
        widths[i] = chip_color_and_width(bus[i], bus_names[i], on ? HC_NAV_CUR : HC_DIM, on ? HC_NAV_BOX : HC_SURFACE2);
        total += widths[i];
    }
    int x = lv_obj_get_x_aligned(bus[0].box);
    const int right = lv_obj_get_x_aligned(bus[3].box) + lv_obj_get_width(bus[3].box);
    const int gap = (right - x - total) / 3;
    for (int i = 0; i < 4; ++i) {
        place_chip(bus[i], x, widths[i]);
        x += widths[i] + gap;
    }
}

static void build_comm(lv_obj_t *parent, const rcc_snapshot_t *m, int height, bool link_started)
{
    lv_obj_t *root = rcc_device_comm_create(parent, height, (lv_font_t *)HF_10, (lv_font_t *)HF_12,
                                            (lv_font_t *)HF_12B, (lv_font_t *)HF_14B);
    const bool on = m->online && link_started;
    const uint32_t color = on ? HC_OK_COL : HC_OFF_COL;
    lv_obj_set_style_bg_color(widget(root, "device_comm_dot"), HC(color), 0);
    lv_obj_t *state = widget(root, "device_comm_state");
    hw_label_set(state, on ? "ONLINE" : "OFFLINE");
    lv_obj_set_style_text_color(state, HC(color), 0);

    char value[64], age[24];
    hp_fmt_age(age, sizeof(age), m->reply_age_ms);
    snprintf(value, sizeof(value), "Reply %s ago", age);
    text(root, "device_comm_reply", value);
    snprintf(value, sizeof(value), "%" PRIu32, m->timeouts);
    text(root, "device_timeouts", value);
    snprintf(value, sizeof(value), "%" PRIu32, m->rx_bad_frames);
    text(root, "device_bad_frames", value);
    snprintf(value, sizeof(value), "%" PRIu32, m->event_missed);
    text(root, "device_missed", value);
    hp_fmt_age(age, sizeof(age), m->status_age_ms);
    text(root, "device_status_age", age);
    hp_fmt_age(age, sizeof(age), m->measurement_age_ms);
    text(root, "device_measure_age", age);
    hp_fmt_age(age, sizeof(age), m->faults_age_ms);
    text(root, "device_fault_age", age);
    snprintf(value, sizeof(value), "UART%d", RCC_LINK_UART_PORT);
    text(root, "device_uart", value);
    snprintf(value, sizeof(value), "%d 8N1", RCC_LINK_BAUD);
    text(root, "device_baud", value);
    snprintf(value, sizeof(value), "GPIO%d / %d", RCC_LINK_TX_GPIO, RCC_LINK_RX_GPIO);
    text(root, "device_pins", value);
}

static void build_settings(lv_obj_t *parent, int height)
{
    lv_obj_t *root = rcc_device_settings_create(parent, height, (lv_font_t *)HF_10, (lv_font_t *)HF_12,
                                                (lv_font_t *)HF_12B, &hmi_icon_wifi_13, &hmi_icon_chev_10);
    const hmi_settings_t *cfg = hmi_settings_get();
    static const uint16_t sleep_presets[] = {0, 60, 180, 300};
    char value[40], name[24];
    snprintf(value, sizeof(value), "Backlight: %u%%", cfg->brightness_pct);
    text(root, "device_backlight", value);
    if (cfg->sleep_timeout_s == 0) snprintf(value, sizeof(value), "Sleep timeout: off");
    else snprintf(value, sizeof(value), "Sleep timeout: %u min", (unsigned)(cfg->sleep_timeout_s / 60));
    text(root, "device_sleep", value);
    for (int i = 0; i < 4; ++i) {
        snprintf(name, sizeof(name), "device_brightness%d", i);
        lv_obj_t *button = widget(root, name);
        selected(button, hp_brightness_preset_selected(cfg->brightness_pct, i), HC_SURFACE2, HC_NAV_CUR, HC_WHITE);
        lv_obj_add_event_cb(button, set_brightness_cb, LV_EVENT_CLICKED, (void *)(uintptr_t)hp_brightness_presets[i]);

        snprintf(name, sizeof(name), "device_timeout%d", i);
        button = widget(root, name);
        selected(button, cfg->sleep_timeout_s == sleep_presets[i], HC_SURFACE2, HC_NAV_CUR, HC_WHITE);
        lv_obj_add_event_cb(button, set_sleep_cb, LV_EVENT_CLICKED, (void *)(uintptr_t)sleep_presets[i]);
    }
    snprintf(value, sizeof(value), "%u%%", (unsigned)cfg->dim_pct);
    text(root, "device_dim", value);
    right_chip(find_chip(root, "device_system"), "ACTIVE", HC_OK_COL, HC_OK_BG);
    for (int i = 0; i < 2; ++i) {
        snprintf(name, sizeof(name), "device_language%d", i);
        lv_obj_t *button = widget(root, name);
        selected(button, cfg->language == (uint8_t)i, HC_SURFACE2, HC_NAV_CUR, HC_WHITE);
        lv_obj_add_event_cb(button, set_lang_cb, LV_EVENT_CLICKED, (void *)(uintptr_t)i);
    }
    snprintf(value, sizeof(value), "%u.%u s", (unsigned)(HMI_TWDT_TIMEOUT_MS / 1000),
             (unsigned)(HMI_TWDT_TIMEOUT_MS % 1000 / 100));
    text(root, "device_watchdog", value);
    hmi_ota_status_t ota;
    hmi_ota_get_status(&ota);
    text(root, "device_version", ota.running_version[0] ? ota.running_version : "unknown");

    static hmi_wifi_status_t ws;   /* ~0.5 KB held off the LVGL task stack */
    hmi_wifi_get(&ws);
    lv_obj_t *wifi = widget(root, "device_wifi_button");
    lv_obj_add_event_cb(wifi, wifi_open_cb, LV_EVENT_CLICKED, NULL);
    const char *wst = ws.state == HMI_WIFI_UNAVAILABLE ? "Unavailable"
                      : ws.state == HMI_WIFI_CONNECTED ? ws.ssid
                      : ws.state == HMI_WIFI_CONNECTING ? "Connecting..." : "Not connected";
    lv_obj_t *status = widget(wifi, "device_wifi_status");
    hw_label_set(status, wst);
    lv_obj_set_style_text_color(status, HC(ws.state == HMI_WIFI_CONNECTED ? HC_OK_COL : HC_LABEL), 0);
}

void page_device_build(lv_obj_t *p, const rcc_snapshot_t *m, const hmi_page_ctx_t *ctx)
{
    const bool rd = m->online && m->device_valid;
    const int height = ctx->height - 68;
    build_banner(p, m, rd);
    build_tabs(p, cur_tab);
    if (cur_tab == HMI_DEV_CONTROLLER) build_controller(p, m, height, rd);
    else if (cur_tab == HMI_DEV_LINK) build_comm(p, m, height, ctx->link_started);
    else build_settings(p, height);
}
