# HMI pages and implemented behavior

This describes `firmware/src/hmi_ui.c` and `rcc_link.c` at revision `28d2253`,
checked on 2026-10-30. It replaces the earlier pre-implementation UI contract.
The HTML mockup and preview images are design references and may differ from
the current firmware. Hardware layout/navigation acceptance remains pending.
Controller wire semantics are documented in
[controller-interface.md](controller-interface.md).

## Layout and refresh

The display is landscape 480 × 320. A left rail selects Overview, Measure,
Faults, Session and Device. The header shows controller identity, UART status,
fault state and Wi-Fi access. Content occupies a 408 px-wide area starting at
x = 64. Banners reduce available content height; the bottom action bar changes
with the page, controller state and latest command result.

A 1 s LVGL timer refreshes the UI. Rebuilding is deferred while a touch is
held or a START/Wi-Fi modal is open. The OTA overlay is updated before this
modal check. Main labels are English literals; selecting Vietnamese stores a
preference but does not translate the interface.

## Pages

| Page | Implemented information |
|---|---|
| Overview | State hero, stage strip, link/fault/relay-command summary, voltage/current/power and START/STOP action |
| Measure | Raw and filtered signed values, sequence, controller sample age, HMI-held age and channel-quality matrix |
| Faults | Summary, primary fault/code, action hint, named fault/inhibit masks and raw registers |
| Session | Session ID/state, completed-session duration, last command, end reason and scrollable event list with SYNC |
| Device / CONTROLLER | Product/protocol, HW/FW revisions, abbreviated image/boot IDs, build flags and running links |
| Device / COMM LINK | Online state, reply/data ages, timeout/bad-frame/missed-event counters and port labels |
| Device / SETTINGS | Backlight, inactivity dimming, language preference and Wi-Fi entry |

Missing or invalid measurements use a dash or a stale/invalid indication.
Current remains signed. Power is derived only when both voltage and current
are valid. VOUT below the measurement floor is shown as an upper bound.
The Measure quality matrix exposes bits 0–5 plus the floor bit; bit positions
for controller fault/inhibit names remain an open controller interface issue.

Session duration comes from terminal session events; an active session shows
`In progress`. Event times are relative to controller boot. The HMI does not
send SET_TIME. SYNC requests event-log data, not clock synchronization.

## Link freshness and polling

- UART1 TX GPIO43 / RX GPIO44, 115200 8N1, is started during display startup.
  A startup error produces `UART FAILED` / `UART INIT FAILED` indications.
- The link is marked offline after more than 2 s without a matching reply.
  The banner warns `LINK LOST · CHARGING MAY CONTINUE`.
- UI status is fresh when online, valid and held for at most 1500 ms.
  Voltage/current additionally need valid measurement data held for at most
  1500 ms and all six channel-status bits set (`0x3F`). Controller sample age
  is displayed separately; these UI predicates do not add it to held age.
- One query transaction runs at a time. Query starts are at least 170 ms
  apart and cycle through PING, STATUS, MEASUREMENTS, STATUS, FAULTS, EVENTS,
  STATUS, MEASUREMENTS, DEVICE_INFO. Event queries request up to four records.
  Actual rates depend on response time/retries, not the open page.

## START and STOP

START is offered on Overview when status is fresh, no command is SENDING or
ACCEPTED, and state is either INHIBITED or OPERATIONAL/IDLE. It opens
`Confirm Charge Start`, showing controller state, link age, ADC validity and
fault/inhibit information. SEND START queues the request. The link layer
rechecks online/status age and pending transactions before accepting it.
Controller authorization, precheck and relay decisions remain controller-owned.

STOP queues immediately without confirmation when no START modal blocks the
click handler. The link layer allows a STOP request without requiring online
status and dispatches it before a newly queued START. This is a request, not
proof that the controller received it. Compact STOP/status bars appear on
other pages when appropriate; a recent authorization rejection replaces the
bar with the no-authority indication.

Initial transmissions allow two identical retries at 50 ms intervals,
retaining the request ID and bytes. ACCEPTED changes to waiting up to 15 s for a
terminal result; a timeout is UNCONFIRMED. START/STOP results are displayed separately
from charging state. `CHARGING` without `charging_established` is shown as
`NOT_CONFIRMED` / `CHARGING NOT ESTABLISHED`.

Current STOP wording is implemented as follows:

| Command result | Action-bar feedback |
|---|---|
| SENDING | Waiting for controller; STOP sent, not yet confirmed |
| COMPLETED | Stopped by command; relay commanded open |
| REJECTED | STOP rejected plus reason, except the separate no-authority bar |
| FAILED | Stop took effect, not tracked; Send STOP again |
| Other nonterminal/unconfirmed state | NOT CONFIRMED; check controller |

These are current UI strings, not independent evidence of contact movement.
The board has no contact feedback. Controller reason semantics, including
QUEUE_FULL and PERSISTENCE_FAILED, must be interpreted using the interface
document; see the remaining differences below.

## Local settings, networking and updates

Device → SETTINGS and the Wi-Fi screens configure the HMI only. Brightness,
dimming and language selection are persisted in NVS. Wi-Fi provides scanning,
connection, network information, disconnect/forget and automatic reconnection.
Web OTA runs after the station obtains an IP address and displays update
progress. See [settings, Wi-Fi and OTA](settings-wifi-and-ota.md) for operating
steps, defaults, rollback behavior and current limitations.

## Known differences and acceptance work

- COMM LINK still says `UART0` / `GPIO1 / 3`; the actual transport uses UART1 /
  GPIO43 / GPIO44. Use the transport source for wiring.
- English/Vietnamese selection is saved but main labels are not translated.
- The earlier design required STOP accessibility through every modal. The
  current START modal covers the action-bar area and `action_click()` returns
  while it is open. Wi-Fi and OTA overlays also need STOP-access acceptance.
- START gating is narrower than the earlier design: VREQ_VALIDATE/PRECHECK do
  not offer START. The UI predicate does not explicitly check product/protocol
  compatibility, relay authorization or active-interface authority.
- A no-authority indication follows a rejected command; the controller does
  not report active control-interface selection in advance. This is not a
  permanent locally enforced monitoring-only mode.
- STOP rejection/failure wording is generic. Reason-specific QUEUE_FULL and
  PERSISTENCE_FAILED behavior from the earlier UI contract is not fully
  represented by the current strings.
- Port labels, generic command wording, build/version labels and fault-table
  compatibility require review before release. A `PRODUCT BUILD` label is
  derived from the provisional flag alone, not production certification.
- There is no reconnect button, separate CRC16/CRC32 counter UI, or guaranteed
  clock synchronization. Controller restart detection clears cached records;
  it is not a full controller or HMI reset.
- Test navigation, touch/modal behavior, reconnect/reboot, command outcomes,
  settings persistence, Wi-Fi/OTA, rollback and long-duration operation on the
  physical target. Protocol simulator tests do not establish these results.

## Simulated ADC demonstration

GET_STATUS flags bit3 identifies simulated measurements; bit4 reports source
readiness. Legacy controllers leave both zero. Both UI and link START admission
require simulated_ready when simulated_measurements is set. The shared banner
keeps SIMULATED V/I visible on all five pages, identifies the real relay command
and retains link/fault/authority warnings. START confirmation and session data
are annotated as simulated. UART pins, baud, touch and normal peers retain the
existing workflow. A fresh status is held for strictly less than 1500 ms.

The controller demo uses real persistence and relay actuation, with a 60 s
effective in-memory session cap. Simulated current does not establish actual
charging. See the controller repository's
[demo runbook](../../robot_charge_controller/docs/firmware/verification/sim-adc-demo-runbook.md).
