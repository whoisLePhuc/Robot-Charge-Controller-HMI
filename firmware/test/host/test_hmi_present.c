/* Host tests for ui/hmi_present.c (no LVGL, no ESP-IDF). Snapshots mirror tools/sim/sim_scenarios.c. */
#include <stdio.h>
#include <string.h>

#include "hmi_present.h"

static int fails, checks;
#define CHECK(c) do { ++checks; if (!(c)) { ++fails; printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #c); } } while (0)
#define CHECK_STR(a, b) do { ++checks; if (strcmp((a), (b))) { ++fails; printf("FAIL %s:%d: \"%s\" != \"%s\"\n", __FILE__, __LINE__, (a), (b)); } } while (0)

static void base(rcc_snapshot_t *m)
{
    memset(m, 0, sizeof(*m));
    m->online = m->device_valid = m->status_valid = m->measurement_valid = m->faults_valid = true;
    m->reply_age_ms = 400; m->status_age_ms = 400; m->measurement_age_ms = 400; m->faults_age_ms = 800;
    m->vout_status = m->iout_status = 0x3f;
    m->command_age_ms = UINT32_MAX;
}
static void idle(rcc_snapshot_t *m) { base(m); m->top_state = 4; m->operational_state = 1; }
static void charging(rcc_snapshot_t *m)
{
    base(m); m->top_state = 4; m->operational_state = 6; m->charging_established = true;
    m->relay_command = 1; m->session_id = 17; m->vout_mv = 48200; m->iout_ma = 8000;
    m->last_command = 0x20; m->command_state = RCC_COMMAND_COMPLETED; m->command_age_ms = 14000;
}
static void fault(rcc_snapshot_t *m)
{
    base(m); m->top_state = 6; m->primary_fault = 0x0402; m->active_fault_mask = 1u << 11;
}
static void offline(rcc_snapshot_t *m) { idle(m); m->online = false; m->reply_age_ms = 12000; }
static void inhibited(rcc_snapshot_t *m) { base(m); m->top_state = 5; m->inhibit_mask = 0x2; }
static void noauth(rcc_snapshot_t *m)
{
    idle(m); m->last_command = 0x20; m->command_state = RCC_COMMAND_REJECTED;
    m->command_reason = 0x0005; m->command_age_ms = 2000;
}

static void test_names(void)
{
    char b[64];
    CHECK_STR(hp_fault_name(0x0402), "IOUT_OVERCURRENT");
    CHECK_STR(hp_fault_name(0), "none");
    CHECK_STR(hp_fault_name(0x7777), "UNRECOGNIZED");
    CHECK_STR(hp_fault_short(0x0604), "DIAG DEGR");
    CHECK_STR(hp_fault_short(0x7777), "FAULT");
    CHECK_STR(hp_fault_hint(0x0302), "check the current-sense circuit");
    CHECK_STR(hp_fault_hint(0x0101), "check the controller service log");
    CHECK_STR(hp_state_name(4, 6), "CHARGING");
    CHECK_STR(hp_state_name(4, 99), "OPERATIONAL");
    CHECK_STR(hp_state_name(6, 0), "LATCHED_FAULT");
    CHECK_STR(hp_state_name(9, 0), "UNKNOWN");
    hp_mask_text(b, sizeof(b), 0, hp_inhibit_names, HP_INHIBIT_COUNT); CHECK_STR(b, "none");
    hp_mask_text(b, sizeof(b), 0x5, hp_inhibit_names, HP_INHIBIT_COUNT); CHECK_STR(b, "REMOTE+RECOVERY");
    hp_mask_text(b, sizeof(b), 0x82, hp_inhibit_names, HP_INHIBIT_COUNT); CHECK_STR(b, "RESET+bit 7");
    CHECK_STR(hp_reason_text(0x0009, b, sizeof(b)), "INHIBITED");
    CHECK_STR(hp_reason_text(0x0123, b, sizeof(b)), "0x0123");
    CHECK_STR(hp_event_name(0x0014), "CHARGE_ESTABLISHED");
    CHECK_STR(hp_event_name(0x9999), "EVENT");
}

static void test_events(void)
{
    char b[64];
    const uint32_t boot[4] = {3, 0, 0, 0};
    hp_event_detail(b, sizeof(b), 0x0001, boot); CHECK_STR(b, "reset: WATCHDOG");
    const uint32_t st[4] = {4 | (1u << 8) | (4u << 16) | (6u << 24), 0, 0, 0};
    hp_event_detail(b, sizeof(b), 0x0010, st); CHECK_STR(b, "IDLE \xE2\x86\x92 CHARGING");
    const uint32_t fl[4] = {1, 0, 0x0402, 0};
    hp_event_detail(b, sizeof(b), 0x0012, fl); CHECK_STR(b, "active \xE2\x86\x92 IOUT_OVERCURRENT");
    const uint32_t est[4] = {0, 48200, (uint32_t)-1500, 0};
    hp_event_detail(b, sizeof(b), 0x0014, est); CHECK_STR(b, "48.2 V \xC2\xB7 \xE2\x88\x92" "1.5 A");
    const uint32_t end[4] = {0, 1, 3723000, 0};
    hp_event_detail(b, sizeof(b), 0x0015, end); CHECK_STR(b, "COMPLETE \xC2\xB7 1h 02m");
    const uint32_t bad[4] = {0, 42, 5000, 0};
    hp_event_detail(b, sizeof(b), 0x0016, bad); CHECK_STR(b, "? \xC2\xB7 5s");
    hp_event_detail(b, sizeof(b), 0x0040, bad); CHECK_STR(b, "");
}

static void test_fmt(void)
{
    char b[32];
    hp_fmt_age(b, sizeof(b), UINT32_MAX); CHECK_STR(b, "\xE2\x80\x94");
    hp_fmt_age(b, sizeof(b), 0); CHECK_STR(b, "0.0 s");
    hp_fmt_age(b, sizeof(b), 9999); CHECK_STR(b, "9.9 s");
    hp_fmt_age(b, sizeof(b), 10000); CHECK_STR(b, "10 s");
    hp_fmt_age(b, sizeof(b), 599999); CHECK_STR(b, "599 s");
    hp_fmt_age(b, sizeof(b), 600000); CHECK_STR(b, "10 min");
    hp_fmt_milli(b, sizeof(b), 48250, false); CHECK_STR(b, "48.2");
    hp_fmt_milli(b, sizeof(b), 0, false); CHECK_STR(b, "0.0");
    hp_fmt_milli(b, sizeof(b), -1550, false); CHECK_STR(b, "\xE2\x88\x92" "1.5");
    hp_fmt_milli(b, sizeof(b), 3400, true); CHECK_STR(b, "\xE2\x89\xA4 3.4");
    hp_fmt_milli(b, sizeof(b), INT32_MIN, false); CHECK_STR(b, "\xE2\x88\x92" "2147483.6");
    hp_fmt_duration(b, sizeof(b), 999); CHECK_STR(b, "0s");
    hp_fmt_duration(b, sizeof(b), 59000); CHECK_STR(b, "59s");
    hp_fmt_duration(b, sizeof(b), 60000); CHECK_STR(b, "1m 00s");
    hp_fmt_duration(b, sizeof(b), 3599000); CHECK_STR(b, "59m 59s");
    hp_fmt_duration(b, sizeof(b), 3600000); CHECK_STR(b, "1h 00m");
    hp_fmt_since_boot(b, sizeof(b), 119000000ull); CHECK_STR(b, "+119s");
    hp_fmt_since_boot(b, sizeof(b), 120000000ull); CHECK_STR(b, "+2m");
    hp_fmt_since_boot(b, sizeof(b), 7200000000ull); CHECK_STR(b, "+2h");
}

static void test_derived(void)
{
    rcc_snapshot_t m;
    idle(&m);
    CHECK(hp_fresh(&m)); CHECK(hp_start_allowed(&m)); CHECK(hp_fault_state(&m) == HP_FS_OK);
    CHECK(hp_v_valid(&m)); CHECK(!hp_v_floor(&m)); CHECK(hp_i_valid(&m)); CHECK(!hp_meas_stale(&m));
    m.status_age_ms = 1500; CHECK(!hp_fresh(&m)); CHECK(!hp_start_allowed(&m));
    idle(&m); m.simulated_measurements = true; CHECK(!hp_start_allowed(&m));
    m.simulated_ready = true; CHECK(hp_start_allowed(&m));
    idle(&m); m.command_state = RCC_COMMAND_SENDING; CHECK(!hp_start_allowed(&m));
    idle(&m); m.vout_status = 0x7f; CHECK(hp_v_floor(&m));
    m.vout_status = 0x3e; CHECK(!hp_v_valid(&m)); CHECK(!hp_v_floor(&m));
    idle(&m); m.measurement_age_ms = 1501; CHECK(hp_meas_stale(&m)); CHECK(!hp_i_valid(&m));
    charging(&m);
    CHECK(!hp_start_allowed(&m)); CHECK(hp_in_session(&m)); CHECK(hp_fault_state(&m) == HP_FS_OK);
    fault(&m); CHECK(hp_fault_state(&m) == HP_FS_FAULT); CHECK(!hp_start_allowed(&m));
    inhibited(&m); CHECK(hp_fault_state(&m) == HP_FS_INHIBIT); CHECK(hp_start_allowed(&m));
    inhibited(&m); m.inhibit_mask = 0; CHECK(hp_fault_state(&m) == HP_FS_INHIBIT);
    offline(&m); CHECK(hp_fault_state(&m) == HP_FS_UNKNOWN); CHECK(!hp_start_allowed(&m)); CHECK(!hp_in_session(&m));
    idle(&m); m.faults_valid = false; CHECK(hp_fault_state(&m) == HP_FS_UNKNOWN);
    noauth(&m); CHECK(hp_no_authority(&m));
}

static void test_state_info(void)
{
    rcc_snapshot_t m; hp_state_info_t s;
    idle(&m); hp_state_info(&m, &s);
    CHECK_STR(s.name, "IDLE"); CHECK_STR(s.sub, "OPERATIONAL \xC2\xB7 READY"); CHECK(s.kind == HP_K_NORMAL); CHECK(s.stage == 0);
    charging(&m); hp_state_info(&m, &s);
    CHECK_STR(s.name, "CHARGING"); CHECK_STR(s.sub, "OPERATIONAL \xC2\xB7 SESSION #17"); CHECK(s.stage == 4);
    m.charging_established = false; hp_state_info(&m, &s);
    CHECK_STR(s.name, "NOT_CONFIRMED"); CHECK_STR(s.sub, "CHARGING NOT ESTABLISHED"); CHECK(s.kind == HP_K_WARN); CHECK(s.stage == 3);
    fault(&m); hp_state_info(&m, &s);
    CHECK_STR(s.name, "LATCHED_FAULT"); CHECK_STR(s.sub, "FAULT 0x0402"); CHECK(s.kind == HP_K_FAULT); CHECK(s.stage == -1);
    inhibited(&m); hp_state_info(&m, &s);
    CHECK_STR(s.name, "INHIBITED"); CHECK_STR(s.sub, "RESET"); CHECK(s.kind == HP_K_WARN);
    m.primary_fault = 0x0601; hp_state_info(&m, &s); CHECK_STR(s.sub, "RESET \xC2\xB7 FAULT CAN_DEGRADED");
    m.inhibit_mask = 0; m.primary_fault = 0; hp_state_info(&m, &s); CHECK_STR(s.sub, "INHIBITED");
    offline(&m); hp_state_info(&m, &s);
    CHECK_STR(s.name, "UNKNOWN"); CHECK_STR(s.sub, "STATE NOT CONFIRMED"); CHECK(s.kind == HP_K_OFF);
    base(&m); m.top_state = 2; hp_state_info(&m, &s); CHECK(s.kind == HP_K_WARN); CHECK_STR(s.name, "SERVICE_LOCK");
}

static void test_bar(void)
{
    rcc_snapshot_t m; hp_bar_spec_t b;
    idle(&m);
    hp_bar_spec(&m, true, &b); CHECK(b.kind == HP_BAR_FULL); CHECK(b.button == HP_B_START);
    hp_bar_spec(&m, false, &b); CHECK(b.kind == HP_BAR_NONE); CHECK(b.button == HP_B_NONE);
    charging(&m);   /* START completed: falls through to the steady bar */
    hp_bar_spec(&m, true, &b); CHECK(b.kind == HP_BAR_FULL); CHECK(b.button == HP_B_STOP);
    hp_bar_spec(&m, false, &b); CHECK(b.kind == HP_BAR_SPLIT); CHECK(b.button == HP_B_STOP);
    CHECK_STR(b.title, "CHARGING"); CHECK_STR(b.sub, "Session #17 \xC2\xB7 relay commanded closed"); CHECK(b.title_kind == HP_K_NORMAL);
    fault(&m);
    hp_bar_spec(&m, false, &b); CHECK(b.kind == HP_BAR_SPLIT); CHECK(b.title_kind == HP_K_FAULT);
    CHECK_STR(b.sub, "IOUT_OVERCURRENT \xC2\xB7 service needed");
    offline(&m);
    hp_bar_spec(&m, true, &b); CHECK(b.kind == HP_BAR_FULL); CHECK(b.button == HP_B_STOP);
    hp_bar_spec(&m, false, &b); CHECK_STR(b.title, "UNKNOWN"); CHECK_STR(b.sub, "Charging may still be in progress");
    CHECK(b.title_kind == HP_K_OFF);
    inhibited(&m);
    hp_bar_spec(&m, true, &b); CHECK(b.kind == HP_BAR_FULL); CHECK(b.button == HP_B_START);
    noauth(&m);
    hp_bar_spec(&m, true, &b); CHECK(b.kind == HP_BAR_NOAUTH);
    m.command_age_ms = 15000; hp_bar_spec(&m, true, &b); CHECK(b.kind == HP_BAR_FULL);
    idle(&m); m.last_command = 0x21; m.command_state = RCC_COMMAND_SENDING; m.command_age_ms = 100;
    hp_bar_spec(&m, true, &b); CHECK(b.kind == HP_BAR_SPLIT); CHECK(b.button == HP_B_STOPPING); CHECK(b.title_kind == HP_K_ACCENT);
    m.command_state = RCC_COMMAND_COMPLETED;
    hp_bar_spec(&m, true, &b); CHECK(b.kind == HP_BAR_SPLIT); CHECK(b.button == HP_B_START); CHECK_STR(b.title, "Stopped by command");
    hp_bar_spec(&m, false, &b); CHECK(b.kind == HP_BAR_MSG); CHECK(b.button == HP_B_NONE);
    m.command_state = RCC_COMMAND_REJECTED; m.command_reason = 0x000A;
    hp_bar_spec(&m, true, &b); CHECK(b.title_kind == HP_K_WARN); CHECK_STR(b.sub, "FAULT_ACTIVE");
    idle(&m); m.last_command = 0x20; m.command_state = RCC_COMMAND_REJECTED; m.command_reason = 0x0099; m.command_age_ms = 100;
    hp_bar_spec(&m, true, &b); CHECK_STR(b.title, "START rejected"); CHECK_STR(b.sub, "0x0099"); CHECK(b.button == HP_B_START);
}

static void test_banner_session(void)
{
    rcc_snapshot_t m; hp_banner_t b; char s[48];
    idle(&m);
    hp_banner(&m, false, &b); CHECK(b.kind == HP_K_FAULT); CHECK(b.target == HP_TGT_NONE);
    hp_banner(&m, true, &b); CHECK(b.text == NULL);
    offline(&m); hp_banner(&m, true, &b); CHECK(b.kind == HP_K_OFF); CHECK(b.target == HP_TGT_LINK);
    noauth(&m); hp_banner(&m, true, &b); CHECK_STR(b.text, "HMI HAS NO CONTROL AUTHORITY");
    fault(&m); hp_banner(&m, true, &b);
    CHECK_STR(b.text, "NEW FAULT: IOUT_OVERCURRENT \xC2\xB7 TAP TO VIEW"); CHECK(b.target == HP_TGT_FAULTS);
    m.simulated_measurements = true; hp_banner(&m, true, &b); CHECK(b.two_line);
    CHECK_STR(b.text, "SIMULATED V/I (WAIT) | REAL relay cmd, not contacts\nNEW FAULT: IOUT_OVERCURRENT \xC2\xB7 TAP TO VIEW");
    idle(&m); m.simulated_measurements = m.simulated_ready = true; hp_banner(&m, true, &b);
    CHECK(!b.two_line); CHECK(b.kind == HP_K_WARN); CHECK(b.target == HP_TGT_LINK);
    CHECK_STR(b.text, "SIMULATED V/I | REAL relay cmd, not contacts");

    idle(&m); hp_session_line(s, sizeof(s), &m); CHECK_STR(s, "Ready");
    charging(&m); hp_session_line(s, sizeof(s), &m); CHECK_STR(s, "#17");
    offline(&m); m.session_id = 5; hp_session_line(s, sizeof(s), &m); CHECK_STR(s, "#5? \xC2\xB7 Unconfirmed");
    idle(&m); m.end_valid = true; m.end_session_id = 9; m.end_reason = 2;
    hp_session_line(s, sizeof(s), &m); CHECK_STR(s, "#9 aborted");
}

static void test_page_helpers(void)
{
    rcc_snapshot_t m; hp_bar_spec_t b; hp_gauge_t g; char s[48];

    idle(&m); CHECK(hp_start_block(&m) == HP_SB_NONE);
    hp_bar_spec(&m, true, &b); CHECK(b.start_block == HP_SB_NONE);
    offline(&m); CHECK(hp_start_block(&m) == HP_SB_LINK_STALE);
    hp_bar_spec(&m, true, &b); CHECK(b.kind == HP_BAR_FULL); CHECK(b.start_block == HP_SB_LINK_STALE);
    hp_bar_spec(&m, false, &b); CHECK(b.start_block == HP_SB_NONE);   /* only the full-width bar shows the reason */
    fault(&m); CHECK(hp_start_block(&m) == HP_SB_FAULT_LATCHED);
    hp_bar_spec(&m, true, &b); CHECK(b.start_block == HP_SB_FAULT_LATCHED);
    charging(&m); CHECK(hp_start_block(&m) == HP_SB_NONE);   /* STOP shown, but not because START is blocked by link/fault */
    CHECK_STR(hp_start_block_text(HP_SB_LINK_STALE), "Start blocked: UART link stale");
    CHECK_STR(hp_start_block_text(HP_SB_FAULT_LATCHED), "Start blocked: latched fault active");
    CHECK_STR(hp_start_block_text(HP_SB_NONE), "");

    idle(&m); hp_controller_summary(s, sizeof(s), &m); CHECK_STR(s, "IDLE \xC2\xB7 Ready");
    charging(&m); hp_controller_summary(s, sizeof(s), &m); CHECK_STR(s, "CHARGING");
    offline(&m); hp_controller_summary(s, sizeof(s), &m); CHECK_STR(s, "UNKNOWN");

    idle(&m); m.product_id = 1; m.protocol_version = 1; CHECK(hp_fw_compat(&m) == HP_FW_COMPATIBLE);
    m.protocol_version = 2; CHECK(hp_fw_compat(&m) == HP_FW_INCOMPATIBLE);
    m.protocol_version = 1; m.product_id = 7; CHECK(hp_fw_compat(&m) == HP_FW_INCOMPATIBLE);
    m.online = false; CHECK(hp_fw_compat(&m) == HP_FW_UNREAD);
    idle(&m); m.device_valid = false; CHECK(hp_fw_compat(&m) == HP_FW_UNREAD);

    CHECK_STR(hp_fault_title(HP_FS_OK), "All Systems Nominal \xE2\x80\x94 No Faults");
    CHECK_STR(hp_fault_title(HP_FS_FAULT), "Active Fault Condition");
    CHECK_STR(hp_fault_title(HP_FS_INHIBIT), "Charging Inhibited");
    CHECK_STR(hp_fault_title(HP_FS_UNKNOWN), "Fault State Unreadable");

    CHECK(hp_event_kind(0x0012) == HP_K_FAULT);
    CHECK(hp_event_kind(0x0011) == HP_K_WARN); CHECK(hp_event_kind(0x0030) == HP_K_WARN); CHECK(hp_event_kind(0x0033) == HP_K_WARN);
    CHECK(hp_event_kind(0x0013) == HP_K_ACCENT); CHECK(hp_event_kind(0x0016) == HP_K_ACCENT);
    CHECK(hp_event_kind(0x0034) == HP_K_OFF); CHECK(hp_event_kind(0x0001) == HP_K_OFF);

    idle(&m); m.command_state = RCC_COMMAND_NONE; hp_command_text(s, sizeof(s), &m); CHECK_STR(s, "\xE2\x80\x94");
    m.last_command = 0x21; m.command_state = RCC_COMMAND_COMPLETED; hp_command_text(s, sizeof(s), &m); CHECK_STR(s, "STOP \xC2\xB7 OK");
    m.last_command = 0x20; m.command_state = RCC_COMMAND_REJECTED; hp_command_text(s, sizeof(s), &m); CHECK_STR(s, "START \xC2\xB7 REJECTED");
    m.command_state = RCC_COMMAND_FAILED; hp_command_text(s, sizeof(s), &m); CHECK_STR(s, "START \xC2\xB7 FAILED");
    m.command_state = RCC_COMMAND_UNCONFIRMED; hp_command_text(s, sizeof(s), &m); CHECK_STR(s, "START \xC2\xB7 NO REPLY");
    m.command_state = RCC_COMMAND_SENDING; hp_command_text(s, sizeof(s), &m); CHECK_STR(s, "START \xC2\xB7 WAIT");

    CHECK(hp_brightness_preset_selected(50, 1)); CHECK(!hp_brightness_preset_selected(50, 2));
    CHECK(hp_brightness_preset_selected(100, 3)); CHECK(hp_brightness_preset_selected(90, 3)); CHECK(!hp_brightness_preset_selected(75, 3));
    CHECK(hp_brightness_preset_selected(25, 0)); CHECK(hp_brightness_preset_selected(10, 0)); CHECK(!hp_brightness_preset_selected(35, 0));
    CHECK(!hp_brightness_preset_selected(60, 1) && !hp_brightness_preset_selected(60, 2));   /* between presets: none highlighted */

    charging(&m); hp_gauge(&m, &g);   /* 48.2 V, 8.0 A -> 385.6 W */
    CHECK(g.v_pm == 803); CHECK(g.a_pm == 800); CHECK(g.power_valid); CHECK(g.power_w == 386); CHECK(g.w_pm == 642);
    m.vout_mv = 90000; m.iout_ma = 20000; hp_gauge(&m, &g); CHECK(g.v_pm == 1000); CHECK(g.a_pm == 1000); CHECK(g.w_pm == 1000);
    m.iout_ma = -500; hp_gauge(&m, &g); CHECK(g.a_pm == 0); CHECK(g.w_pm == 0); CHECK(g.power_valid);
    charging(&m); m.vout_status |= 0x40; CHECK(hp_v_floor(&m)); hp_gauge(&m, &g);   /* below the voltage floor: no volt or power fill */
    CHECK(g.v_pm == 0 && !g.power_valid && g.a_pm == 800);
    offline(&m); hp_gauge(&m, &g); CHECK(g.v_pm == 0 && g.a_pm == 0 && g.w_pm == 0 && !g.power_valid);
}

int main(void)
{
    test_names(); test_events(); test_fmt(); test_derived(); test_state_info(); test_bar(); test_banner_session(); test_page_helpers();
    printf("test_hmi_present: %d checks, %d failed\n", checks, fails);
    return fails ? 1 : 0;
}
