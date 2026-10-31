/*
 * Presentation logic for the HMI (see hmi_present.h). No LVGL here: everything is
 * text or a tone derived from an rcc_snapshot_t, so it can be unit-tested on the host.
 */
#include "hmi_present.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

/* ------------------------------------------------------------------ names */
const char *const hp_fault_names[HP_FAULT_COUNT] = {
    "CONTROL_INTERNAL", "NVS_WRITE_FAILED", "RELAY_COMMAND_FAILED", "RELAY_FEEDBACK_CONFLICT",
    "ADC_UNAVAILABLE", "ADC_STALE", "ADC_PATTERN_INVALID", "ADC_SATURATED", "VOUT_INVALID",
    "IOUT_INVALID", "VOUT_OVERVOLTAGE", "IOUT_OVERCURRENT", "IOUT_REVERSE_CURRENT",
    "CHARGE_NOT_ESTABLISHED", "CHARGE_MAX_DURATION", "CAN_DEGRADED", "RS485_DEGRADED",
    "OPERATIONAL_UART_DEGRADED", "DIAGNOSTIC_DEGRADED"};
static const uint16_t fault_ids[HP_FAULT_COUNT] = {
    0x0101, 0x0102, 0x0201, 0x0202, 0x0301, 0x0302, 0x0303, 0x0304, 0x0305, 0x0306,
    0x0401, 0x0402, 0x0403, 0x0501, 0x0502, 0x0601, 0x0602, 0x0603, 0x0604};
const char *const hp_inhibit_names[HP_INHIBIT_COUNT] = {"REMOTE", "RESET", "RECOVERY"};
static const char *const top_names[7] = {"BOOT_SAFE", "SELF_TEST", "SERVICE_LOCK", "CONFIG_MODE",
                                         "OPERATIONAL", "INHIBITED", "LATCHED_FAULT"};
static const char *const op_names[9] = {"NONE", "IDLE", "VREQ_VALIDATE", "PRECHECK",
                                        "RELAY_CLOSING", "CHARGE_VERIFY", "CHARGING",
                                        "COMPLETE", "WAIT_REARM"};
const char *const hp_end_names[HP_END_COUNT] = {"?", "COMPLETE", "REMOTE_STOP", "RECOVERABLE_FAULT",
                                                "LATCHED_FAULT", "NOT_ESTABLISHED", "MAX_DURATION",
                                                "INTERRUPTED_RESET", "START_ABORTED", "CHARGE_LOST"};
static const char *const reset_names[7] = {"UNKNOWN", "POWER_ON", "SOFTWARE", "WATCHDOG",
                                           "PANIC", "BROWNOUT", "OTHER"};

const char *hp_fault_name(uint16_t id)
{
    for (int i = 0; i < HP_FAULT_COUNT; ++i) if (fault_ids[i] == id) return hp_fault_names[i];
    return id ? "UNRECOGNIZED" : "none";
}
const char *hp_fault_short(uint16_t id)
{
    static const char *const s[HP_FAULT_COUNT] = {
        "INTERNAL", "NVS WRITE", "RELAY CMD", "RELAY FB", "ADC N/A", "ADC STALE", "ADC PATT.",
        "ADC SAT.", "VOUT BAD", "IOUT BAD", "OVERVOLT", "OVERCURR.", "REV CURR.", "NOT ESTAB",
        "MAX TIME", "CAN DEGR.", "RS485 DEG", "UART DEGR", "DIAG DEGR"};
    for (int i = 0; i < HP_FAULT_COUNT; ++i) if (fault_ids[i] == id) return s[i];
    return "FAULT";
}
const char *hp_fault_hint(uint16_t id)
{
    if (id >= 0x0301 && id <= 0x0306) return "check the current-sense circuit";
    if (id >= 0x0401 && id <= 0x0403) return "check the load and the current limit";
    if (id >= 0x0201 && id <= 0x0202) return "check the relay and its driver";
    if (id >= 0x0601 && id <= 0x0604) return "check the controller communication links";
    return "check the controller service log";
}
const char *hp_state_name(uint8_t top, uint8_t op)
{
    if (top == 4) return op < 9 ? op_names[op] : "OPERATIONAL";
    return top < 7 ? top_names[top] : "UNKNOWN";
}
void hp_mask_text(char *out, size_t cap, uint32_t mask, const char *const *names, int count)
{
    out[0] = '\0';
    if (!mask) { snprintf(out, cap, "none"); return; }
    for (int b = 0; b < 32; ++b) {
        if (!(mask & (1u << b))) continue;
        size_t used = strlen(out);
        if (b < count) snprintf(out + used, cap - used, "%s%s", used ? "+" : "", names[b]);
        else snprintf(out + used, cap - used, "%sbit %d", used ? "+" : "", b);
    }
}
const char *hp_reason_text(uint16_t r, char *buf, size_t n)
{
    static const char *const names[0x17] = {"NONE", "UNSUPPORTED_VERSION", "MALFORMED_MESSAGE",
        "INTEGRITY_FAILED", "UNSUPPORTED_COMMAND", "UNAUTHORIZED_SOURCE", "DUPLICATE_ID_CONFLICT",
        "QUEUE_FULL", "INVALID_STATE", "INHIBITED", "FAULT_ACTIVE", "INTERLOCK_FAILED",
        "MEASUREMENT_INVALID", "CONFIG_INVALID", "CAL_INVALID", "PERSISTENCE_FAILED",
        "CHARGER_REQUEST_ABSENT", "TIMEOUT", "CHARGE_NOT_ESTABLISHED", "ABORTED_BY_STOP",
        "ABORTED_BY_FAULT", "INTERNAL_ERROR", "RELAY_FEEDBACK_MISMATCH"};
    if (r < 0x17) return names[r];
    snprintf(buf, n, "0x%04X", (unsigned)r);
    return buf;
}

/* ------------------------------------------------------------------ derived state */
bool hp_fresh(const rcc_snapshot_t *m)
{
    return m->online && m->status_valid && m->status_age_ms < 1500;
}
bool hp_v_valid(const rcc_snapshot_t *m)
{
    return m->online && m->measurement_valid && m->measurement_age_ms <= 1500 &&
           (m->vout_status & 0x3f) == 0x3f;
}
bool hp_v_floor(const rcc_snapshot_t *m) { return hp_v_valid(m) && (m->vout_status & 0x40); }
bool hp_i_valid(const rcc_snapshot_t *m)
{
    return m->online && m->measurement_valid && m->measurement_age_ms <= 1500 &&
           (m->iout_status & 0x3f) == 0x3f;
}
bool hp_meas_stale(const rcc_snapshot_t *m)
{
    return !(m->online && m->measurement_valid && m->measurement_age_ms <= 1500);
}
bool hp_in_session(const rcc_snapshot_t *m)
{
    return hp_fresh(m) && m->top_state == 4 && m->operational_state >= 2 && m->operational_state <= 6;
}
hp_fault_state_t hp_fault_state(const rcc_snapshot_t *m)
{
    if (!m->online || !m->faults_valid) return HP_FS_UNKNOWN;
    if (m->active_fault_mask || m->primary_fault) return HP_FS_FAULT;
    if (m->inhibit_mask || (hp_fresh(m) && m->top_state == 5)) return HP_FS_INHIBIT;
    return HP_FS_OK;
}
bool hp_start_allowed(const rcc_snapshot_t *m)
{
    return hp_fresh(m) && (!m->simulated_measurements || m->simulated_ready) &&
           m->command_state != RCC_COMMAND_SENDING &&
           m->command_state != RCC_COMMAND_ACCEPTED &&
           (m->top_state == 5 || (m->top_state == 4 && m->operational_state == 1));
}
bool hp_no_authority(const rcc_snapshot_t *m)
{
    return m->command_state == RCC_COMMAND_REJECTED && m->command_reason == 0x0005;
}

/* ------------------------------------------------------------------ formatters */
void hp_fmt_age(char *b, size_t n, uint32_t ms)
{
    if (ms == UINT32_MAX) snprintf(b, n, "\xE2\x80\x94");
    else if (ms < 10000u) snprintf(b, n, "%u.%u s", (unsigned)(ms / 1000u), (unsigned)(ms % 1000u / 100u));
    else if (ms < 600000u) snprintf(b, n, "%u s", (unsigned)(ms / 1000u));
    else snprintf(b, n, "%u min", (unsigned)(ms / 60000u));
}
void hp_fmt_milli(char *b, size_t n, int32_t v, bool below)
{
    const bool neg = v < 0;
    const uint32_t a = neg ? (uint32_t)(-(int64_t)v) : (uint32_t)v;
    snprintf(b, n, "%s%s%u.%u", below ? "\xE2\x89\xA4 " : "", neg ? "\xE2\x88\x92" : "",
             (unsigned)(a / 1000u), (unsigned)(a % 1000u / 100u));
}
void hp_fmt_duration(char *b, size_t n, uint32_t ms)
{
    const uint32_t s = ms / 1000u;
    if (s >= 3600u) snprintf(b, n, "%uh %02um", (unsigned)(s / 3600u), (unsigned)(s % 3600u / 60u));
    else if (s >= 60u) snprintf(b, n, "%um %02us", (unsigned)(s / 60u), (unsigned)(s % 60u));
    else snprintf(b, n, "%us", (unsigned)s);
}
/* Controller time since its boot (the log has no wall clock). */
void hp_fmt_since_boot(char *b, size_t n, uint64_t us)
{
    const uint32_t s = (uint32_t)(us / 1000000u);
    if (s < 120u) snprintf(b, n, "+%us", (unsigned)s);
    else if (s < 7200u) snprintf(b, n, "+%um", (unsigned)(s / 60u));
    else snprintf(b, n, "+%uh", (unsigned)(s / 3600u));
}
void hp_session_line(char *b, size_t n, const rcc_snapshot_t *m)
{
    const bool active = hp_in_session(m);
    if (!hp_fresh(m)) {
        if (m->session_id) snprintf(b, n, "#%" PRIu32 "? \xC2\xB7 Unconfirmed", m->session_id);
        else snprintf(b, n, "Unconfirmed");
    } else if (active && m->session_id) {
        snprintf(b, n, "#%" PRIu32, m->session_id);
    } else if (m->end_valid) {
        snprintf(b, n, "#%" PRIu32 " %s", m->end_session_id, m->end_reason == 1 ? "done" : "aborted");
    } else if (m->session_id) {
        snprintf(b, n, "#%" PRIu32, m->session_id);
    } else {
        snprintf(b, n, "Ready");
    }
}

/* ------------------------------------------------------------------ events */
const char *hp_event_name(uint16_t code)
{
    switch (code) {
    case 0x0001: return "BOOT_COMPLETED";
    case 0x0010: return "STATE_CHANGED";
    case 0x0011: return "INHIBIT_CHANGED";
    case 0x0012: return "FAULT_CHANGED";
    case 0x0013: return "SESSION_ARMED";
    case 0x0014: return "CHARGE_ESTABLISHED";
    case 0x0015: return "SESSION_COMPLETED";
    case 0x0016: return "SESSION_ABORTED";
    case 0x0020: return "CONFIG_CHANGED";
    case 0x0021: return "CAL_CHANGED";
    case 0x0030: return "COMM_DEGRADED";
    case 0x0031: return "PERSISTENCE_HEALTH";
    case 0x0032: return "DIAGNOSTIC_LOSS";
    case 0x0033: return "MEASUREMENT_FAULT";
    case 0x0040: return "TIME_SYNC_CHANGED";
    default: return "EVENT";
    }
}
void hp_event_detail(char *b, size_t n, uint16_t code, const uint32_t a[4])
{
    char x[32], y[32], z[20];
    b[0] = '\0';
    switch (code) {
    case 0x0001: snprintf(b, n, "reset: %s", (a[0] & 0xffu) < 7u ? reset_names[a[0] & 0xffu] : "?"); break;
    case 0x0010:
        snprintf(x, sizeof(x), "%s", hp_state_name(a[0] & 0xffu, (a[0] >> 8) & 0xffu));
        snprintf(y, sizeof(y), "%s", hp_state_name((a[0] >> 16) & 0xffu, (a[0] >> 24) & 0xffu));
        snprintf(b, n, "%.24s \xE2\x86\x92 %.24s", x, y); break;
    case 0x0011:
        hp_mask_text(x, sizeof(x), a[0], hp_inhibit_names, HP_INHIBIT_COUNT);
        hp_mask_text(y, sizeof(y), a[1], hp_inhibit_names, HP_INHIBIT_COUNT);
        snprintf(b, n, "%.24s \xE2\x86\x92 %.24s", x, y); break;
    case 0x0012:
        snprintf(b, n, "%s \xE2\x86\x92 %s", a[0] ? "active" : "none",
                 (a[2] & 0xffffu) ? hp_fault_name((uint16_t)a[2]) : "none"); break;
    case 0x0013: snprintf(b, n, "session #%" PRIu32, a[0]); break;
    case 0x0014:
        hp_fmt_milli(x, sizeof(x), (int32_t)a[1], false); hp_fmt_milli(y, sizeof(y), (int32_t)a[2], false);
        snprintf(b, n, "%.20s V \xC2\xB7 %.20s A", x, y); break;
    case 0x0015: case 0x0016:
        hp_fmt_duration(z, sizeof(z), a[2]);
        snprintf(b, n, "%s \xC2\xB7 %s", (a[1] & 0xffu) < 10u ? hp_end_names[a[1] & 0xffu] : "?", z); break;
    default: break;
    }
}

/* ------------------------------------------------------------------ state headline */
void hp_state_info(const rcc_snapshot_t *m, hp_state_info_t *s)
{
    char t[40];
    memset(s, 0, sizeof(*s));
    s->stage = -1;
    if (!hp_fresh(m)) {
        s->name = "UNKNOWN"; snprintf(s->sub, sizeof(s->sub), "STATE NOT CONFIRMED");
        s->kind = HP_K_OFF; return;
    }
    s->name = hp_state_name(m->top_state, m->operational_state);
    switch (m->top_state) {
    case 0: case 1: snprintf(s->sub, sizeof(s->sub), "STARTING UP"); break;
    case 2: snprintf(s->sub, sizeof(s->sub), "CONFIG / CALIBRATION NEEDED"); s->kind = HP_K_WARN; break;
    case 3: snprintf(s->sub, sizeof(s->sub), "SERVICE CONFIGURATION"); break;
    case 5:
        hp_mask_text(t, sizeof(t), m->inhibit_mask, hp_inhibit_names, HP_INHIBIT_COUNT);
        for (char *c = t; *c; ++c) if (*c >= 'a' && *c <= 'z') *c -= 32;
        if (m->primary_fault) snprintf(s->sub, sizeof(s->sub), "%s \xC2\xB7 FAULT %s", t, hp_fault_name(m->primary_fault));
        else snprintf(s->sub, sizeof(s->sub), "%s", m->inhibit_mask ? t : "INHIBITED");
        s->kind = HP_K_WARN; break;
    case 6:
        snprintf(s->sub, sizeof(s->sub), "FAULT 0x%04X", (unsigned)m->primary_fault);
        s->kind = HP_K_FAULT; break;
    case 4: {
        static const char *const subs[9] = {"OPERATIONAL", "OPERATIONAL \xC2\xB7 READY",
            "VALIDATING REQUEST", "PRE-CHECKING", "CLOSING RELAY", "VERIFYING CURRENT",
            "OPERATIONAL \xC2\xB7 SESSION ACTIVE", "CHARGE COMPLETE", "WAITING REARM"};
        const uint8_t op = m->operational_state < 9 ? m->operational_state : 0;
        snprintf(s->sub, sizeof(s->sub), "%s", subs[op]);
        if (op == 6 && m->session_id) snprintf(s->sub, sizeof(s->sub), "OPERATIONAL \xC2\xB7 SESSION #%u", (unsigned)m->session_id);
        static const int stage[9] = {-1, 0, 1, 1, 2, 3, 4, 4, -1};
        s->stage = stage[op];
        if (op == 6 && !m->charging_established) {
            s->name = "NOT_CONFIRMED"; s->kind = HP_K_WARN; s->stage = 3;
            snprintf(s->sub, sizeof(s->sub), "CHARGING NOT ESTABLISHED");
        }
        break; }
    default: break;
    }
}

/* ------------------------------------------------------------------ action bar */
void hp_bar_spec(const rcc_snapshot_t *m, bool is_overview, hp_bar_spec_t *b)
{
    memset(b, 0, sizeof(*b));
    hp_state_info_t s; hp_state_info(m, &s);
    char rb[16];
    const bool pending = m->command_state == RCC_COMMAND_SENDING || m->command_state == RCC_COMMAND_ACCEPTED ||
                         m->command_state == RCC_COMMAND_UNCONFIRMED;
    const bool terminal = m->command_state == RCC_COMMAND_COMPLETED || m->command_state == RCC_COMMAND_REJECTED ||
                          m->command_state == RCC_COMMAND_FAILED;
    const bool recent = pending || (terminal && m->command_age_ms < 15000u);
    b->title_kind = HP_K_ACCENT;
    if (recent && hp_no_authority(m)) { b->kind = HP_BAR_NOAUTH; return; }
    if (recent && m->last_command == 0x21) {
        b->kind = HP_BAR_SPLIT; b->button = HP_B_STOP;
        switch (m->command_state) {
        case RCC_COMMAND_SENDING:
            snprintf(b->title, sizeof(b->title), "Waiting for the controller\xE2\x80\xA6");
            snprintf(b->sub, sizeof(b->sub), "STOP sent \xE2\x80\x94 not yet confirmed"); b->button = HP_B_STOPPING; break;
        case RCC_COMMAND_COMPLETED:
            snprintf(b->title, sizeof(b->title), "Stopped by command");
            snprintf(b->sub, sizeof(b->sub), "Relay commanded open");
            b->button = hp_start_allowed(m) ? HP_B_START : HP_B_NONE; break;
        case RCC_COMMAND_REJECTED:
            b->title_kind = HP_K_WARN;
            snprintf(b->title, sizeof(b->title), "STOP rejected");
            snprintf(b->sub, sizeof(b->sub), "%s", hp_reason_text(m->command_reason, rb, sizeof(rb))); break;
        case RCC_COMMAND_FAILED:
            b->title_kind = HP_K_WARN;
            snprintf(b->title, sizeof(b->title), "Stop took effect, not tracked");
            snprintf(b->sub, sizeof(b->sub), "Send STOP again"); break;
        default:
            b->title_kind = HP_K_WARN;
            snprintf(b->title, sizeof(b->title), "NOT CONFIRMED");
            snprintf(b->sub, sizeof(b->sub), "No reply to STOP \xE2\x80\x94 check controller"); break;
        }
    } else if (recent && m->last_command == 0x20 && m->command_state != RCC_COMMAND_COMPLETED) {
        b->kind = HP_BAR_SPLIT; b->button = HP_B_STOP;
        switch (m->command_state) {
        case RCC_COMMAND_SENDING:
            snprintf(b->title, sizeof(b->title), "Sending START\xE2\x80\xA6");
            snprintf(b->sub, sizeof(b->sub), "Waiting for the controller"); break;
        case RCC_COMMAND_ACCEPTED:
            snprintf(b->title, sizeof(b->title), "Request accepted \xE2\x80\x94 checking");
            snprintf(b->sub, sizeof(b->sub), "Accepted, not yet complete"); break;
        case RCC_COMMAND_UNCONFIRMED:
            b->title_kind = HP_K_WARN;
            snprintf(b->title, sizeof(b->title), "NOT CONFIRMED");
            snprintf(b->sub, sizeof(b->sub), "No reply to START"); break;
        default:
            b->title_kind = HP_K_WARN;
            snprintf(b->title, sizeof(b->title), "START rejected");
            snprintf(b->sub, sizeof(b->sub), "%s", hp_reason_text(m->command_reason, rb, sizeof(rb)));
            b->button = hp_start_allowed(m) ? HP_B_START : HP_B_STOP; break;
        }
    } else {
        b->kind = HP_BAR_FULL; b->button = hp_start_allowed(m) ? HP_B_START : HP_B_STOP;
        if (!is_overview) {
            if (b->button == HP_B_START) { b->kind = HP_BAR_NONE; b->button = HP_B_NONE; return; }
            b->kind = HP_BAR_SPLIT;
            b->title_kind = s.kind;
            snprintf(b->title, sizeof(b->title), "%s", s.name);
            if (!hp_fresh(m)) snprintf(b->sub, sizeof(b->sub), "Charging may still be in progress");
            else if (m->top_state == 6) snprintf(b->sub, sizeof(b->sub), "%s \xC2\xB7 service needed", hp_fault_name(m->primary_fault));
            else if (m->session_id) snprintf(b->sub, sizeof(b->sub), "Session #%" PRIu32 " \xC2\xB7 relay commanded %s", m->session_id, m->relay_command ? "closed" : "open");
            else snprintf(b->sub, sizeof(b->sub), "Relay commanded %s", m->relay_command ? "closed" : "open");
        }
    }
    if (b->kind == HP_BAR_SPLIT && b->button == HP_B_NONE) b->kind = HP_BAR_MSG;
    if (!is_overview && b->button == HP_B_START) { b->button = HP_B_NONE; b->kind = HP_BAR_MSG; }
    if (b->kind == HP_BAR_FULL && b->button == HP_B_STOP) b->start_block = hp_start_block(m);
}

/* ------------------------------------------------------------------ banner */
void hp_banner(const rcc_snapshot_t *m, bool link_started, hp_banner_t *b)
{
    b->text = NULL; b->buf[0] = '\0';
    b->kind = HP_K_NORMAL; b->target = HP_TGT_NONE; b->two_line = false;
    if (!link_started) { b->text = "UART INIT FAILED \xC2\xB7 NO CONTROLLER LINK"; b->kind = HP_K_FAULT; }
    else if (!m->online) { b->text = "LINK LOST \xC2\xB7 CHARGING MAY CONTINUE"; b->kind = HP_K_OFF; b->target = HP_TGT_LINK; }
    else if (hp_no_authority(m) && m->command_age_ms < 15000u) { b->text = "HMI HAS NO CONTROL AUTHORITY"; b->kind = HP_K_WARN; }
    else if (hp_fresh(m) && m->top_state == 6) {
        snprintf(b->buf, sizeof(b->buf), "NEW FAULT: %s \xC2\xB7 TAP TO VIEW", hp_fault_name(m->primary_fault));
        b->text = b->buf; b->kind = HP_K_FAULT; b->target = HP_TGT_FAULTS;
    }
    /* Shared chrome keeps source identity on all pages, including stale data.
     * Retain safety/link warnings as a second line in the existing banner. */
    if (m->simulated_measurements) {
        const char *source = !hp_fresh(m) ? "SIMULATED V/I (STALE) | REAL relay cmd, not contacts" :
                             (!m->simulated_ready && !hp_in_session(m)) ? "SIMULATED V/I (WAIT) | REAL relay cmd, not contacts" :
                                                  "SIMULATED V/I | REAL relay cmd, not contacts";
        if (b->text) {
            char warning[sizeof(b->buf)];
            snprintf(warning, sizeof(warning), "%s", b->text);
            snprintf(b->buf, sizeof(b->buf), "%s\n%.70s", source, warning);
            b->text = b->buf;
            b->two_line = true;
        } else {
            b->text = source;
            b->kind = HP_K_WARN;
            b->target = HP_TGT_LINK;
        }
    }
}

/* ------------------------------------------------------------------ derived text and state for pages */
hp_start_block_t hp_start_block(const rcc_snapshot_t *m)
{
    if (hp_start_allowed(m)) return HP_SB_NONE;
    return !hp_fresh(m) ? HP_SB_LINK_STALE : m->top_state == 6 ? HP_SB_FAULT_LATCHED : HP_SB_NONE;
}
const char *hp_start_block_text(hp_start_block_t r)
{
    return r == HP_SB_LINK_STALE ? "Start blocked: UART link stale" :
           r == HP_SB_FAULT_LATCHED ? "Start blocked: latched fault active" : "";
}
hp_fw_compat_t hp_fw_compat(const rcc_snapshot_t *m)
{
    if (!(m->online && m->device_valid)) return HP_FW_UNREAD;
    return m->product_id == HP_PRODUCT_ID && m->protocol_version == HP_PROTOCOL_VERSION ? HP_FW_COMPATIBLE : HP_FW_INCOMPATIBLE;
}
const char *hp_fault_title(hp_fault_state_t fs)
{
    static const char *const title[4] = {"All Systems Nominal \xE2\x80\x94 No Faults", "Active Fault Condition",
                                         "Charging Inhibited", "Fault State Unreadable"};
    return title[fs];
}
hp_kind_t hp_event_kind(uint16_t code)
{
    if (code == 0x0012) return HP_K_FAULT;
    if (code == 0x0011 || (code >= 0x0030 && code <= 0x0033)) return HP_K_WARN;
    if (code >= 0x0013 && code <= 0x0016) return HP_K_ACCENT;
    return HP_K_OFF;
}
void hp_command_text(char *b, size_t n, const rcc_snapshot_t *m)
{
    if (m->command_state == RCC_COMMAND_NONE) { snprintf(b, n, "\xE2\x80\x94"); return; }
    const char *res = m->command_state == RCC_COMMAND_COMPLETED ? "OK" :
                      m->command_state == RCC_COMMAND_REJECTED ? "REJECTED" :
                      m->command_state == RCC_COMMAND_FAILED ? "FAILED" :
                      m->command_state == RCC_COMMAND_UNCONFIRMED ? "NO REPLY" : "WAIT";
    snprintf(b, n, "%s \xC2\xB7 %s", m->last_command == 0x21 ? "STOP" : "START", res);
}
void hp_controller_summary(char *b, size_t n, const rcc_snapshot_t *m)
{
    if (hp_fresh(m) && m->top_state == 4 && m->operational_state == 1) { snprintf(b, n, "IDLE \xC2\xB7 Ready"); return; }
    hp_state_info_t s; hp_state_info(m, &s);
    snprintf(b, n, "%s", s.name);
}
const uint8_t hp_brightness_presets[4] = {25, 50, 75, 100};
bool hp_brightness_preset_selected(uint8_t pct, int i)
{
    return pct == hp_brightness_presets[i] || (i == 3 && pct > 75) || (i == 0 && pct < 35);
}
void hp_gauge(const rcc_snapshot_t *m, hp_gauge_t *g)
{
    memset(g, 0, sizeof(*g));
    if (hp_v_valid(m) && !hp_v_floor(m)) g->v_pm = (int)((int64_t)m->vout_mv * 1000 / HP_V_FULL_MV);
    if (hp_i_valid(m) && m->iout_ma > 0) g->a_pm = (int)((int64_t)m->iout_ma * 1000 / HP_I_FULL_MA);
    if (hp_v_valid(m) && !hp_v_floor(m) && hp_i_valid(m)) {
        const int64_t mw = (int64_t)m->vout_mv * m->iout_ma / 1000;
        g->power_valid = true;
        g->power_w = (int)((mw + 500) / 1000);
        if (mw > 0) g->w_pm = (int)(mw * 1000 / HP_P_FULL_MW);
    }
    if (g->v_pm > 1000) g->v_pm = 1000;
    if (g->a_pm > 1000) g->a_pm = 1000;
    if (g->w_pm > 1000) g->w_pm = 1000;
}
