/**
 * @file hmi_overview_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "hmi_overview_gen.h"
#include "../lvgl_ui.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/***********************
 *  STATIC VARIABLES
 **********************/

/***********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * hmi_overview_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_CHECK_COMPILE_TARGET(LVGL_UI_TARGET_ALL)
    if (lvgl_ui_check_target(LVGL_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
        lv_obj_set_name_static(lv_obj_0, "hmi_overview_#");
        lv_obj_set_style_radius(lv_obj_0, 0, 0);
        lv_obj_set_style_border_width(lv_obj_0, 0, 0);
        lv_obj_set_style_bg_opa(lv_obj_0, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(lv_obj_0, lv_color_hex(0x090d14), 0);
        lv_obj_set_style_pad_top(lv_obj_0, 0, 0);
        lv_obj_set_style_pad_bottom(lv_obj_0, 0, 0);
        lv_obj_set_style_pad_left(lv_obj_0, 0, 0);
        lv_obj_set_style_pad_right(lv_obj_0, 0, 0);
        lv_obj_set_scrollable(lv_obj_0, false);

        lv_obj_t * ui_obj = lv_obj_create(lv_obj_0);
        lv_obj_set_x(ui_obj, 0);
        lv_obj_set_y(ui_obj, 0);
        lv_obj_set_width(ui_obj, 480);
        lv_obj_set_height(ui_obj, 320);
        lv_obj_set_clickable(ui_obj, false);
        lv_obj_set_style_radius(ui_obj, 0, 0);
        lv_obj_set_style_border_width(ui_obj, 0, 0);
        lv_obj_set_style_bg_opa(ui_obj, (255 * 0 / 100), 0);
        lv_obj_set_style_pad_top(ui_obj, 0, 0);
        lv_obj_set_style_pad_bottom(ui_obj, 0, 0);
        lv_obj_set_style_pad_left(ui_obj, 0, 0);
        lv_obj_set_style_pad_right(ui_obj, 0, 0);
        lv_obj_set_scrollable(ui_obj, false);
        lv_obj_set_name(ui_obj, "ui_obj");
        lv_obj_t * ui_obj_2 = lv_obj_create(ui_obj);
        lv_obj_set_x(ui_obj_2, 64);
        lv_obj_set_y(ui_obj_2, 34);
        lv_obj_set_width(ui_obj_2, 408);
        lv_obj_set_height(ui_obj_2, 230);
        lv_obj_set_clickable(ui_obj_2, false);
        lv_obj_set_style_radius(ui_obj_2, 0, 0);
        lv_obj_set_style_border_width(ui_obj_2, 0, 0);
        lv_obj_set_style_bg_opa(ui_obj_2, (255 * 0 / 100), 0);
        lv_obj_set_style_pad_top(ui_obj_2, 0, 0);
        lv_obj_set_style_pad_bottom(ui_obj_2, 0, 0);
        lv_obj_set_style_pad_left(ui_obj_2, 0, 0);
        lv_obj_set_style_pad_right(ui_obj_2, 0, 0);
        lv_obj_set_scrollable(ui_obj_2, false);
        lv_obj_set_name(ui_obj_2, "ui_obj_2");
        lv_obj_t * ui_charging = rcc_overview_hero_create(ui_obj_2, 74, hmi_hero28, hmi_f10, "CHARGING", "OPERATIONAL · SESSION #17", 186);
        lv_obj_set_name(ui_charging, "ui_charging");
        lv_obj_t * ui_charging_steps = rcc_overview_hero_get_steps(ui_charging);
        if (ui_charging_steps) {
        rcc_overview_step_create(ui_charging_steps, 13, 48, 0, "IDLE", lv_color_hex(0xcbd5e1), lv_color_hex(0x22c55e), 0, hmi_f10);

        rcc_overview_connector_create(ui_charging_steps, 64, 24, lv_color_hex(0x1e4963));

        rcc_overview_step_create(ui_charging_steps, 91, 45, 0, "PRE", lv_color_hex(0xcbd5e1), lv_color_hex(0x22c55e), 0, hmi_f10);

        rcc_overview_connector_create(ui_charging_steps, 139, 24, lv_color_hex(0x1e4963));

        rcc_overview_step_create(ui_charging_steps, 166, 59, 0, "RELAY", lv_color_hex(0xcbd5e1), lv_color_hex(0x22c55e), 0, hmi_f10);

        rcc_overview_connector_create(ui_charging_steps, 228, 24, lv_color_hex(0x1e4963));

        rcc_overview_step_create(ui_charging_steps, 255, 61, 0, "VERIFY", lv_color_hex(0xcbd5e1), lv_color_hex(0x22c55e), 0, hmi_f10);

        rcc_overview_connector_create(ui_charging_steps, 319, 24, lv_color_hex(0x1e4963));

        rcc_overview_step_create(ui_charging_steps, 346, 49, 1, "CHG", lv_color_hex(0x38bdf8), lv_color_hex(0x38bdf8), 255, hmi_f10);
        } else {
        LV_LOG_WARN("`steps` slot of `ui_charging` doesn't exist");
        }

        lv_obj_t * ui_uart = rcc_overview_system_create(ui_obj_2, 80, 150, hmi_f10, hmi_f12b, hmi_icon_link_13, hmi_icon_check_12, hmi_icon_relay_13, hmi_icon_clock_13);
        lv_obj_set_name(ui_uart, "ui_uart");
        lv_obj_t * ui_uart_uart_hit = rcc_overview_system_get_uart_hit(ui_uart);
        if (ui_uart_uart_hit) {
        lv_obj_add_screen_create_event(ui_uart_uart_hit, LV_EVENT_CLICKED, hmi_device_link_create, LV_SCREEN_LOAD_ANIM_NONE, 0, 0);
        } else {
        LV_LOG_WARN("`uart_hit` slot of `ui_uart` doesn't exist");
        }

        lv_obj_t * ui_uart_fault_hit = rcc_overview_system_get_fault_hit(ui_uart);
        if (ui_uart_fault_hit) {
        lv_obj_add_screen_create_event(ui_uart_fault_hit, LV_EVENT_CLICKED, hmi_faults_create, LV_SCREEN_LOAD_ANIM_NONE, 0, 0);
        } else {
        LV_LOG_WARN("`fault_hit` slot of `ui_uart` doesn't exist");
        }

        lv_obj_t * ui_uart_session_hit = rcc_overview_system_get_session_hit(ui_uart);
        if (ui_uart_session_hit) {
        lv_obj_add_screen_create_event(ui_uart_session_hit, LV_EVENT_CLICKED, hmi_session_create, LV_SCREEN_LOAD_ANIM_NONE, 0, 0);
        } else {
        LV_LOG_WARN("`session_hit` slot of `ui_uart` doesn't exist");
        }

        lv_obj_t * ui_volt = rcc_telemetry_card_create(ui_obj_2, 164, 80, 150, "VOLT", "V", lv_color_hex(0xfbbf24), "48.2", "80%/60V", 47, 60, lv_color_hex(0xf8fafc), false, false, hmi_f10, hmi_f22b, hmi_f12b, hmi_icon_bolt_13);
        lv_obj_set_name(ui_volt, "ui_volt");
        lv_obj_add_screen_create_event(ui_volt, LV_EVENT_CLICKED, hmi_measure_create, LV_SCREEN_LOAD_ANIM_NONE, 0, 0);

        lv_obj_t * ui_curr = rcc_telemetry_card_create(ui_obj_2, 247, 80, 150, "CURR.", "A", lv_color_hex(0xfb923c), "8.0", "80%/10A", 47, 47, lv_color_hex(0xf8fafc), false, false, hmi_f10, hmi_f22b, hmi_f12b, hmi_icon_wave_13);
        lv_obj_set_name(ui_curr, "ui_curr");
        lv_obj_add_screen_create_event(ui_curr, LV_EVENT_CLICKED, hmi_measure_create, LV_SCREEN_LOAD_ANIM_NONE, 0, 0);

        lv_obj_t * ui_power = rcc_telemetry_card_create(ui_obj_2, 331, 80, 150, "POWER", "W", lv_color_hex(0x38bdf8), "386", "64%/600W", 37, 53, lv_color_hex(0xf8fafc), false, false, hmi_f10, hmi_f22b, hmi_f12b, hmi_icon_power_13);
        lv_obj_set_name(ui_power, "ui_power");
        lv_obj_add_screen_create_event(ui_power, LV_EVENT_CLICKED, hmi_measure_create, LV_SCREEN_LOAD_ANIM_NONE, 0, 0);

        lv_obj_t * ui_obj_26 = lv_obj_create(lv_obj_0);
        lv_obj_set_x(ui_obj_26, 0);
        lv_obj_set_y(ui_obj_26, 0);
        lv_obj_set_width(ui_obj_26, 56);
        lv_obj_set_height(ui_obj_26, 320);
        lv_obj_set_clickable(ui_obj_26, false);
        lv_obj_set_style_radius(ui_obj_26, 0, 0);
        lv_obj_set_style_border_width(ui_obj_26, 0, 0);
        lv_obj_set_style_bg_opa(ui_obj_26, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(ui_obj_26, lv_color_hex(0x06090e), 0);
        lv_obj_set_style_pad_top(ui_obj_26, 0, 0);
        lv_obj_set_style_pad_bottom(ui_obj_26, 0, 0);
        lv_obj_set_style_pad_left(ui_obj_26, 0, 0);
        lv_obj_set_style_pad_right(ui_obj_26, 0, 0);
        lv_obj_set_scrollable(ui_obj_26, false);
        lv_obj_set_name(ui_obj_26, "ui_obj_26");

        lv_obj_t * ui_obj_27 = lv_obj_create(lv_obj_0);
        lv_obj_set_x(ui_obj_27, 55);
        lv_obj_set_y(ui_obj_27, 0);
        lv_obj_set_width(ui_obj_27, 1);
        lv_obj_set_height(ui_obj_27, 320);
        lv_obj_set_clickable(ui_obj_27, false);
        lv_obj_set_style_radius(ui_obj_27, 0, 0);
        lv_obj_set_style_border_width(ui_obj_27, 0, 0);
        lv_obj_set_style_bg_opa(ui_obj_27, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(ui_obj_27, lv_color_hex(0x1e293b), 0);
        lv_obj_set_style_pad_top(ui_obj_27, 0, 0);
        lv_obj_set_style_pad_bottom(ui_obj_27, 0, 0);
        lv_obj_set_style_pad_left(ui_obj_27, 0, 0);
        lv_obj_set_style_pad_right(ui_obj_27, 0, 0);
        lv_obj_set_scrollable(ui_obj_27, false);
        lv_obj_set_name(ui_obj_27, "ui_obj_27");

        lv_obj_t * ui_overview = lv_button_create(lv_obj_0);
        lv_obj_set_x(ui_overview, 4);
        lv_obj_set_y(ui_overview, 4);
        lv_obj_set_width(ui_overview, 47);
        lv_obj_set_height(ui_overview, 60);
        lv_obj_set_clickable(ui_overview, true);
        lv_obj_set_style_radius(ui_overview, 10, 0);
        lv_obj_set_style_border_width(ui_overview, 0, 0);
        lv_obj_set_style_bg_opa(ui_overview, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(ui_overview, lv_color_hex(0x111722), 0);
        lv_obj_set_style_pad_top(ui_overview, 0, 0);
        lv_obj_set_style_pad_bottom(ui_overview, 0, 0);
        lv_obj_set_style_pad_left(ui_overview, 0, 0);
        lv_obj_set_style_pad_right(ui_overview, 0, 0);
        lv_obj_set_scrollable(ui_overview, false);
        lv_obj_set_name(ui_overview, "ui_overview");
        lv_obj_t * ui_home_19 = lv_image_create(ui_overview);
        lv_obj_set_x(ui_home_19, 14);
        lv_obj_set_y(ui_home_19, 12);
        lv_obj_set_width(ui_home_19, 19);
        lv_obj_set_height(ui_home_19, 19);
        lv_obj_set_clickable(ui_home_19, false);
        lv_obj_set_style_radius(ui_home_19, 0, 0);
        lv_obj_set_style_border_width(ui_home_19, 0, 0);
        lv_obj_set_style_bg_opa(ui_home_19, (255 * 0 / 100), 0);
        lv_obj_set_style_pad_top(ui_home_19, 0, 0);
        lv_obj_set_style_pad_bottom(ui_home_19, 0, 0);
        lv_obj_set_style_pad_left(ui_home_19, 0, 0);
        lv_obj_set_style_pad_right(ui_home_19, 0, 0);
        lv_obj_set_scrollable(ui_home_19, false);
        lv_image_set_src(ui_home_19, hmi_icon_home_19);
        lv_obj_set_style_image_recolor(ui_home_19, lv_color_hex(0x38bdf8), 0);
        lv_obj_set_style_image_recolor_opa(ui_home_19, (255 * 100 / 100), 0);
        lv_obj_set_name(ui_home_19, "ui_home_19");

        lv_obj_t * ui_overview_2 = lv_label_create(ui_overview);
        lv_obj_set_x(ui_overview_2, 0);
        lv_obj_set_y(ui_overview_2, 35);
        lv_obj_set_width(ui_overview_2, 48);
        lv_obj_set_height(ui_overview_2, 13);
        lv_obj_set_clickable(ui_overview_2, false);
        lv_obj_set_style_radius(ui_overview_2, 0, 0);
        lv_obj_set_style_border_width(ui_overview_2, 0, 0);
        lv_obj_set_style_bg_opa(ui_overview_2, (255 * 0 / 100), 0);
        lv_obj_set_style_text_font(ui_overview_2, hmi_f10, 0);
        lv_obj_set_style_text_color(ui_overview_2, lv_color_hex(0xf8fafc), 0);
        lv_obj_set_style_pad_top(ui_overview_2, 0, 0);
        lv_obj_set_style_pad_bottom(ui_overview_2, 0, 0);
        lv_obj_set_style_pad_left(ui_overview_2, 0, 0);
        lv_obj_set_style_pad_right(ui_overview_2, 0, 0);
        lv_obj_set_scrollable(ui_overview_2, false);
        lv_label_set_text(ui_overview_2, "Overview");
        lv_label_set_long_mode(ui_overview_2, LV_LABEL_LONG_MODE_WRAP);
        lv_obj_set_style_text_align(ui_overview_2, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_letter_space(ui_overview_2, 0, 0);
        lv_obj_set_name(ui_overview_2, "ui_overview_2");

        lv_obj_t * ui_obj_28 = lv_obj_create(lv_obj_0);
        lv_obj_set_x(ui_obj_28, 0);
        lv_obj_set_y(ui_obj_28, 18);
        lv_obj_set_width(ui_obj_28, 3);
        lv_obj_set_height(ui_obj_28, 32);
        lv_obj_set_clickable(ui_obj_28, false);
        lv_obj_set_style_radius(ui_obj_28, 2, 0);
        lv_obj_set_style_border_width(ui_obj_28, 0, 0);
        lv_obj_set_style_bg_opa(ui_obj_28, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(ui_obj_28, lv_color_hex(0x38bdf8), 0);
        lv_obj_set_style_pad_top(ui_obj_28, 0, 0);
        lv_obj_set_style_pad_bottom(ui_obj_28, 0, 0);
        lv_obj_set_style_pad_left(ui_obj_28, 0, 0);
        lv_obj_set_style_pad_right(ui_obj_28, 0, 0);
        lv_obj_set_scrollable(ui_obj_28, false);
        lv_obj_set_name(ui_obj_28, "ui_obj_28");

        lv_obj_t * ui_measure = lv_button_create(lv_obj_0);
        lv_obj_set_x(ui_measure, 4);
        lv_obj_set_y(ui_measure, 67);
        lv_obj_set_width(ui_measure, 47);
        lv_obj_set_height(ui_measure, 60);
        lv_obj_set_clickable(ui_measure, true);
        lv_obj_set_style_radius(ui_measure, 10, 0);
        lv_obj_set_style_border_width(ui_measure, 0, 0);
        lv_obj_set_style_bg_opa(ui_measure, (255 * 0 / 100), 0);
        lv_obj_set_style_pad_top(ui_measure, 0, 0);
        lv_obj_set_style_pad_bottom(ui_measure, 0, 0);
        lv_obj_set_style_pad_left(ui_measure, 0, 0);
        lv_obj_set_style_pad_right(ui_measure, 0, 0);
        lv_obj_set_scrollable(ui_measure, false);
        lv_obj_set_name(ui_measure, "ui_measure");
        lv_obj_t * ui_gauge_19 = lv_image_create(ui_measure);
        lv_obj_set_x(ui_gauge_19, 14);
        lv_obj_set_y(ui_gauge_19, 12);
        lv_obj_set_width(ui_gauge_19, 19);
        lv_obj_set_height(ui_gauge_19, 19);
        lv_obj_set_clickable(ui_gauge_19, false);
        lv_obj_set_style_radius(ui_gauge_19, 0, 0);
        lv_obj_set_style_border_width(ui_gauge_19, 0, 0);
        lv_obj_set_style_bg_opa(ui_gauge_19, (255 * 0 / 100), 0);
        lv_obj_set_style_pad_top(ui_gauge_19, 0, 0);
        lv_obj_set_style_pad_bottom(ui_gauge_19, 0, 0);
        lv_obj_set_style_pad_left(ui_gauge_19, 0, 0);
        lv_obj_set_style_pad_right(ui_gauge_19, 0, 0);
        lv_obj_set_scrollable(ui_gauge_19, false);
        lv_image_set_src(ui_gauge_19, hmi_icon_gauge_19);
        lv_obj_set_style_image_recolor(ui_gauge_19, lv_color_hex(0x64748b), 0);
        lv_obj_set_style_image_recolor_opa(ui_gauge_19, (255 * 100 / 100), 0);
        lv_obj_set_name(ui_gauge_19, "ui_gauge_19");

        lv_obj_t * ui_measure_2 = lv_label_create(ui_measure);
        lv_obj_set_x(ui_measure_2, 1);
        lv_obj_set_y(ui_measure_2, 35);
        lv_obj_set_width(ui_measure_2, 44);
        lv_obj_set_height(ui_measure_2, 13);
        lv_obj_set_clickable(ui_measure_2, false);
        lv_obj_set_style_radius(ui_measure_2, 0, 0);
        lv_obj_set_style_border_width(ui_measure_2, 0, 0);
        lv_obj_set_style_bg_opa(ui_measure_2, (255 * 0 / 100), 0);
        lv_obj_set_style_text_font(ui_measure_2, hmi_f10, 0);
        lv_obj_set_style_text_color(ui_measure_2, lv_color_hex(0x64748b), 0);
        lv_obj_set_style_pad_top(ui_measure_2, 0, 0);
        lv_obj_set_style_pad_bottom(ui_measure_2, 0, 0);
        lv_obj_set_style_pad_left(ui_measure_2, 0, 0);
        lv_obj_set_style_pad_right(ui_measure_2, 0, 0);
        lv_obj_set_scrollable(ui_measure_2, false);
        lv_label_set_text(ui_measure_2, "Measure");
        lv_label_set_long_mode(ui_measure_2, LV_LABEL_LONG_MODE_WRAP);
        lv_obj_set_style_text_align(ui_measure_2, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_letter_space(ui_measure_2, 0, 0);
        lv_obj_set_name(ui_measure_2, "ui_measure_2");

        lv_obj_add_screen_create_event(ui_measure, LV_EVENT_CLICKED, hmi_measure_create, LV_SCREEN_LOAD_ANIM_NONE, 0, 0);

        lv_obj_t * ui_faults = lv_button_create(lv_obj_0);
        lv_obj_set_x(ui_faults, 4);
        lv_obj_set_y(ui_faults, 130);
        lv_obj_set_width(ui_faults, 47);
        lv_obj_set_height(ui_faults, 60);
        lv_obj_set_clickable(ui_faults, true);
        lv_obj_set_style_radius(ui_faults, 10, 0);
        lv_obj_set_style_border_width(ui_faults, 0, 0);
        lv_obj_set_style_bg_opa(ui_faults, (255 * 0 / 100), 0);
        lv_obj_set_style_pad_top(ui_faults, 0, 0);
        lv_obj_set_style_pad_bottom(ui_faults, 0, 0);
        lv_obj_set_style_pad_left(ui_faults, 0, 0);
        lv_obj_set_style_pad_right(ui_faults, 0, 0);
        lv_obj_set_scrollable(ui_faults, false);
        lv_obj_set_name(ui_faults, "ui_faults");
        lv_obj_t * ui_warn_19 = lv_image_create(ui_faults);
        lv_obj_set_x(ui_warn_19, 14);
        lv_obj_set_y(ui_warn_19, 12);
        lv_obj_set_width(ui_warn_19, 19);
        lv_obj_set_height(ui_warn_19, 19);
        lv_obj_set_clickable(ui_warn_19, false);
        lv_obj_set_style_radius(ui_warn_19, 0, 0);
        lv_obj_set_style_border_width(ui_warn_19, 0, 0);
        lv_obj_set_style_bg_opa(ui_warn_19, (255 * 0 / 100), 0);
        lv_obj_set_style_pad_top(ui_warn_19, 0, 0);
        lv_obj_set_style_pad_bottom(ui_warn_19, 0, 0);
        lv_obj_set_style_pad_left(ui_warn_19, 0, 0);
        lv_obj_set_style_pad_right(ui_warn_19, 0, 0);
        lv_obj_set_scrollable(ui_warn_19, false);
        lv_image_set_src(ui_warn_19, hmi_icon_warn_19);
        lv_obj_set_style_image_recolor(ui_warn_19, lv_color_hex(0x64748b), 0);
        lv_obj_set_style_image_recolor_opa(ui_warn_19, (255 * 100 / 100), 0);
        lv_obj_set_name(ui_warn_19, "ui_warn_19");

        lv_obj_t * ui_faults_2 = lv_label_create(ui_faults);
        lv_obj_set_x(ui_faults_2, 8);
        lv_obj_set_y(ui_faults_2, 35);
        lv_obj_set_width(ui_faults_2, 31);
        lv_obj_set_height(ui_faults_2, 13);
        lv_obj_set_clickable(ui_faults_2, false);
        lv_obj_set_style_radius(ui_faults_2, 0, 0);
        lv_obj_set_style_border_width(ui_faults_2, 0, 0);
        lv_obj_set_style_bg_opa(ui_faults_2, (255 * 0 / 100), 0);
        lv_obj_set_style_text_font(ui_faults_2, hmi_f10, 0);
        lv_obj_set_style_text_color(ui_faults_2, lv_color_hex(0x64748b), 0);
        lv_obj_set_style_pad_top(ui_faults_2, 0, 0);
        lv_obj_set_style_pad_bottom(ui_faults_2, 0, 0);
        lv_obj_set_style_pad_left(ui_faults_2, 0, 0);
        lv_obj_set_style_pad_right(ui_faults_2, 0, 0);
        lv_obj_set_scrollable(ui_faults_2, false);
        lv_label_set_text(ui_faults_2, "Faults");
        lv_label_set_long_mode(ui_faults_2, LV_LABEL_LONG_MODE_WRAP);
        lv_obj_set_style_text_align(ui_faults_2, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_letter_space(ui_faults_2, 0, 0);
        lv_obj_set_name(ui_faults_2, "ui_faults_2");

        lv_obj_add_screen_create_event(ui_faults, LV_EVENT_CLICKED, hmi_faults_create, LV_SCREEN_LOAD_ANIM_NONE, 0, 0);

        lv_obj_t * ui_session = lv_button_create(lv_obj_0);
        lv_obj_set_x(ui_session, 4);
        lv_obj_set_y(ui_session, 193);
        lv_obj_set_width(ui_session, 47);
        lv_obj_set_height(ui_session, 60);
        lv_obj_set_clickable(ui_session, true);
        lv_obj_set_style_radius(ui_session, 10, 0);
        lv_obj_set_style_border_width(ui_session, 0, 0);
        lv_obj_set_style_bg_opa(ui_session, (255 * 0 / 100), 0);
        lv_obj_set_style_pad_top(ui_session, 0, 0);
        lv_obj_set_style_pad_bottom(ui_session, 0, 0);
        lv_obj_set_style_pad_left(ui_session, 0, 0);
        lv_obj_set_style_pad_right(ui_session, 0, 0);
        lv_obj_set_scrollable(ui_session, false);
        lv_obj_set_name(ui_session, "ui_session");
        lv_obj_t * ui_list_19 = lv_image_create(ui_session);
        lv_obj_set_x(ui_list_19, 14);
        lv_obj_set_y(ui_list_19, 12);
        lv_obj_set_width(ui_list_19, 19);
        lv_obj_set_height(ui_list_19, 19);
        lv_obj_set_clickable(ui_list_19, false);
        lv_obj_set_style_radius(ui_list_19, 0, 0);
        lv_obj_set_style_border_width(ui_list_19, 0, 0);
        lv_obj_set_style_bg_opa(ui_list_19, (255 * 0 / 100), 0);
        lv_obj_set_style_pad_top(ui_list_19, 0, 0);
        lv_obj_set_style_pad_bottom(ui_list_19, 0, 0);
        lv_obj_set_style_pad_left(ui_list_19, 0, 0);
        lv_obj_set_style_pad_right(ui_list_19, 0, 0);
        lv_obj_set_scrollable(ui_list_19, false);
        lv_image_set_src(ui_list_19, hmi_icon_list_19);
        lv_obj_set_style_image_recolor(ui_list_19, lv_color_hex(0x64748b), 0);
        lv_obj_set_style_image_recolor_opa(ui_list_19, (255 * 100 / 100), 0);
        lv_obj_set_name(ui_list_19, "ui_list_19");

        lv_obj_t * ui_session_2 = lv_label_create(ui_session);
        lv_obj_set_x(ui_session_2, 4);
        lv_obj_set_y(ui_session_2, 35);
        lv_obj_set_width(ui_session_2, 38);
        lv_obj_set_height(ui_session_2, 13);
        lv_obj_set_clickable(ui_session_2, false);
        lv_obj_set_style_radius(ui_session_2, 0, 0);
        lv_obj_set_style_border_width(ui_session_2, 0, 0);
        lv_obj_set_style_bg_opa(ui_session_2, (255 * 0 / 100), 0);
        lv_obj_set_style_text_font(ui_session_2, hmi_f10, 0);
        lv_obj_set_style_text_color(ui_session_2, lv_color_hex(0x64748b), 0);
        lv_obj_set_style_pad_top(ui_session_2, 0, 0);
        lv_obj_set_style_pad_bottom(ui_session_2, 0, 0);
        lv_obj_set_style_pad_left(ui_session_2, 0, 0);
        lv_obj_set_style_pad_right(ui_session_2, 0, 0);
        lv_obj_set_scrollable(ui_session_2, false);
        lv_label_set_text(ui_session_2, "Session");
        lv_label_set_long_mode(ui_session_2, LV_LABEL_LONG_MODE_WRAP);
        lv_obj_set_style_text_align(ui_session_2, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_letter_space(ui_session_2, 0, 0);
        lv_obj_set_name(ui_session_2, "ui_session_2");

        lv_obj_add_screen_create_event(ui_session, LV_EVENT_CLICKED, hmi_session_create, LV_SCREEN_LOAD_ANIM_NONE, 0, 0);

        lv_obj_t * ui_device = lv_button_create(lv_obj_0);
        lv_obj_set_x(ui_device, 4);
        lv_obj_set_y(ui_device, 256);
        lv_obj_set_width(ui_device, 47);
        lv_obj_set_height(ui_device, 60);
        lv_obj_set_clickable(ui_device, true);
        lv_obj_set_style_radius(ui_device, 10, 0);
        lv_obj_set_style_border_width(ui_device, 0, 0);
        lv_obj_set_style_bg_opa(ui_device, (255 * 0 / 100), 0);
        lv_obj_set_style_pad_top(ui_device, 0, 0);
        lv_obj_set_style_pad_bottom(ui_device, 0, 0);
        lv_obj_set_style_pad_left(ui_device, 0, 0);
        lv_obj_set_style_pad_right(ui_device, 0, 0);
        lv_obj_set_scrollable(ui_device, false);
        lv_obj_set_name(ui_device, "ui_device");
        lv_obj_t * ui_chip_19 = lv_image_create(ui_device);
        lv_obj_set_x(ui_chip_19, 14);
        lv_obj_set_y(ui_chip_19, 12);
        lv_obj_set_width(ui_chip_19, 19);
        lv_obj_set_height(ui_chip_19, 19);
        lv_obj_set_clickable(ui_chip_19, false);
        lv_obj_set_style_radius(ui_chip_19, 0, 0);
        lv_obj_set_style_border_width(ui_chip_19, 0, 0);
        lv_obj_set_style_bg_opa(ui_chip_19, (255 * 0 / 100), 0);
        lv_obj_set_style_pad_top(ui_chip_19, 0, 0);
        lv_obj_set_style_pad_bottom(ui_chip_19, 0, 0);
        lv_obj_set_style_pad_left(ui_chip_19, 0, 0);
        lv_obj_set_style_pad_right(ui_chip_19, 0, 0);
        lv_obj_set_scrollable(ui_chip_19, false);
        lv_image_set_src(ui_chip_19, hmi_icon_chip_19);
        lv_obj_set_style_image_recolor(ui_chip_19, lv_color_hex(0x64748b), 0);
        lv_obj_set_style_image_recolor_opa(ui_chip_19, (255 * 100 / 100), 0);
        lv_obj_set_name(ui_chip_19, "ui_chip_19");

        lv_obj_t * ui_device_2 = lv_label_create(ui_device);
        lv_obj_set_x(ui_device_2, 6);
        lv_obj_set_y(ui_device_2, 35);
        lv_obj_set_width(ui_device_2, 35);
        lv_obj_set_height(ui_device_2, 13);
        lv_obj_set_clickable(ui_device_2, false);
        lv_obj_set_style_radius(ui_device_2, 0, 0);
        lv_obj_set_style_border_width(ui_device_2, 0, 0);
        lv_obj_set_style_bg_opa(ui_device_2, (255 * 0 / 100), 0);
        lv_obj_set_style_text_font(ui_device_2, hmi_f10, 0);
        lv_obj_set_style_text_color(ui_device_2, lv_color_hex(0x64748b), 0);
        lv_obj_set_style_pad_top(ui_device_2, 0, 0);
        lv_obj_set_style_pad_bottom(ui_device_2, 0, 0);
        lv_obj_set_style_pad_left(ui_device_2, 0, 0);
        lv_obj_set_style_pad_right(ui_device_2, 0, 0);
        lv_obj_set_scrollable(ui_device_2, false);
        lv_label_set_text(ui_device_2, "Device");
        lv_label_set_long_mode(ui_device_2, LV_LABEL_LONG_MODE_WRAP);
        lv_obj_set_style_text_align(ui_device_2, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_letter_space(ui_device_2, 0, 0);
        lv_obj_set_name(ui_device_2, "ui_device_2");

        lv_obj_add_screen_create_event(ui_device, LV_EVENT_CLICKED, hmi_device_create, LV_SCREEN_LOAD_ANIM_NONE, 0, 0);

        lv_obj_t * ui_node = lv_button_create(lv_obj_0);
        lv_obj_set_x(ui_node, 64);
        lv_obj_set_y(ui_node, 4);
        lv_obj_set_width(ui_node, 99);
        lv_obj_set_height(ui_node, 24);
        lv_obj_set_clickable(ui_node, true);
        lv_obj_set_style_radius(ui_node, 6, 0);
        lv_obj_set_style_border_width(ui_node, 0, 0);
        lv_obj_set_style_bg_opa(ui_node, (255 * 0 / 100), 0);
        lv_obj_set_style_pad_top(ui_node, 0, 0);
        lv_obj_set_style_pad_bottom(ui_node, 0, 0);
        lv_obj_set_style_pad_left(ui_node, 0, 0);
        lv_obj_set_style_pad_right(ui_node, 0, 0);
        lv_obj_set_scrollable(ui_node, false);
        lv_obj_set_name(ui_node, "ui_node");
        lv_obj_t * ui_node_2 = lv_label_create(ui_node);
        lv_obj_set_x(ui_node_2, 0);
        lv_obj_set_y(ui_node_2, 5);
        lv_obj_set_width(ui_node_2, 41);
        lv_obj_set_height(ui_node_2, 13);
        lv_obj_set_clickable(ui_node_2, false);
        lv_obj_set_style_radius(ui_node_2, 0, 0);
        lv_obj_set_style_border_width(ui_node_2, 0, 0);
        lv_obj_set_style_bg_opa(ui_node_2, (255 * 0 / 100), 0);
        lv_obj_set_style_text_font(ui_node_2, hmi_f12b, 0);
        lv_obj_set_style_text_color(ui_node_2, lv_color_hex(0x94a3b8), 0);
        lv_obj_set_style_pad_top(ui_node_2, 0, 0);
        lv_obj_set_style_pad_bottom(ui_node_2, 0, 0);
        lv_obj_set_style_pad_left(ui_node_2, 0, 0);
        lv_obj_set_style_pad_right(ui_node_2, 0, 0);
        lv_obj_set_scrollable(ui_node_2, false);
        lv_label_set_text(ui_node_2, "NODE");
        lv_label_set_long_mode(ui_node_2, LV_LABEL_LONG_MODE_WRAP);
        lv_obj_set_style_text_align(ui_node_2, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_letter_space(ui_node_2, 1, 0);
        lv_obj_set_name(ui_node_2, "ui_node_2");

        lv_obj_t * ui_rcc_01 = lv_label_create(ui_node);
        lv_obj_set_x(ui_rcc_01, 49);
        lv_obj_set_y(ui_rcc_01, 5);
        lv_obj_set_width(ui_rcc_01, 50);
        lv_obj_set_height(ui_rcc_01, 13);
        lv_obj_set_clickable(ui_rcc_01, false);
        lv_obj_set_style_radius(ui_rcc_01, 0, 0);
        lv_obj_set_style_border_width(ui_rcc_01, 0, 0);
        lv_obj_set_style_bg_opa(ui_rcc_01, (255 * 0 / 100), 0);
        lv_obj_set_style_text_font(ui_rcc_01, hmi_f12b, 0);
        lv_obj_set_style_text_color(ui_rcc_01, lv_color_hex(0xcbd5e1), 0);
        lv_obj_set_style_pad_top(ui_rcc_01, 0, 0);
        lv_obj_set_style_pad_bottom(ui_rcc_01, 0, 0);
        lv_obj_set_style_pad_left(ui_rcc_01, 0, 0);
        lv_obj_set_style_pad_right(ui_rcc_01, 0, 0);
        lv_obj_set_scrollable(ui_rcc_01, false);
        lv_label_set_text(ui_rcc_01, "RCC-01");
        lv_label_set_long_mode(ui_rcc_01, LV_LABEL_LONG_MODE_WRAP);
        lv_obj_set_style_text_align(ui_rcc_01, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_letter_space(ui_rcc_01, 1, 0);
        lv_obj_set_name(ui_rcc_01, "ui_rcc_01");

        lv_obj_t * ui_online_0_4_s = lv_button_create(lv_obj_0);
        lv_obj_set_x(ui_online_0_4_s, 239);
        lv_obj_set_y(ui_online_0_4_s, 5);
        lv_obj_set_width(ui_online_0_4_s, 122);
        lv_obj_set_height(ui_online_0_4_s, 23);
        lv_obj_set_clickable(ui_online_0_4_s, true);
        lv_obj_set_style_radius(ui_online_0_4_s, 10, 0);
        lv_obj_set_style_border_width(ui_online_0_4_s, 1, 0);
        lv_obj_set_style_bg_opa(ui_online_0_4_s, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(ui_online_0_4_s, lv_color_hex(0x111722), 0);
        lv_obj_set_style_border_color(ui_online_0_4_s, lv_color_hex(0x1e293b), 0);
        lv_obj_set_style_pad_top(ui_online_0_4_s, -1, 0);
        lv_obj_set_style_pad_bottom(ui_online_0_4_s, -1, 0);
        lv_obj_set_style_pad_left(ui_online_0_4_s, -1, 0);
        lv_obj_set_style_pad_right(ui_online_0_4_s, -1, 0);
        lv_obj_set_scrollable(ui_online_0_4_s, false);
        lv_obj_set_name(ui_online_0_4_s, "ui_online_0_4_s");
        lv_obj_t * ui_obj_29 = lv_obj_create(ui_online_0_4_s);
        lv_obj_set_x(ui_obj_29, 11);
        lv_obj_set_y(ui_obj_29, 8);
        lv_obj_set_width(ui_obj_29, 7);
        lv_obj_set_height(ui_obj_29, 7);
        lv_obj_set_clickable(ui_obj_29, false);
        lv_obj_set_style_radius(ui_obj_29, 4, 0);
        lv_obj_set_style_border_width(ui_obj_29, 0, 0);
        lv_obj_set_style_bg_opa(ui_obj_29, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(ui_obj_29, lv_color_hex(0x22c55e), 0);
        lv_obj_set_style_pad_top(ui_obj_29, 0, 0);
        lv_obj_set_style_pad_bottom(ui_obj_29, 0, 0);
        lv_obj_set_style_pad_left(ui_obj_29, 0, 0);
        lv_obj_set_style_pad_right(ui_obj_29, 0, 0);
        lv_obj_set_scrollable(ui_obj_29, false);
        lv_obj_set_name(ui_obj_29, "ui_obj_29");

        lv_obj_t * ui_online_0_4_s_2 = lv_label_create(ui_online_0_4_s);
        lv_obj_set_x(ui_online_0_4_s_2, 24);
        lv_obj_set_y(ui_online_0_4_s_2, 5);
        lv_obj_set_width(ui_online_0_4_s_2, 87);
        lv_obj_set_height(ui_online_0_4_s_2, 13);
        lv_obj_set_clickable(ui_online_0_4_s_2, false);
        lv_obj_set_style_radius(ui_online_0_4_s_2, 0, 0);
        lv_obj_set_style_border_width(ui_online_0_4_s_2, 0, 0);
        lv_obj_set_style_bg_opa(ui_online_0_4_s_2, (255 * 0 / 100), 0);
        lv_obj_set_style_text_font(ui_online_0_4_s_2, hmi_f12b, 0);
        lv_obj_set_style_text_color(ui_online_0_4_s_2, lv_color_hex(0x22c55e), 0);
        lv_obj_set_style_pad_top(ui_online_0_4_s_2, 0, 0);
        lv_obj_set_style_pad_bottom(ui_online_0_4_s_2, 0, 0);
        lv_obj_set_style_pad_left(ui_online_0_4_s_2, 0, 0);
        lv_obj_set_style_pad_right(ui_online_0_4_s_2, 0, 0);
        lv_obj_set_scrollable(ui_online_0_4_s_2, false);
        lv_label_set_text(ui_online_0_4_s_2, "ONLINE · 0.4 s");
        lv_label_set_long_mode(ui_online_0_4_s_2, LV_LABEL_LONG_MODE_WRAP);
        lv_obj_set_style_text_align(ui_online_0_4_s_2, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_letter_space(ui_online_0_4_s_2, 0, 0);
        lv_obj_set_name(ui_online_0_4_s_2, "ui_online_0_4_s_2");

        lv_obj_t * ui_button = lv_button_create(lv_obj_0);
        lv_obj_set_x(ui_button, 438);
        lv_obj_set_y(ui_button, 4);
        lv_obj_set_width(ui_button, 34);
        lv_obj_set_height(ui_button, 24);
        lv_obj_set_clickable(ui_button, true);
        lv_obj_set_style_radius(ui_button, 8, 0);
        lv_obj_set_style_border_width(ui_button, 1, 0);
        lv_obj_set_style_bg_opa(ui_button, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(ui_button, lv_color_hex(0x111722), 0);
        lv_obj_set_style_border_color(ui_button, lv_color_hex(0x1e293b), 0);
        lv_obj_set_style_pad_top(ui_button, -1, 0);
        lv_obj_set_style_pad_bottom(ui_button, -1, 0);
        lv_obj_set_style_pad_left(ui_button, -1, 0);
        lv_obj_set_style_pad_right(ui_button, -1, 0);
        lv_obj_set_scrollable(ui_button, false);
        lv_obj_set_name(ui_button, "ui_button");
        lv_obj_t * ui_checkc_15 = lv_image_create(ui_button);
        lv_obj_set_x(ui_checkc_15, 9);
        lv_obj_set_y(ui_checkc_15, 5);
        lv_obj_set_width(ui_checkc_15, 15);
        lv_obj_set_height(ui_checkc_15, 15);
        lv_obj_set_clickable(ui_checkc_15, false);
        lv_obj_set_style_radius(ui_checkc_15, 0, 0);
        lv_obj_set_style_border_width(ui_checkc_15, 0, 0);
        lv_obj_set_style_bg_opa(ui_checkc_15, (255 * 0 / 100), 0);
        lv_obj_set_style_pad_top(ui_checkc_15, 0, 0);
        lv_obj_set_style_pad_bottom(ui_checkc_15, 0, 0);
        lv_obj_set_style_pad_left(ui_checkc_15, 0, 0);
        lv_obj_set_style_pad_right(ui_checkc_15, 0, 0);
        lv_obj_set_scrollable(ui_checkc_15, false);
        lv_image_set_src(ui_checkc_15, hmi_icon_checkc_15);
        lv_obj_set_style_image_recolor(ui_checkc_15, lv_color_hex(0x22c55e), 0);
        lv_obj_set_style_image_recolor_opa(ui_checkc_15, (255 * 100 / 100), 0);
        lv_obj_set_name(ui_checkc_15, "ui_checkc_15");

        lv_obj_t * ui_obj_30 = lv_obj_create(lv_obj_0);
        lv_obj_set_x(ui_obj_30, 64);
        lv_obj_set_y(ui_obj_30, 270);
        lv_obj_set_width(ui_obj_30, 408);
        lv_obj_set_height(ui_obj_30, 42);
        lv_obj_set_clickable(ui_obj_30, false);
        lv_obj_set_style_radius(ui_obj_30, 0, 0);
        lv_obj_set_style_border_width(ui_obj_30, 0, 0);
        lv_obj_set_style_bg_opa(ui_obj_30, (255 * 0 / 100), 0);
        lv_obj_set_style_pad_top(ui_obj_30, 0, 0);
        lv_obj_set_style_pad_bottom(ui_obj_30, 0, 0);
        lv_obj_set_style_pad_left(ui_obj_30, 0, 0);
        lv_obj_set_style_pad_right(ui_obj_30, 0, 0);
        lv_obj_set_scrollable(ui_obj_30, false);
        lv_obj_set_name(ui_obj_30, "ui_obj_30");
        lv_obj_t * ui_stop = lv_button_create(ui_obj_30);
        lv_obj_set_x(ui_stop, 0);
        lv_obj_set_y(ui_stop, 0);
        lv_obj_set_width(ui_stop, 408);
        lv_obj_set_height(ui_stop, 42);
        lv_obj_set_clickable(ui_stop, true);
        lv_obj_set_style_radius(ui_stop, 10, 0);
        lv_obj_set_style_border_width(ui_stop, 1, 0);
        lv_obj_set_style_bg_opa(ui_stop, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_color(ui_stop, lv_color_hex(0xdc2626), 0);
        lv_obj_set_style_border_color(ui_stop, lv_color_hex(0xffffff), 0);
        lv_obj_set_style_pad_top(ui_stop, 0, 0);
        lv_obj_set_style_pad_bottom(ui_stop, 0, 0);
        lv_obj_set_style_pad_left(ui_stop, 0, 0);
        lv_obj_set_style_pad_right(ui_stop, 0, 0);
        lv_obj_set_scrollable(ui_stop, false);
        lv_obj_set_name(ui_stop, "ui_stop");
        lv_obj_t * ui_stop_2 = lv_label_create(ui_stop);
        lv_obj_set_x(ui_stop_2, 191);
        lv_obj_set_y(ui_stop_2, 12);
        lv_obj_set_width(ui_stop_2, 49);
        lv_obj_set_height(ui_stop_2, 17);
        lv_obj_set_clickable(ui_stop_2, false);
        lv_obj_set_style_radius(ui_stop_2, 0, 0);
        lv_obj_set_style_border_width(ui_stop_2, 0, 0);
        lv_obj_set_style_bg_opa(ui_stop_2, (255 * 0 / 100), 0);
        lv_obj_set_style_text_font(ui_stop_2, hmi_f16b, 0);
        lv_obj_set_style_text_color(ui_stop_2, lv_color_hex(0xffffff), 0);
        lv_obj_set_style_pad_top(ui_stop_2, 0, 0);
        lv_obj_set_style_pad_bottom(ui_stop_2, 0, 0);
        lv_obj_set_style_pad_left(ui_stop_2, 0, 0);
        lv_obj_set_style_pad_right(ui_stop_2, 0, 0);
        lv_obj_set_scrollable(ui_stop_2, false);
        lv_label_set_text(ui_stop_2, "STOP");
        lv_label_set_long_mode(ui_stop_2, LV_LABEL_LONG_MODE_WRAP);
        lv_obj_set_style_text_align(ui_stop_2, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_letter_space(ui_stop_2, 1, 0);
        lv_obj_set_name(ui_stop_2, "ui_stop_2");

        lv_obj_t * ui_stop_16 = lv_image_create(ui_stop);
        lv_obj_set_x(ui_stop_16, 167);
        lv_obj_set_y(ui_stop_16, 13);
        lv_obj_set_width(ui_stop_16, 16);
        lv_obj_set_height(ui_stop_16, 16);
        lv_obj_set_clickable(ui_stop_16, false);
        lv_obj_set_style_radius(ui_stop_16, 0, 0);
        lv_obj_set_style_border_width(ui_stop_16, 0, 0);
        lv_obj_set_style_bg_opa(ui_stop_16, (255 * 0 / 100), 0);
        lv_obj_set_style_pad_top(ui_stop_16, 0, 0);
        lv_obj_set_style_pad_bottom(ui_stop_16, 0, 0);
        lv_obj_set_style_pad_left(ui_stop_16, 0, 0);
        lv_obj_set_style_pad_right(ui_stop_16, 0, 0);
        lv_obj_set_scrollable(ui_stop_16, false);
        lv_image_set_src(ui_stop_16, hmi_icon_stop_16);
        lv_obj_set_style_image_recolor(ui_stop_16, lv_color_hex(0xffffff), 0);
        lv_obj_set_style_image_recolor_opa(ui_stop_16, (255 * 100 / 100), 0);
        lv_obj_set_name(ui_stop_16, "ui_stop_16");

        lv_obj_add_screen_create_event(ui_stop, LV_EVENT_CLICKED, hmi_command_stopping_create, LV_SCREEN_LOAD_ANIM_NONE, 0, 0);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

