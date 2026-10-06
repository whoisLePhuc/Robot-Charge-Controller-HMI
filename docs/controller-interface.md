# HMI ↔ Robot Charge Controller interface (operational UART)

Everything the HMI needs to know about the controller, in one place: the conditions for
control authority, framing, byte layouts of every command and response, code tables and
START/STOP semantics. This page is a **cited extract** of the controller documentation,
not its source.

| Item | Value |
|---|---|
| Checked against | repository `robot_charge_controller`, commit `f72ec7b` (firmware at `bae44f7`), 2026-10-06 |
| Normative sources | `docs/firmware/interfaces/firmware-interface-control-document.md` (ICD), `CAN.md` §5, §8, §9, `RS485.md` §4–§5, `UART.md`, plus the firmware headers named in each table |
| Rule | Where this page and the ICD or controller firmware differ, **the ICD/firmware is right** and this page is stale. Update this page and the golden vectors together |
| Confidence labels | `confirmed` (in the ICD or firmware source), `inferred` (derived, not measured), `not-frozen` (in firmware source but **not** frozen by the ICD; may change) |

## 1. Conditions for HMI control

Monitoring (PING, GET_*) is allowed from every interface. Control (START, STOP,
SET_TIME) is allowed only from the **active control interface** (ICD §10). For an HMI
on the operational UART all of the following must hold:

| # | Condition | Set by | State on 2026-10-06 |
|---|---|---|---|
| A1 | CONFIG `operational_interface = OPERATIONAL_UART` (3) and `operational_node = 0` | service UART, through a controlled CONFIG transaction | bench is on RS485 (2), node `0x02`; change required by [HMI-D01](decisions.md#hmi-d01-role-of-the-hmi) |
| A2 | build profile `operational_node_authorized = true` (GET_DEVICE_INFO build flag bit2) | controller build | bench image `-DRCC_BENCH_OPERATIONAL_NODE=1` per [HMI-D02](decisions.md#hmi-d02-control-authority); **no production path** (`FDD10-OPEN-005`, `ICD-OPEN-006`) |
| A3 | for the relay to actually close in a session: `relay_enable_authorized = true` (build flag bit1) | controller build | only with `VERIFIED` limits or a `-DRCC_BENCH_RELAY_ENABLE=1` bench build |

Consequences:

- **One control interface at a time.** While the HMI holds control, RS485/CAN (robot
  side) cannot START/STOP, and vice versa (`confirmed`, ICD §10).
- **The HMI cannot know its authority in advance.** `GET_CONFIG` is denied on the
  operational UART; GET_DEVICE_INFO reports A2 but not A1. The HMI learns that it lacks
  authority only from `REJECTED/UNAUTHORIZED_SOURCE` (`confirmed`). See
  [controller-change-requests.md](controller-change-requests.md) CR-01.
- The emergency broadcast STOP exists only on RS485 and CAN; **UART has no broadcast**
  (ICD-DUP-008).

## 2. Physical layer (controller operational UART)

| Item | Value | Source |
|---|---|---|
| Controller port | UART2, TX = GPIO19, RX = GPIO21 (connector J2) | firmware `rcc_board_binding.h`, `confirmed` |
| Logic level | 3.3 V, not RS-232 | ICD §18.1 |
| Character format | 115 200 bit/s, 8N1, no flow control | `UART.md` §3.1 |
| Topology | point-to-point, full duplex | `UART.md` §3.1 |

HMI side: [hmi-hardware.md §4](hmi-hardware.md#4-link-to-the-controller).

## 3. Framing

```text
wire      = 0x00 || COBS(raw_frame) || 0x00
raw_frame = serial_header(12) || fragment_data(1..128) || crc16(2, LE)
```

### 3.1 Serial header (`RS485.md` §4.2, used unchanged on UART)

| Offset | Field | Type | Rule |
|---|---|---|---|
| 0 | `frame_version` | u8 | `0x01` |
| 1 | `frame_type` | u8 | `0x01` SINGLE, `0x02` OBJECT_FIRST, `0x03` OBJECT_DATA |
| 2 | `destination` | u8 | `0x01` (controller) for every HMI frame |
| 3 | `source` | u8 | a fixed HMI value; **must not be `0x01`** (the controller drops frames claiming its own address) nor `0xFF`. The PC tools use `0x02`. On UART it only addresses the replies; the trusted identity is the port, node 0 |
| 4 | `request_id` | u32 LE | equals the object's `request_id`; non-zero for COMMAND |
| 8 | `canonical_object_length` | u16 LE | 16–512, identical on every frame of one object |
| 10 | `fragment_index` | u8 | 0 for SINGLE/OBJECT_FIRST; +1 per OBJECT_DATA |
| 11 | reserved | u8 | 0 |

- CRC-16/CCITT-FALSE (poly `0x1021`, init `0xFFFF`, no reflection, xorout 0,
  `check("123456789") = 0x29B1`) over the COBS-decoded `raw_frame` except the CRC
  bytes.
- An object of ≤ 128 bytes travels in one SINGLE frame. Larger objects: OBJECT_FIRST
  then OBJECT_DATA; every fragment except the last carries a full 128 bytes
  (`RS485.md` §5).
- The longest reply (a GET_EVENT_LOG page, object ≤ 268 bytes) takes **3 frames**. The
  HMI must reassemble multi-frame objects from the start.
- Reassembly key `(source, destination, request_id)`; a missing, repeated or
  out-of-order fragment aborts the whole transfer; a partial transfer expires after 2 s.

### 3.2 Canonical object (`CAN.md` §5)

| Offset | Field | Type |
|---|---|---|
| 0 | `protocol_version` | u8, `0x01` |
| 1 | `message_type` | u8: `0x01` COMMAND, `0x02` COMMAND_RESULT, `0x03` EVENT |
| 2 | `flags` | u16 LE, **must be 0** in V1 |
| 4 | `command_code` | u16 LE (§4); always `0x0000` for EVENT |
| 6 | `request_id` | u32 LE |
| 10 | `payload_length` | u16 LE = `canonical_object_length − 16` |
| 12 | `payload` | 0–496 bytes |
| end | `crc32` | u32 LE, CRC-32/ISO-HDLC over the whole object except itself |

Decode order (`CAN.md` §5.2): length → **CRC-32 before any interpretation** → version
→ `payload_length` → field rules. Golden vectors: `CAN.md` §5.3 and
`firmware/tests/host/test_rcc_serial_frame.c` (an independently produced wire vector).
The HMI host tests should reuse exactly these vectors.

## 4. Commands the HMI uses

| Code | Command | Request payload | Allowed on the operational UART |
|---|---|---|---|
| `0x0001` | PING | token, 0–32 bytes | always |
| `0x0002` | GET_DEVICE_INFO | empty | always |
| `0x0010` | GET_STATUS | empty | always |
| `0x0011` | GET_MEASUREMENTS | empty | always |
| `0x0012` | GET_FAULTS | empty | always |
| `0x0013` | GET_EVENT_LOG | 8 bytes: `cursor` u32, `max_count` u8, 3 zero bytes | always |
| `0x0020` | START_CHARGE | empty | only with A1 + A2 (§1) |
| `0x0021` | STOP_CHARGE | empty | only with A1 + A2 |

`SET_TIME` (`0x0022`, `utc_ms` u64) exists but the HMI does not send it
([HMI-D07](decisions.md#hmi-d07-time-source)). A payload of the wrong length is
`REJECTED/MALFORMED_MESSAGE`. CONFIG/CAL/CLEAR_FAULT are always denied on the
operational UART (ICD §10); the HMI never sends them.

## 5. COMMAND_RESULT

### 5.1 12-byte header (start of every result payload; ICD §11.1.1)

| Offset | Field | Type |
|---|---|---|
| 0 | `result` | u8, §8.1 |
| 1 | reserved | u8 |
| 2 | `reason_code` | u16 LE, §8.2 |
| 4 | `top_state` | u8, §8.3 |
| 5 | `operational_state` | u8, §8.3 |
| 6 | `primary_fault` | u16 LE, §8.6 |
| 8 | `inhibit_mask` | u32 LE, §8.5 |

`response_data` follows the header. Every result therefore carries the current state,
primary fault and inhibit mask; the HMI may use them to refresh the UI between polls.

### 5.2 GET_STATUS, 48 bytes (schema 1)

| Offset | Field | Type |
|---|---|---|
| 0 | `snapshot_sequence` | u32 |
| 4 | `boot_id` | u64 |
| 12 | `active_fault_mask` | u32, §8.6 |
| 16 | `relay_command` | u8, §8.4 |
| 17 | `relay_feedback` | u8, §8.4 |
| 18 | flags: bit0 `charging_established`, bit1 `time_synchronized`, bit2 `cal_lease_active` | u8 |
| 19 | reserved | u8 |
| 20 | `session_id` | u32 |
| 24 | `config_revision` | u32 |
| 28 | `calibration_revision` | u32 |
| 32 | `hardware_revision` | u32 |
| 36 | `firmware_revision` | u32 |
| 40 | effective profiles: CAN id, CAN rev, RS485 id, RS485 rev | 4 × u16 |

### 5.3 GET_MEASUREMENTS, 32 bytes (schema 1)

| Offset | Field | Type |
|---|---|---|
| 0 | `measurement_sequence` | u32 |
| 4 | `age_ms` (saturates at `0xFFFFFFFF`) | u32 |
| 8 | `vout_mv` | u32 |
| 12 | `iout_ma` | **i32** (signed) |
| 16 | `vout_filtered_mv` | u32 |
| 20 | `iout_filtered_ma` | i32 |
| 24 | `vout_status` | u16 |
| 26 | `iout_status` | u16 |
| 28 | `snapshot_status` | u32 (bits not defined by the ICD; the HMI shows it raw) |

Channel status bits: bit0 present, bit1 fresh, bit2 calibrated, bit3 in-range, bit4
not-saturated, bit5 plausible. **A value is usable only when all six are set.** Bit6
below-floor (VOUT only, informational): the value is the channel measurement floor and
the true VOUT is **at or below** it; on the current bench the floor is about 3.38 V with
no charger. Before the first snapshot is published the result is
`FAILED/MEASUREMENT_INVALID` with no response data.

The controller publishes a measurement every 10 ms and treats it as stale after 30 ms
(`provisional`); `age_ms` is the age when the controller answered, without UART delay.

### 5.4 GET_FAULTS, 12 bytes (schema 1)

| Offset | Field | Type |
|---|---|---|
| 0 | `active_fault_mask` | u32 |
| 4 | `inhibit_mask` | u32 |
| 8 | `primary_fault` | u16 |
| 10 | reserved | u16 |

### 5.5 GET_DEVICE_INFO, 40 bytes (schema 1)

| Offset | Field | Type |
|---|---|---|
| 0 | `product_id` (`0x0001` = Robot Charge Controller) | u16 |
| 2 | `protocol_version` | u8 |
| 3 | build flags: bit0 provisional, bit1 `relay_enable_authorized`, bit2 operational node authorized | u8 |
| 4 | `hardware_revision` | u32 |
| 8 | `firmware_revision` | u32 |
| 12 | first 8 bytes of the image SHA-256 | 8 bytes |
| 20 | `boot_id` | u64 |
| 28 | running links: bit0 CAN, bit1 RS485, bit2 operational UART, bit3 service UART | u32 |
| 32 | effective profiles: CAN id, CAN rev, RS485 id, RS485 rev | 4 × u16 |

Minimum compatibility check: `product_id == 0x0001` and `protocol_version == 1`.
GET_DEVICE_INFO does **not** report the active control interface (§1).

### 5.6 GET_EVENT_LOG (schema 1)

The log lives in controller RAM, holds the last 32 events (`provisional`), is **lost
when the controller restarts**, and never contains TELEMETRY.

| Offset | Field | Type |
|---|---|---|
| 0 | `next_cursor` | u32 |
| 4 | `oldest_seq` still held (0 when empty) | u32 |
| 8 | `missed` (entries at or after `cursor` already overwritten) | u32 |
| 12 | `count` | u8 |
| 13 | reserved | 3 bytes |
| 16 | `count` × (`length` u8 + an event payload of `length` bytes, §7) | — |

`cursor = 0` starts at the oldest entry. A page holds at most 224 bytes of entries.

## 6. START / STOP semantics (ICD §8.2, §11.2, §13)

### 6.1 Result life cycle

| Result | Terminal | Meaning |
|---|---|---|
| `ACCEPTED` | no | passed source/frame checks and admitted to the control workflow. It does **not** mean the relay is closed |
| `REJECTED` | yes | not admitted; no action started (STOP is the exception, §6.3) |
| `COMPLETED` | yes | the command's own success boundary was reached |
| `FAILED` | yes | admitted, but the success boundary was not reached |

An `ACCEPTED` command always gets exactly one terminal result with the same
`request_id`. It **may arrive seconds later** and arrives unsolicited: the HMI does not
ask again. The only exception is a controller restart, detected by a changed `boot_id`.

### 6.2 START_CHARGE

- `ACCEPTED` only while a new session can still start from it: `OPERATIONAL/IDLE`,
  `VREQ_VALIDATE` or `PRECHECK` with no session armed, or `INHIBITED` when the START is
  allowed to clear the inhibit; and no other START is waiting for its result. Otherwise
  `REJECTED/INVALID_STATE`.
- No charge request (VOUT below `VREQ_ON` with the relay open) →
  `REJECTED/CHARGER_REQUEST_ABSENT`; no session is created.
- `COMPLETED` once charge current is established, the ACTIVE record is durable and the
  state is `CHARGING`. Everything after that (completion, maximum duration, fault, STOP)
  shows only as state and events; there is **no** second result for the START.
- `FAILED` reasons: `CHARGE_NOT_ESTABLISHED`, `ABORTED_BY_STOP`, `ABORTED_BY_FAULT`,
  `INTERLOCK_FAILED`, `CHARGER_REQUEST_ABSENT`, `PERSISTENCE_FAILED`,
  `CONFIG_INVALID`, `CAL_INVALID`, `INVALID_STATE`, `INTERNAL_ERROR` (cause table: ICD
  §11.2).
- A START result may be held for one control iteration; meanwhile a new START is
  `REJECTED/INVALID_STATE`.

### 6.3 STOP_CHARGE

- Relay OFF is commanded **before** persistence and before the reply.
- `COMPLETED` only after `REMOTE_INHIBIT` is durably written to flash. If the write
  fails: `FAILED/PERSISTENCE_FAILED`, but the relay **has still been commanded OFF**.
- A repeated STOP while flash already holds `REMOTE_INHIBIT` is answered at once and
  writes nothing.
- When the STOP ledger is full (4 entries, `provisional`): the STOP **still takes
  effect** (relay OFF, `REMOTE_INHIBIT`) but is answered at once with
  `REJECTED/QUEUE_FULL`. For STOP, this `REJECTED` means "cannot be tracked, send it
  again", **not** "nothing was done".
- A STOP reply confirms the command and persistence boundaries, **not** that the
  contacts opened (the current board has no contact feedback).

### 6.4 Loss of communication

ICD §19: losing communication does **not** end a valid charging session. If the HMI
loses its link or powers off while charging, the controller keeps charging until the
session ends by itself (completion, maximum duration, fault, loss of the charge source)
or a STOP arrives from the active control interface (`confirmed`).

## 7. Events (GET_EVENT_LOG)

28-byte header of every event (ICD §14.2):

| Offset | Field | Type |
|---|---|---|
| 0 | `event_code` | u16 |
| 2 | flags: bit0 `time_synchronized` | u8 |
| 3 | reserved | u8 |
| 4 | `event_seq` (starts at 1 each boot) | u32 |
| 8 | `boot_id` | u64 |
| 16 | `monotonic_us` | u64 |
| 24 | `origin_request_id` (0 when none) | u32 |

| Code | Event | Data after the header |
|---|---|---|
| `0x0001` | BOOT_COMPLETED | `reset_class` u8, `top_state` u8, reserved u16, `hardware_revision` u32, `firmware_revision` u32 |
| `0x0010` | STATE_CHANGED | old top u8, old op u8, new top u8, new op u8 |
| `0x0011` | INHIBIT_CHANGED | old mask u32, new mask u32 |
| `0x0012` | FAULT_CHANGED | old mask u32, new mask u32, `primary_fault` u16, reserved u16 |
| `0x0013` | SESSION_ARMED | `session_id` u32, `start_kind` u8, `source_interface` u8, reserved u16 |
| `0x0014` | CHARGE_ESTABLISHED | `session_id` u32, `vout_mv` u32, `iout_ma` i32 |
| `0x0015` / `0x0016` | SESSION_COMPLETED / SESSION_ABORTED | `session_id` u32, `terminal_reason` u8 (§8.7), 3 reserved, `duration_ms` u32, `vout_mv` u32, `iout_ma` i32 |
| `0x0020` | CONFIG_CHANGED | old rev u16, new rev u16, old gen u32, new gen u32, source u8, result u8, reserved u16 |
| `0x0021` | CAL_CHANGED | old gen u32, new gen u32, hw rev u32, source u8, result u8, reserved u16 |
| `0x0030` | COMMUNICATION_DEGRADED | interface u8, error state u8, reserved u16, frames dropped u32 |
| `0x0031` | PERSISTENCE_HEALTH_CHANGED | domain u16, healthy u8, reserved u8, last status i32, failures u32 |
| `0x0032` | DIAGNOSTIC_LOSS | 3 × u32 counters |
| `0x0033` | MEASUREMENT_FAULT | 32 bytes, see ICD §14.2 |
| `0x0040` | TIME_SYNC_CHANGED | synced u8, source u8, reserved u16, `utc_ms` u64 |
| `0x0050` | TELEMETRY | never in the log |

`STATE_CHANGED`, `INHIBIT_CHANGED` and `FAULT_CHANGED` are coalesced and rate-limited
(held 100 ms, at most one per kind per 500 ms): the log is **not** a complete record of
every change. Session events are exact and never rate-limited. Unknown codes are shown
raw.

## 8. Code tables

### 8.1 Result (`CAN.md` §9.3)

`0x00` ACCEPTED · `0x01` REJECTED · `0x02` COMPLETED · `0x03` FAILED

### 8.2 Reason (`CAN.md` §9.4)

| Code | Name | Code | Name |
|---|---|---|---|
| `0x0000` | NONE | `0x000B` | INTERLOCK_FAILED |
| `0x0001` | UNSUPPORTED_VERSION | `0x000C` | MEASUREMENT_INVALID |
| `0x0002` | MALFORMED_MESSAGE | `0x000D` | CONFIG_INVALID |
| `0x0003` | INTEGRITY_FAILED | `0x000E` | CAL_INVALID |
| `0x0004` | UNSUPPORTED_COMMAND | `0x000F` | PERSISTENCE_FAILED |
| `0x0005` | UNAUTHORIZED_SOURCE | `0x0010` | CHARGER_REQUEST_ABSENT |
| `0x0006` | DUPLICATE_ID_CONFLICT | `0x0011` | TIMEOUT |
| `0x0007` | QUEUE_FULL | `0x0012` | CHARGE_NOT_ESTABLISHED |
| `0x0008` | INVALID_STATE | `0x0013` | ABORTED_BY_STOP |
| `0x0009` | INHIBITED | `0x0014` | ABORTED_BY_FAULT |
| `0x000A` | FAULT_ACTIVE | `0x0015` | INTERNAL_ERROR |
| | | `0x0016` | RELAY_FEEDBACK_MISMATCH (reserved; never returned by current firmware) |

### 8.3 Two-level state (firmware `rcc_states.h`, `confirmed`)

| `top_state` | Name | | `operational_state` | Name |
|---|---|---|---|---|
| 0 | BOOT_SAFE | | 0 | NONE (outside OPERATIONAL) |
| 1 | SELF_TEST | | 1 | IDLE |
| 2 | SERVICE_LOCK | | 2 | VREQ_VALIDATE |
| 3 | CONFIG_MODE | | 3 | PRECHECK |
| 4 | OPERATIONAL | | 4 | RELAY_CLOSING |
| 5 | INHIBITED | | 5 | CHARGE_VERIFY |
| 6 | LATCHED_FAULT | | 6 | CHARGING |
| | | | 7 | COMPLETE |
| | | | 8 | WAIT_REARM |

`operational_state` is meaningful only when `top_state = OPERATIONAL`. There is no state
named `ACTIVE` or `FAULT`.

### 8.4 Relay (`rcc_relay_types.h`, `confirmed`)

`relay_command`: 0 OFF, 1 ON. `relay_feedback`: 0 UNKNOWN, 1 OPEN, 2 CLOSED, 3 CONFLICT.
The current board has **no contact feedback**: `relay_feedback` is always 0 UNKNOWN.

### 8.5 Inhibit mask (`rcc_inhibit.h`, `not-frozen`)

bit0 REMOTE (set by STOP), bit1 RESET (after a watchdog/panic/brownout reset), bit2
RECOVERY (after a recoverable fault). Any other bit: show raw, never drop it.

### 8.6 Faults (`rcc_fault_registry.h`; IDs `confirmed` by FDD-004, bit positions `not-frozen`)

| Bit in `active_fault_mask` | ID (`primary_fault`) | Name |
|---:|---|---|
| 0 | `0x0101` | CONTROL_INTERNAL |
| 1 | `0x0102` | NVS_WRITE_FAILED |
| 2 | `0x0201` | RELAY_COMMAND_FAILED |
| 3 | `0x0202` | RELAY_FEEDBACK_CONFLICT |
| 4 | `0x0301` | ADC_UNAVAILABLE |
| 5 | `0x0302` | ADC_STALE |
| 6 | `0x0303` | ADC_PATTERN_INVALID |
| 7 | `0x0304` | ADC_SATURATED |
| 8 | `0x0305` | VOUT_INVALID |
| 9 | `0x0306` | IOUT_INVALID |
| 10 | `0x0401` | VOUT_OVERVOLTAGE |
| 11 | `0x0402` | IOUT_OVERCURRENT |
| 12 | `0x0403` | IOUT_REVERSE_CURRENT |
| 13 | `0x0501` | CHARGE_NOT_ESTABLISHED |
| 14 | `0x0502` | CHARGE_MAX_DURATION |
| 15 | `0x0601` | CAN_DEGRADED |
| 16 | `0x0602` | RS485_DEGRADED |
| 17 | `0x0603` | OPERATIONAL_UART_DEGRADED |
| 18 | `0x0604` | DIAGNOSTIC_DEGRADED |

`primary_fault = 0` means no fault. Real example from the 2026-10-06 bench:
`active_fault_mask = 0x2A0` = ADC_STALE + ADC_SATURATED + IOUT_INVALID.

### 8.7 Other enumerations

| Enumeration | Values |
|---|---|
| `reset_class` | 0 UNKNOWN, 1 POWER_ON, 2 EXPLICIT_SOFTWARE, 3 WATCHDOG, 4 PANIC, 5 BROWNOUT, 6 OTHER |
| `source_interface` / CONFIG `operational_interface` | 0 NONE, 1 CAN, 2 RS485, 3 OPERATIONAL_UART, 4 SERVICE_UART |
| `terminal_reason` | 1 COMPLETE, 2 REMOTE_STOP, 3 RECOVERABLE_FAULT, 4 LATCHED_FAULT, 5 NOT_ESTABLISHED, 6 MAX_DURATION, 7 INTERRUPTED_RESET, 8 START_ABORTED, 9 CHARGE_LOST |

## 9. Timing, retry and duplicate handling

| Item | Value | Source |
|---|---|---|
| Reply timeout per attempt | 50 ms | `UART.md` §7 |
| Transport-level retransmissions | 2 (about 150 ms in total), **same `request_id`, same content** | `UART.md` §7 |
| Ledger result retention (`router_retention_us`) | 2 s, `provisional` | `ICD-OPEN-005`; firmware profile |
| Normal / STOP ledger depth | 8 / 4, `provisional` | firmware `rcc_build_profile.h` |
| Duplicate key | `(interface, node, request_id)` | ICD-DUP-001 |

- Same key, same content → the cached result, no repeated action. Same key, different
  content → `REJECTED/DUPLICATE_ID_CONFLICT`.
- **A resend after the ledger has forgotten (> 2 s) is a new command.** Automatic
  retries must stay inside the ~150 ms window.
- Queries (PING, GET_*) bypass the ledger and are answered afresh; repeating them is safe.
- Bench observation 2026-10-06 (`inferred` for the HMI): queries over the operational
  UART had a p50 latency of about 60 ms including USB and the PC; 30 back-to-back queries
  with no gap were all answered.

## 10. Not yet stable (the HMI must tolerate change)

| Item | Status |
|---|---|
| Fault and inhibit bit positions | `not-frozen` (CR-02) |
| Retention, ledger depths, timeouts | `provisional` (`ICD-OPEN-005`) |
| Reporting the active control interface | absent (CR-01) |
| Unsolicited events on UART | firmware sends none; `UART.md` §7 says they are "allowed" (CR-03). The HMI polls |
| `snapshot_status` | bits not defined by the ICD |
| Production authorization of the operational node | none (`FDD10-OPEN-005`, CR-04) |
