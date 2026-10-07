/* Simulated controller snapshots (stand-in for rcc_link.c) for the PC renderer. */
#include <string.h>
#include "rcc_link.h"

static rcc_snapshot_t s_snap;
int sim_start_requests, sim_stop_requests;

static void base(rcc_snapshot_t *m)
{
    memset(m, 0, sizeof(*m));
    m->online = m->device_valid = m->status_valid = m->measurement_valid = m->faults_valid = true;
    m->reply_age_ms = 400; m->status_age_ms = 400; m->measurement_age_ms = 400; m->faults_age_ms = 800;
    m->measurement_sample_age_ms = 12; m->measurement_sequence = 48213;
    m->vout_status = m->iout_status = 0x3f;
    m->firmware_revision = 7; m->hardware_revision = 1; m->build_flags = 0x05;
    m->product_id = 1; m->protocol_version = 1; m->running_links = 0xf;
    memcpy(m->image_id, "\x9F\x3A\xC2\x1B\x00\x00\xE8\x15", 8);
    m->boot_id = 0x9F3AC21B0000E4B2ull;
    m->command_age_ms = UINT32_MAX;
}
/* t_s: controller seconds since boot; a: first words of the event data. */
static void ev(rcc_snapshot_t *m, uint16_t code, uint32_t seq, uint32_t t_s,
               uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
    const int n = m->event_count++;
    m->event_codes[n] = code; m->event_sequences[n] = seq; m->event_us[n] = (uint64_t)t_s * 1000000u;
    m->event_args[n][0] = a0; m->event_args[n][1] = a1; m->event_args[n][2] = a2; m->event_args[n][3] = a3;
}
#define ST(ot, oo, nt, no) ((ot) | ((oo) << 8) | ((nt) << 16) | ((uint32_t)(no) << 24))

int sim_scenario(const char *name)
{
    rcc_snapshot_t *m = &s_snap; base(m);
    if (!strcmp(name, "CHARGING")) {
        m->top_state = 4; m->operational_state = 6; m->charging_established = true; m->build_flags = 0x07;
        m->relay_command = 1; m->session_id = 17; m->vout_mv = 48200; m->iout_ma = 8000;
        m->vout_filtered_mv = 48100; m->iout_filtered_ma = 8000;
        m->last_command = 0x20; m->command_state = RCC_COMMAND_COMPLETED; m->command_age_ms = 14000;
        m->event_missed = 3;
        ev(m, 1, 101, 1, 1, 0, 0, 0); ev(m, 0x0013, 118, 1310, 17, 0, 0, 0);
        ev(m, 0x0011, 119, 1311, 1, 0, 0, 0); ev(m, 0x0010, 120, 1318, ST(4,4,4,5), 0, 0, 0);
        ev(m, 0x0014, 121, 1320, 17, 48200, 8000, 0); ev(m, 0x0010, 122, 1321, ST(4,5,4,6), 0, 0, 0);
    } else if (!strcmp(name, "IDLE")) {
        m->top_state = 4; m->operational_state = 1; m->session_id = 0;
        m->vout_mv = 3400; m->vout_filtered_mv = 3400; m->vout_status = 0x7f; m->iout_ma = 0;
        m->command_age_ms = UINT32_MAX;
        ev(m, 0x0014, 88, 100, 16, 48000, 8100, 0); ev(m, 0x0015, 90, 4400, 16, 1, 4320000, 48000);
        ev(m, 0x0010, 91, 4401, ST(4,7,4,8), 0, 0, 0); ev(m, 0x0010, 92, 4700, ST(4,8,4,1), 0, 0, 0);
        m->end_valid = true; m->end_session_id = 16; m->end_reason = 1; m->end_duration_ms = 4320000;
    } else if (!strcmp(name, "PRECHECK")) {
        m->top_state = 4; m->operational_state = 3; m->vout_mv = 47900; m->vout_filtered_mv = 47900;
        m->iout_ma = 0; m->last_command = 0x20; m->command_state = RCC_COMMAND_ACCEPTED; m->command_age_ms = 2000;
        ev(m, 0x0010, 130, 5000, ST(4,1,4,2), 0, 0, 0); ev(m, 0x0010, 131, 5001, ST(4,2,4,3), 0, 0, 0);
    } else if (!strcmp(name, "INHIBITED")) {
        m->top_state = 5; m->inhibit_mask = 4; m->primary_fault = 0x0302; m->active_fault_mask = 0x2A0;
        m->vout_mv = 3400; m->vout_filtered_mv = 3400; m->vout_status = 0x7f; m->iout_status = 0x0014;
        ev(m, 1, 1, 1, 1, 5, 0, 0); ev(m, 0x0010, 40, 2, ST(1,0,4,1), 0, 0, 0);
        ev(m, 0x0012, 41, 180, 0, 0x2A0, 0x0302, 0); ev(m, 0x0011, 42, 180, 0, 4, 0, 0);
        ev(m, 0x0010, 43, 180, ST(4,1,5,0), 0, 0, 0);
    } else if (!strcmp(name, "FAULT")) {
        m->top_state = 6; m->primary_fault = 0x0402; m->active_fault_mask = 1u << 11; m->build_flags = 0x07;
        m->session_id = 17; m->vout_mv = 47900; m->vout_filtered_mv = 47900; m->iout_ma = 100; m->iout_filtered_ma = 100;
        m->last_command = 0x20; m->command_state = RCC_COMMAND_COMPLETED; m->command_age_ms = 1140000;
        m->end_valid = true; m->end_session_id = 17; m->end_reason = 4; m->end_duration_ms = 1084000;
        ev(m, 0x0013, 118, 100, 17, 0, 0, 0); ev(m, 0x0014, 119, 120, 17, 48200, 8000, 0);
        ev(m, 0x0012, 130, 1200, 0, 1u << 11, 0x0402, 0); ev(m, 0x0016, 131, 1200, 17, 4, 1084000, 47900);
        ev(m, 0x0010, 132, 1200, ST(4,6,6,0), 0, 0, 0);
    } else if (!strcmp(name, "OFFLINE")) {
        m->online = false; m->reply_age_ms = 8200; m->status_age_ms = 8200; m->measurement_age_ms = 8200;
        m->faults_age_ms = 8200; m->top_state = 4; m->operational_state = 6; m->timeouts = 14;
        m->vout_mv = 48200; m->iout_ma = 8000; m->last_command = 0x20;
        m->command_state = RCC_COMMAND_COMPLETED; m->command_age_ms = 180000; m->session_id = 17;
    } else if (!strcmp(name, "REMOTE")) {
        m->top_state = 5; m->inhibit_mask = 1; m->vout_mv = 47900; m->vout_filtered_mv = 47900; m->session_id = 17;
        m->last_command = 0x21; m->command_state = RCC_COMMAND_COMPLETED; m->command_age_ms = 1000;
        m->end_valid = true; m->end_session_id = 17; m->end_reason = 2; m->end_duration_ms = 372000;
        ev(m, 0x0014, 121, 1320, 17, 48200, 8000, 0); ev(m, 0x0011, 140, 1700, 0, 1, 0, 0);
        ev(m, 0x0016, 141, 1700, 17, 2, 372000, 47900); ev(m, 0x0010, 142, 1700, ST(4,6,5,0), 0, 0, 0);
    } else return 0;
    return 1;
}
void sim_command_state(const char *name)
{
    rcc_snapshot_t *m = &s_snap; m->command_age_ms = 1000;
    if (!strcmp(name, "stopping")) { m->last_command = 0x21; m->command_state = RCC_COMMAND_SENDING; }
    else if (!strcmp(name, "stopped")) { m->last_command = 0x21; m->command_state = RCC_COMMAND_COMPLETED; }
    else if (!strcmp(name, "timeout")) { m->last_command = 0x21; m->command_state = RCC_COMMAND_UNCONFIRMED; }
    else if (!strcmp(name, "queuefull")) { m->last_command = 0x21; m->command_state = RCC_COMMAND_REJECTED; m->command_reason = 7; }
    else if (!strcmp(name, "noauth")) { m->last_command = 0x20; m->command_state = RCC_COMMAND_REJECTED; m->command_reason = 5; }
    else if (!strcmp(name, "accepted")) { m->last_command = 0x20; m->command_state = RCC_COMMAND_ACCEPTED; }
}
void rcc_link_snapshot(rcc_snapshot_t *out) { *out = s_snap; }
bool rcc_link_request_start(void) { ++sim_start_requests; return true; }
bool rcc_link_request_stop(void) { ++sim_stop_requests; return true; }
esp_err_t rcc_link_start(void) { return 0; }
