# Commands and payloads

Control authority, the commands the HMI sends, byte layouts of every response, START/STOP semantics and events. Transport is in [framing-and-transport.md](framing-and-transport.md); code values in [code-tables.md](code-tables.md).

Cited extract, not a source. Same provenance, rule ("the ICD/firmware is right") and confidence labels as [framing-and-transport.md](framing-and-transport.md). Extracted from `docs/controller-interface.md` (checked against controller commit `f72ec7b`, 2026-10-06), then compared on 2026-11-27 with the controller ICD at `bc4f308`.

> **Corrections made after the split.** Three statements in the old page disagreed with the current controller documents and the HMI source, and are fixed below: GET_STATUS byte 19 and flag bit5 (CR-01) are defined, not reserved; the HMI reads `control_interface`; and the former condition A2 (`operational_node_authorized`, a build flag) no longer exists. Checked 2026-11-27 against the controller ICD §10 and §11.1, FDD-006 and FDD-011 §1.2 (branches `main` and `codex/rcc-studio-m1` agree on GET_STATUS byte 19).

## 1. Conditions for HMI control

Monitoring (PING, GET_*) is allowed from every interface. Control (START, STOP, SET_TIME) is allowed only from the **active control interface** (ICD §10). For an HMI on the operational UART all of the following must hold:

| # | Condition | Set by | State |
|---|---|---|---|
| A | CONFIG `operational_interface = OPERATIONAL_UART` (3) and `operational_node = 0` | service UART, through a controlled CONFIG transaction. Committing a CONFIG that names the peer **is** the authorization act (`confirmed`: FDD-006 operational-control row, FDD-011 §1.2, decided 2026-10-30). The seed CONFIG is `NONE` (monitor-only) | Demo CONFIG generation 2 names the operational UART, node 0 (owner decision 2026-11-20, `DEMO1-CTRL`). The 2026-10-06 bench was on RS485 (2), node `0x02` |
| B | for the relay to actually close in a session: `relay_enable_authorized = true` (GET_DEVICE_INFO build flag bit1) | controller build | only with `VERIFIED` limits or a `-DRCC_BENCH_RELAY_ENABLE=1` bench build (`confirmed`, FDD-011 §1.2) |

The former condition "A2 `operational_node_authorized`" and the build switch `RCC_BENCH_OPERATIONAL_NODE` were **retired on 2026-10-30**; the controller build now rejects the switch. GET_DEVICE_INFO build-flag bit2 is reserved and always 0 (ICD §11).

Consequences:

- **One control interface at a time.** While the HMI holds control, RS485/CAN (robot side) cannot START/STOP, and vice versa (`confirmed`, ICD §10).
- **Authority is reported.** GET_STATUS carries `control_interface` (byte 19) and flag bit5 `requester_is_control` (ICD, CR-01 assigned 2026-10-30; `confirmed` in the ICD and read by `rcc_link.c`). Against a controller **older** than CR-01 these fields are 0, which means "none/unknown". Treat that as unknown, not as denied. The HMI can still learn it lacks authority from `REJECTED/UNAUTHORIZED_SOURCE`.
- The emergency broadcast STOP exists only on RS485 and CAN; **UART has no broadcast** (ICD-DUP-008).
- `GET_CONFIG` is denied on the operational UART.

## 2. Commands the HMI uses

| Code | Command | Request payload | Allowed on the operational UART |
|---|---|---|---|
| `0x0001` | PING | token, 0–32 bytes | always |
| `0x0002` | GET_DEVICE_INFO | empty | always |
| `0x0010` | GET_STATUS | empty | always |
| `0x0011` | GET_MEASUREMENTS | empty | always |
| `0x0012` | GET_FAULTS | empty | always |
| `0x0013` | GET_EVENT_LOG | 8 bytes: `cursor` u32, `max_count` u8, 3 zero bytes | always |
| `0x0020` | START_CHARGE | empty | only with the authority in §1 |
| `0x0021` | STOP_CHARGE | empty | only with the authority in §1 |

`SET_TIME` (`0x0022`, `utc_ms` u64) exists but **the HMI does not send it**, so controller event times stay relative (the controller's UTC annotation stays unsynchronised). A payload of the wrong length is `REJECTED/MALFORMED_MESSAGE`. CONFIG, CAL and CLEAR_FAULT are always denied on the operational UART (ICD §10); the HMI never sends them.

## 3. COMMAND_RESULT

### 3.1 12-byte header (start of every result payload; ICD §11.1.1)

| Offset | Field | Type |
|---|---|---|
| 0 | `result` | u8, [code-tables §1](code-tables.md#1-result) |
| 1 | reserved | u8 |
| 2 | `reason_code` | u16 LE, [§2](code-tables.md#2-reason) |
| 4 | `top_state` | u8, [§3](code-tables.md#3-two-level-state) |
| 5 | `operational_state` | u8, [§3](code-tables.md#3-two-level-state) |
| 6 | `primary_fault` | u16 LE, [§6](code-tables.md#6-faults) |
| 8 | `inhibit_mask` | u32 LE, [§5](code-tables.md#5-inhibit-mask) |

`response_data` follows the header. Every result therefore carries the current state, primary fault and inhibit mask; the HMI may use them to refresh the UI between polls.

### 3.2 GET_STATUS, 48 bytes (schema 1)

| Offset | Field | Type |
|---|---|---|
| 0 | `snapshot_sequence` | u32 |
| 4 | `boot_id` | u64 |
| 12 | `active_fault_mask` | u32, [code-tables §6](code-tables.md#6-faults) |
| 16 | `relay_command` | u8, [code-tables §4](code-tables.md#4-relay) |
| 17 | `relay_feedback` | u8, [code-tables §4](code-tables.md#4-relay) |
| 18 | flags: bit0 `charging_established`, bit1 `time_synchronized`, bit2 `cal_lease_active`, bit3 `simulated_measurements`, bit4 `simulated_ready`, **bit5 `requester_is_control`**, bits 6–7 reserved | u8 |
| 19 | **`control_interface`**: the active control interface the CONFIG binds, `source_interface` values (0 = none, monitor-only, or no valid CONFIG). Read-only; grants nothing | u8 |
| 20 | `session_id` | u32 |
| 24 | `config_revision` | u32 |
| 28 | `calibration_revision` | u32 |
| 32 | `hardware_revision` | u32 |
| 36 | `firmware_revision` | u32 |
| 40 | effective profiles: CAN id, CAN rev, RS485 id, RS485 rev | 4 × u16 |

Bold rows differ from the old page, which listed byte 19 as reserved and omitted bit5. Source: ICD at `bc4f308`; HMI: `rcc_link.c` line 237, field `control_interface` in `rcc_types.h`.

### 3.3 GET_MEASUREMENTS, 32 bytes (schema 1)

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

Channel status bits: bit0 present, bit1 fresh, bit2 calibrated, bit3 in-range, bit4 not-saturated, bit5 plausible. **A value is usable only when all six are set.** Bit6 below-floor (VOUT only, informational): the value is the channel measurement floor and the true VOUT is **at or below** it; on the 2026-10-06 bench the floor was about 3.38 V with no charger. Before the first snapshot is published the result is `FAILED/MEASUREMENT_INVALID` with no response data.

The controller publishes a measurement every 10 ms and treats it as stale after 30 ms (`provisional`); `age_ms` is the age when the controller answered, without UART delay.

### 3.4 GET_FAULTS, 12 bytes (schema 1)

| Offset | Field | Type |
|---|---|---|
| 0 | `active_fault_mask` | u32 |
| 4 | `inhibit_mask` | u32 |
| 8 | `primary_fault` | u16 |
| 10 | reserved | u16 |

### 3.5 GET_DEVICE_INFO, 40 bytes (schema 1)

| Offset | Field | Type |
|---|---|---|
| 0 | `product_id` (`0x0001` = Robot Charge Controller) | u16 |
| 2 | `protocol_version` | u8 |
| 3 | build flags: bit0 provisional build profile, bit1 `relay_enable_authorized`, bit2 **reserved, always 0** (formerly "operational node authorized", retired 2026-10-30) | u8 |
| 4 | `hardware_revision` | u32 |
| 8 | `firmware_revision` | u32 |
| 12 | first 8 bytes of the image SHA-256 | 8 bytes |
| 20 | `boot_id` | u64 |
| 28 | running links: bit0 CAN, bit1 RS485, bit2 operational UART, bit3 service UART | u32 |
| 32 | effective profiles: CAN id, CAN rev, RS485 id, RS485 rev | 4 × u16 |

Minimum compatibility check: `product_id == 0x0001` and `protocol_version == 1`. GET_DEVICE_INFO does **not** report the active control interface; use GET_STATUS byte 19.

### 3.6 GET_EVENT_LOG (schema 1)

The log lives in controller RAM, holds the last 32 events (`provisional`), is **lost when the controller restarts**, and never contains TELEMETRY.

| Offset | Field | Type |
|---|---|---|
| 0 | `next_cursor` | u32 |
| 4 | `oldest_seq` still held (0 when empty) | u32 |
| 8 | `missed` (entries at or after `cursor` already overwritten) | u32 |
| 12 | `count` | u8 |
| 13 | reserved | 3 bytes |
| 16 | `count` × (`length` u8 + an event payload of `length` bytes, §5) | — |

`cursor = 0` starts at the oldest entry. A page holds at most 224 bytes of entries.

## 4. START / STOP semantics (ICD §8.2, §11.2, §13)

### 4.1 Result life cycle

| Result | Terminal | Meaning |
|---|---|---|
| `ACCEPTED` | no | passed source/frame checks and admitted to the control workflow. It does **not** mean the relay is closed |
| `REJECTED` | yes | not admitted; no action started (STOP is the exception, §4.3) |
| `COMPLETED` | yes | the command's own success boundary was reached |
| `FAILED` | yes | admitted, but the success boundary was not reached |

An `ACCEPTED` command always gets exactly one terminal result with the same `request_id`. It **may arrive seconds later** and arrives unsolicited: the HMI does not ask again. The only exception is a controller restart, detected by a changed `boot_id`.

### 4.2 START_CHARGE

- `ACCEPTED` only while a new session can still start from it: `OPERATIONAL/IDLE`, `VREQ_VALIDATE` or `PRECHECK` with no session armed, or `INHIBITED` when the START is allowed to clear the inhibit; and no other START is waiting for its result. Otherwise `REJECTED/INVALID_STATE`.
- No charge request (VOUT below `VREQ_ON` with the relay open) → `REJECTED/CHARGER_REQUEST_ABSENT`; no session is created.
- `COMPLETED` once charge current is established, the ACTIVE record is durable and the state is `CHARGING`. Everything after that (completion, maximum duration, fault, STOP) shows only as state and events; there is **no** second result for the START.
- `FAILED` reasons: `CHARGE_NOT_ESTABLISHED`, `ABORTED_BY_STOP`, `ABORTED_BY_FAULT`, `INTERLOCK_FAILED`, `CHARGER_REQUEST_ABSENT`, `PERSISTENCE_FAILED`, `CONFIG_INVALID`, `CAL_INVALID`, `INVALID_STATE`, `INTERNAL_ERROR` (cause table: ICD §11.2).
- A START result may be held for one control iteration; meanwhile a new START is `REJECTED/INVALID_STATE`.

### 4.3 STOP_CHARGE

- Relay OFF is commanded **before** persistence and before the reply.
- `COMPLETED` only after `REMOTE_INHIBIT` is durably written to flash. If the write fails: `FAILED/PERSISTENCE_FAILED`, but the relay **has still been commanded OFF**.
- A repeated STOP while flash already holds `REMOTE_INHIBIT` is answered at once and writes nothing.
- When the STOP ledger is full (4 entries, `provisional`): the STOP **still takes effect** (relay OFF, `REMOTE_INHIBIT`) but is answered at once with `REJECTED/QUEUE_FULL`. For STOP, this `REJECTED` means "cannot be tracked, send it again", **not** "nothing was done".
- A STOP reply confirms the command and persistence boundaries, **not** that the contacts opened (the board has no contact feedback).

### 4.4 Loss of communication

ICD §19: losing communication does **not** end a valid charging session. If the HMI loses its link or powers off while charging, the controller keeps charging until the session ends by itself (completion, maximum duration, fault, loss of the charge source) or a STOP arrives from the active control interface (`confirmed`).

## 5. Events (GET_EVENT_LOG)

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
| `0x0015` / `0x0016` | SESSION_COMPLETED / SESSION_ABORTED | `session_id` u32, `terminal_reason` u8 ([code-tables §7](code-tables.md#7-other-enumerations)), 3 reserved, `duration_ms` u32, `vout_mv` u32, `iout_ma` i32 |
| `0x0020` | CONFIG_CHANGED | old rev u16, new rev u16, old gen u32, new gen u32, source u8, result u8, reserved u16 |
| `0x0021` | CAL_CHANGED | old gen u32, new gen u32, hw rev u32, source u8, result u8, reserved u16 |
| `0x0030` | COMMUNICATION_DEGRADED | interface u8, error state u8, reserved u16, frames dropped u32 |
| `0x0031` | PERSISTENCE_HEALTH_CHANGED | domain u16, healthy u8, reserved u8, last status i32, failures u32 |
| `0x0032` | DIAGNOSTIC_LOSS | 3 × u32 counters |
| `0x0033` | MEASUREMENT_FAULT | 32 bytes, see ICD §14.2 |
| `0x0040` | TIME_SYNC_CHANGED | synced u8, source u8, reserved u16, `utc_ms` u64 |
| `0x0050` | TELEMETRY | never in the log |

`STATE_CHANGED`, `INHIBIT_CHANGED` and `FAULT_CHANGED` are coalesced and rate-limited (held 100 ms, at most one per kind per 500 ms): the log is **not** a complete record of every change. Session events are exact and never rate-limited. Unknown codes are shown raw.
