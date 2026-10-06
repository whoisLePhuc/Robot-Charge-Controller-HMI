# HMI pages and information

This document specifies the content, behaviour and data of every HMI screen. It
complements [lvgl-implementation-plan.md](lvgl-implementation-plan.md) and is not LVGL
code. Every field, code and state comes from
[controller-interface.md](controller-interface.md); where the two differ, the interface
document is right.

The HMI controls and monitors the charger ([HMI-D01](decisions.md#hmi-d01-role-of-the-hmi)).
While it holds control, the robot can no longer START/STOP over RS485/CAN: a session can
be stopped only from the HMI or the hardware E-stop, so STOP on the HMI must always be
available. §3.4 still applies when the controller is not configured for the HMI.

On-screen language is English ([HMI-D09](decisions.md#hmi-d09-on-screen-language)); the
labels below are the strings shown on the display, kept in one string table.

## 1. Display principles

1. **Device state matters more than the open page.** Link state, serious faults and
   whether data is confirmed are always visible.
2. **Missing data is never shown as 0.** Missing or invalid data shows `—` with
   `No data`, `Stale` or `Invalid`.
3. **Command state is separate from charging state.** `ACCEPTED` means the controller
   took the request, not that it is charging or that the relay closed.
4. **Never claim contact state.** The current controller board has no contact feedback:
   `relay_feedback` is always `UNKNOWN`. Show the relay *command* separately and label
   it as a command, not a confirmation.
5. **A lost link does not mean stopped.** The controller continues a session when
   communication is lost (ICD §19). When the link is lost during charging, say
   "charging may still be in progress".
6. **No service changes from the HMI.** No control-interface, node, calibration or
   clear-fault operations.
7. **Warnings use an icon or text as well as colour,** never colour alone.
8. **Large touch targets:** at least about 44 × 44 px; START/STOP larger and far enough
   from other targets to avoid mis-taps. The panel is resistive: calibrate touch and
   avoid swipe gestures for important actions.

## 2. Frame layout (landscape 480 × 320)

Orientation: landscape ([HMI-D04](decisions.md#hmi-d04-screen-orientation)). Grid
taken from `mockups/hmi-overview-landscape.html`:

| Region | Position (px) | Content |
|---|---|---|
| Navigation rail | x 0–52, full height | five tabs of 64 px: `Overview`, `Measure`, `Faults`, `Session`, `Device`; icon plus short label; current tab marked; fault badge on `Faults` |
| Header | x 60–472, y 0–34 | device name, link chip (state + age), fault icon (tap opens Device / Faults) |
| Banner | x 52–480, y 34–52 | global banner (§9.3), hidden when none |
| Content | x 60–472, y 36–262 | page content; may scroll except on Overview |
| Action bar | x 60–472, y 268–312 | START or STOP, full width, 44 px high |

- On every page except Overview, a compact STOP button stays in the action bar whenever
  a session may be active or the state is unknown.
- No modal, keyboard or banner may cover the action bar. The START confirmation has an
  easy-to-hit cancel button and never opens by itself at boot or reconnect.

## 3. Page 1 — Overview

### 3.1 Information

- Link: `Connected` / `Link lost` / `Reconnecting`, with the age of the last valid reply
  (`Updated 0.4 s ago`).
- The controller's two-level state (§3.2), never renamed in a way that changes its
  meaning.
- VOUT (V) and IOUT (A) only when all six validity bits are set; VOUT below the floor is
  shown as `≤ 3.4 V` (bit6 below-floor), never as 0.
- The primary fault or the inhibit that blocks START.
- Latest command state: sending, accepted, completed, rejected, failed, unconfirmed.
- `Relay command: CLOSED/OPEN` and `Contact feedback: none on this board`.

### 3.2 State → label → controls

`top_state` / `operational_state` per
[controller-interface.md §8.3](controller-interface.md#83-two-level-state-firmware-rcc_statesh-confirmed).
"START allowed" is a UI hint only; the controller always decides. The compact STOP is
available in every state; the last column says when STOP is the primary action on
Overview.

| top / op | Label | START allowed | STOP primary |
|---|---|---|---|
| BOOT_SAFE, SELF_TEST | `Starting up` | no | no |
| SERVICE_LOCK | `Service lock — configuration or calibration needed` | no | no |
| CONFIG_MODE | `Being configured (service)` | no | no |
| OPERATIONAL / IDLE | `Ready` | yes, if the link is fresh, no fault, no other command pending | no |
| OPERATIONAL / VREQ_VALIDATE | `Validating charge request` | yes | yes |
| OPERATIONAL / PRECHECK | `Pre-checking` | yes (if not armed) | yes |
| OPERATIONAL / RELAY_CLOSING | `Closing relay` | no | yes |
| OPERATIONAL / CHARGE_VERIFY | `Verifying charge current` | no | yes |
| OPERATIONAL / CHARGING | `Charging` (only with flag `charging_established` = 1) | no | yes |
| OPERATIONAL / COMPLETE | `Charge complete` | no | no (relay commanded open) |
| OPERATIONAL / WAIT_REARM | `Waiting for charger removal` | no | no |
| INHIBITED | `Inhibited` + inhibit name (REMOTE / RESET / RECOVERY) | yes: START may clear an eligible inhibit; the controller decides | no |
| LATCHED_FAULT | `Latched fault — service needed` + fault name | no | no |
| unknown (link lost, not yet read) | `Not confirmed` | no | yes |

Example from the bench without INA240: `INHIBITED`, inhibit `RECOVERY`, primary fault
`ADC_STALE`. The UI must show this case without misreading it.

### 3.3 START/STOP

- **START:** opens a confirmation summarizing state, fault/inhibit and data age. After
  sending, per [lvgl-implementation-plan.md §7.2](lvgl-implementation-plan.md#72-start):
  show `Request accepted — checking` on `ACCEPTED`, and show `Charging` only when the
  terminal result is `COMPLETED` **and** GET_STATUS reports `CHARGING` with
  `charging_established = 1`.
- START is disabled when: the link is lost or stale, another START is pending, the state
  does not allow it (§3.2), or the protocol is incompatible. Give the reason in text.
- **STOP:** sent at once, no confirmation. The button reads `STOPPING…` until the
  terminal result.
  - `COMPLETED`: `Stopped by command — relay commanded open`. Never "contacts open".
  - `REJECTED/QUEUE_FULL`: `Stop took effect but could not be tracked — send again`.
    For STOP this `REJECTED` does **not** mean "nothing was done".
  - `FAILED/PERSISTENCE_FAILED`: `Relay commanded open, but saving the state failed`.
  - `REJECTED/UNAUTHORIZED_SOURCE`: `HMI has no authority to stop — use the E-stop`,
    shown as an error.
  - Timeout: `NOT CONFIRMED — check the controller`; never `Stopped`.

### 3.4 When the HMI has no control authority

Applies on `UNAUTHORIZED_SOURCE`, i.e. the controller has not selected the operational
UART as the control interface, or its image does not trust the operational node
([HMI-D02](decisions.md#hmi-d02-control-authority)):

- Replace the START/STOP area with `Controller has not granted control to this HMI —
  monitoring only` and a short hint: the operational UART must be configured on the
  controller (service).
- Do not show a STOP button as if it worked. Point to the hardware E-stop.
- Before the first control command, authority is "unknown" (the controller does not
  report it; [controller-change-requests.md](controller-change-requests.md) CR-01).

### 3.5 Overview layout (landscape)

```text
┌────┬─────────────────────────────────────────────────────┐
│ OV │ RCC-01        ● CONNECTED · 0.4 s              [⚠] │  header 34 px
│ ME ├─────────────────────────────────────────────────────┤
│ FL │                    CHARGING                         │  hero
│ SE │             OPERATIONAL · CHARGING                  │
│ DE │   IDLE   PRE   RELAY   VERIFY   [CHG]               │  stage strip
│    │ ┌─────────────────┐   48.2 V    8.0 A    386 W     │
│    │ │ UART  ·  0.4 s  │    (ring)   (ring)   (ring)    │  card + rings
│    │ │ No active fault │                                 │
│    │ │ Relay cmd CLOSED│                                 │
│    │ │ Feedback  NONE  │                                 │
│    │ ├─────────────────────────────────────────────────┤ │
│    │ │                     STOP                         │ │  action bar 44 px
└────┴─────────────────────────────────────────────────────┘
```

The sketch shows structure only. Example readings are inside the project's operating
envelope (≤ 60 V, ≤ 10 A continuous). The stage strip groups `VREQ_VALIDATE` and
`PRECHECK` under `PRE` and has no `COMPLETE`/`WAIT_REARM`; the hero label carries the
exact state. Power (W) is shown only when both V and I are valid.

## 4. Page 2 — Measure

| Field | Display | Rule |
|---|---|---|
| VOUT | V, 2 decimals | from `vout_mv`; valid only with all six bits; bit6 → `≤ x V` |
| IOUT | A, **signed** | from `iout_ma` (i32); never drop the sign or clamp negatives to 0 |
| Filtered VOUT / IOUT | V / A, labelled `Filtered` | `vout_filtered_mv`, `iout_filtered_ma` |
| Measurement sequence | number | not advancing between reads = the controller is not updating |
| Age | ms | `age_ms` plus the time the HMI has held it; over the threshold → `Stale` |
| Calibration | `Calibrated` / `Not calibrated` | bit2 of each channel; there is no separate field |
| Validity | present, fresh, calibrated, in-range, not-saturated, plausible, below-floor | summarized as `Valid`; tap to see every bit |

- Invalid or stale data shows `—`. The last value may stay in a secondary area, dimmed
  and labelled `Last value, stale`.
- A negative IOUT while charging may mean a calibration with the opposite sign (see the
  controller's demo 1 runbook, decision D4). Never flip the sign; show what the
  controller reports.
- No charts until the HMI sampling rate and RAM budget are known.

## 5. Page 3 — Faults and operating conditions

- Summary: `No fault`, `Fault active`, `Inhibited`, or `Fault state unreadable`.
- `primary_fault` with its name from
  [controller-interface.md §8.6](controller-interface.md#86-faults-rcc_fault_registryh-ids-confirmed-by-fdd-004-bit-positions-not-frozen)
  and its hex code.
- `active_fault_mask` and `inhibit_mask` as lists of named bits. Unknown bits:
  `Unrecognized (bit n)`; never dropped.
- Distinguish: an active fault, an inhibit that is not a fault, and unreadable data.
- Hints for common faults (for example `ADC_SATURATED` / `IOUT_INVALID`: "check the
  current-sense circuit") are hints, not diagnoses.

No `CLEAR_FAULT`, calibration, CONFIG or interface change. Bit positions are not frozen
by the ICD (CR-02): when the controller's `firmware_revision` differs from the one this
table was checked against, show `Fault table may not match this controller` with the
raw codes.

## 6. Page 4 — Session and event log

### 6.1 Session

- `session_id` from GET_STATUS.
- Current charging state.
- Duration: only from `duration_ms` of SESSION_COMPLETED/SESSION_ABORTED once the session
  has ended; never an HMI-side stopwatch presented as session duration.
- Latest START/STOP result: result, reason, time the HMI received it.
- Session end reason from `terminal_reason` (COMPLETE, REMOTE_STOP, CHARGE_LOST, …).

### 6.2 Event log

- Read with GET_EVENT_LOG by cursor; loaded when the page opens and on `Load newer`.
- The log lives in controller RAM, keeps the last 32 events and is **lost when the
  controller restarts**. Show `missed` when > 0: `n events overwritten`.
- Time column: the HMI does not send `SET_TIME`
  ([HMI-D07](decisions.md#hmi-d07-time-source)), so controller time stays unsynchronized.
  Show time relative to now or to the controller boot, from `monotonic_us`; show UTC only
  if an event carries `time_synchronized` = 1.
- `STATE_CHANGED`, `INHIBIT_CHANGED` and `FAULT_CHANGED` are coalesced; the log is not a
  complete record of every change.
- Distinguish `log empty` from `log unavailable`. Never use the log instead of current
  state; current state comes from GET_STATUS.

## 7. Page 5 — Device and link

### 7.1 From GET_DEVICE_INFO

- `product_id`, `protocol_version`, `hardware_revision`, `firmware_revision`, 8-byte image
  hash.
- Build flags: `provisional`, `relay_enable`, `operational_node` (read-only, with a short
  explanation).
- When `provisional` = 1 or `operational_node` = 1: a permanent notice `Controller runs a
  bench image — not a product build` ([HMI-D02](decisions.md#hmi-d02-control-authority)).
  When `relay_enable` = 0: add `Relay is locked in this build — START will not close the
  relay`.
- Links running on the controller: CAN, RS485, operational UART, service UART.
- `boot_id`, and when the HMI saw it change.
- Compatibility: `Compatible` when `product_id = 0x0001` and `protocol_version = 1`;
  otherwise `Unsupported version`.

GET_DEVICE_INFO does **not** report the active control interface; never present it as
known.

### 7.2 Link diagnostics

- Online/offline, age of the last valid reply.
- HMI-side counters: timeouts, CRC-16 errors, CRC-32 errors, malformed frames, reconnects.
- HMI UART configuration (port, pins, baud), kept apart from controller information.
- `Reconnect`: restarts communication and queries only; never resets the controller and
  never sends START/STOP.

## 8. Time

The HMI does not send `SET_TIME` and has no clock setting
([HMI-D07](decisions.md#hmi-d07-time-source)). There is no time page.

## 9. Modals, toasts and banners

### 9.1 START confirmation

`Request charge start?`, current state, data age, fault/inhibit. `CANCEL` and
`SEND START` clearly distinct; when conditions are not met, sending is disabled and the
reason is given.

### 9.2 Command results

| Result | Text |
|---|---|
| sending | `Waiting for the controller…` |
| `ACCEPTED` | `Controller accepted the request; not yet complete` |
| `COMPLETED` | START: `Charge current established` (once state confirms); STOP: §3.3 |
| `REJECTED` | `Rejected` + reason; STOP/QUEUE_FULL per §3.3 |
| `FAILED` | `Failed` + reason + state |
| timeout | `No confirmation received`; actual state unknown until the next query |

Reasons are shown with a readable name and the code (`CHARGER_REQUEST_ABSENT` →
`No charge request from the charger`, `INVALID_STATE` → `Not allowed in the current
state`, …).

### 9.3 Global banners

| Condition | Banner |
|---|---|
| Link lost or stale | `LINK LOST — charging may still be in progress`; START disabled; never covers STOP |
| New fault | high priority; tap to open Faults |
| `boot_id` changed | `Controller restarted`; cancel every pending command, query everything again |
| Incompatible protocol | control disabled; diagnostics only |
| `UNAUTHORIZED_SOURCE` | `HMI has no control authority` (§3.4) |

## 10. Data sources

| Data | Source | Suggested refresh | Used for control? |
|---|---|---|---|
| State, relay, session, flags, `boot_id` | GET_STATUS | ~2 Hz in foreground | START hint; the controller decides |
| V/I and validity | GET_MEASUREMENTS | 2–5 Hz | display only |
| Faults, inhibits | GET_FAULTS (also in every result header) | ~1 Hz and on page open | explanation, START hint |
| Identity, build flags | GET_DEVICE_INFO | on connect and when `boot_id` changes | compatibility |
| Event log | GET_EVENT_LOG | on demand | no |
| START/STOP results | COMMAND_RESULT by `request_id` | when they arrive, possibly seconds later | command life cycle |

Rates are starting points to be measured on hardware.

## 11. Colour and wording

- **Normal:** neutral; green/blue only for states the controller has confirmed.
- **In progress:** accent colour plus spinner or text, never the success colour.
- **Warning/inhibit:** yellow/orange plus the cause.
- **Fault:** red plus icon plus description.
- **Link lost / stale / unknown:** grey, always with text.

Consistent wording: *request* is a command; *state* is controller data; *contact
feedback* is an independent sensor (absent on this board); *not confirmed* means no
evidence from the controller yet.

## 12. UI review checklist (before coding)

- Does every field have a source, unit, freshness and validity rule from
  `controller-interface.md`?
- Is command state separate from controller state?
- Do link loss, controller reboot, late replies, duplicate replies and protocol mismatch
  each have a defined display?
- Does a lost link during charging say that charging may continue?
- Is STOP reachable without closing a modal or navigating? Is "no authority" shown?
- Is STOP `REJECTED/QUEUE_FULL` never shown as "nothing done"?
- Does any page accidentally allow changing controller authority or configuration?
- Do labels fit 480 × 320 at the chosen font size; are touch targets large enough?
- Are relay command and contact feedback shown separately, feedback always "none"?
- Does the UI work with unsynchronized time and with an empty or overwritten log?
