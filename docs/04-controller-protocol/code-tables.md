# Code tables

Numeric codes the HMI decodes, and which of them may still change. Layouts that use these codes are in [commands-and-payloads.md](commands-and-payloads.md).

Cited extract, not a source. Provenance, the "ICD/firmware is right" rule and the confidence labels (`confirmed`, `inferred`, `not-frozen`, `provisional`) are defined at the top of [framing-and-transport.md](framing-and-transport.md). Carried over from `docs/controller-interface.md` (controller commit `f72ec7b`, 2026-10-06); **not re-checked against the current controller headers**, so confirm in `rcc_fault_registry.h` before depending on a bit position.

## 1. Result

(`CAN.md` §9.3) `0x00` ACCEPTED · `0x01` REJECTED · `0x02` COMPLETED · `0x03` FAILED

## 2. Reason

(`CAN.md` §9.4)

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

## 3. Two-level state

(firmware `rcc_states.h`, `confirmed`)

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

`operational_state` is meaningful only when `top_state = OPERATIONAL`. There is no state named `ACTIVE` or `FAULT`.

## 4. Relay

(`rcc_relay_types.h`, `confirmed`) `relay_command`: 0 OFF, 1 ON. `relay_feedback`: 0 UNKNOWN, 1 OPEN, 2 CLOSED, 3 CONFLICT.

The board has **no contact feedback**: `relay_feedback` is always 0 UNKNOWN. The HMI must never present "relay command ON" as "contacts closed".

## 5. Inhibit mask

(`rcc_inhibit.h`, `not-frozen`) bit0 REMOTE (set by STOP), bit1 RESET (after a watchdog/panic/brownout reset), bit2 RECOVERY (after a recoverable fault). Any other bit: show raw, never drop it.

## 6. Faults

(`rcc_fault_registry.h`; IDs `confirmed` by FDD-004, bit positions `not-frozen`)

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

`primary_fault = 0` means no fault. Example from the 2026-10-06 bench: `active_fault_mask = 0x2A0` = ADC_STALE + ADC_SATURATED + IOUT_INVALID.

## 7. Other enumerations

| Enumeration | Values |
|---|---|
| `reset_class` | 0 UNKNOWN, 1 POWER_ON, 2 EXPLICIT_SOFTWARE, 3 WATCHDOG, 4 PANIC, 5 BROWNOUT, 6 OTHER |
| `source_interface` / CONFIG `operational_interface` | 0 NONE, 1 CAN, 2 RS485, 3 OPERATIONAL_UART, 4 SERVICE_UART |
| `terminal_reason` | 1 COMPLETE, 2 REMOTE_STOP, 3 RECOVERABLE_FAULT, 4 LATCHED_FAULT, 5 NOT_ESTABLISHED, 6 MAX_DURATION, 7 INTERRUPTED_RESET, 8 START_ABORTED, 9 CHARGE_LOST |

## 8. What is not stable

The HMI must tolerate change in these.

| Item | Status |
|---|---|
| Fault and inhibit bit positions | `not-frozen` ([CR-02](controller-change-requests.md#cr-02-freeze-bit-positions)) |
| Retention, ledger depths, timeouts | `provisional` (`ICD-OPEN-005`; [CR-05](controller-change-requests.md#cr-05-retention-and-timeouts)) |
| Reporting the active control interface | **Now defined** in GET_STATUS (CR-01, 2026-10-30). Older controllers report 0 |
| Unsolicited events on UART | firmware sends none; `UART.md` §7 says they are "allowed" ([CR-03](controller-change-requests.md#cr-03-unsolicited-events-on-uart)). The HMI polls |
| `snapshot_status` | bits not defined by the ICD |
| Production authorization of the operational node | Resolved on the controller side 2026-10-30 (CR-04): authority is CONFIG data; the build switch is retired (see [commands-and-payloads §1](commands-and-payloads.md#1-conditions-for-hmi-control)) |
