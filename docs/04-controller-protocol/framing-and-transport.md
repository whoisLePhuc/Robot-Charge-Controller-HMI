# Framing and transport

How bytes travel between the HMI and the controller: the physical link, the wire framing, the canonical object, and timing, retry and duplicate handling. Command meanings are in [commands-and-payloads.md](commands-and-payloads.md); code values in [code-tables.md](code-tables.md).

This page is a **cited extract** of the controller documentation, not its source.

| Item | Value |
|---|---|
| Extracted from | `docs/controller-interface.md`, checked against controller repository `robot_charge_controller` commit `f72ec7b` (firmware `bae44f7`), 2026-10-06 |
| Spot-checked 2026-11-27 | HMI constants in `rcc_link.c` and `hmi_config.h`; controller ICD in the sibling checkout at `bc4f308` (branch `codex/rcc-studio-m1`) |
| Normative sources | Controller `docs/firmware/interfaces/`: `firmware-interface-control-document.md` (ICD), `CAN.md` §5, §8, §9, `RS485.md` §4–§5, `UART.md` |
| Rule | Where this page and the ICD or controller firmware differ, **the ICD/firmware is right** and this page is stale. Update this page and the golden vectors together |
| Confidence labels | `confirmed` (in ICD or firmware source), `inferred` (derived, not measured), `not-frozen` (in source but not frozen by the ICD), `provisional` (value in the firmware profile, may change) |

## 1. Physical layer (controller operational UART)

| Item | Value | Source |
|---|---|---|
| Controller port | UART2, TX = GPIO19, RX = GPIO21 (connector J2) | firmware `rcc_board_binding.h`, `confirmed` |
| Logic level | 3.3 V, not RS-232 | ICD §18.1 |
| Character format | 115 200 bit/s, 8N1, no flow control | `UART.md` §3.1 |
| Topology | point-to-point, full duplex | `UART.md` §3.1 |

HMI side: `rcc_link` uses UART1, TX GPIO43, RX GPIO44, 115200 8N1, no flow control (constants in `firmware/components/common/hmi_common/include/hmi_config.h`). Wiring and the open connector question: [controller-wiring.md](../03-hardware/controller-wiring.md).

## 2. Wire framing

```text
wire      = 0x00 || COBS(raw_frame) || 0x00
raw_frame = serial_header(12) || fragment_data(1..128) || crc16(2, LE)
```

### 2.1 Serial header (`RS485.md` §4.2, used unchanged on UART)

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

- CRC-16/CCITT-FALSE (poly `0x1021`, init `0xFFFF`, no reflection, xorout 0, `check("123456789") = 0x29B1`) over the COBS-decoded `raw_frame` except the CRC bytes.
- An object of ≤ 128 bytes travels in one SINGLE frame. Larger objects: OBJECT_FIRST then OBJECT_DATA; every fragment except the last carries a full 128 bytes (`RS485.md` §5).
- The longest reply (a GET_EVENT_LOG page, object ≤ 268 bytes) takes **3 frames**. The HMI must reassemble multi-frame objects from the start.
- Reassembly key `(source, destination, request_id)`; a missing, repeated or out-of-order fragment aborts the whole transfer; a partial transfer expires after 2 s.

### 2.2 Canonical object (`CAN.md` §5)

| Offset | Field | Type |
|---|---|---|
| 0 | `protocol_version` | u8, `0x01` |
| 1 | `message_type` | u8: `0x01` COMMAND, `0x02` COMMAND_RESULT, `0x03` EVENT |
| 2 | `flags` | u16 LE, **must be 0** in V1 |
| 4 | `command_code` | u16 LE ([commands](commands-and-payloads.md#2-commands-the-hmi-uses)); always `0x0000` for EVENT |
| 6 | `request_id` | u32 LE |
| 10 | `payload_length` | u16 LE = `canonical_object_length − 16` |
| 12 | `payload` | 0–496 bytes |
| end | `crc32` | u32 LE, CRC-32/ISO-HDLC over the whole object except itself |

Decode order (`CAN.md` §5.2): length → **CRC-32 before any interpretation** → version → `payload_length` → field rules.

Golden vectors: `CAN.md` §5.3 and the controller's `firmware/tests/host/test_rcc_serial_frame.c` (an independently produced wire vector). The HMI host tests (`firmware/test/host/test_rcc_codec.c`) should reuse exactly these vectors.

## 3. Timing, retry and duplicate handling

| Item | Value | Source |
|---|---|---|
| Reply timeout per attempt | 50 ms | `UART.md` §7 |
| Transport-level retransmissions | 2 (about 150 ms in total), **same `request_id`, same content** | `UART.md` §7 |
| Ledger result retention (`router_retention_us`) | 2 s, `provisional` | `ICD-OPEN-005`; firmware profile |
| Normal / STOP ledger depth | 8 / 4, `provisional` | firmware `rcc_build_profile.h` |
| Duplicate key | `(interface, node, request_id)` | ICD-DUP-001 |

### What the HMI does (`rcc_link.c`, checked 2026-11-27)

- A transaction starts with a **50 ms** deadline and is sent up to **3 times** in total (two identical retransmissions) before it becomes *unconfirmed*.
- Once `ACCEPTED` arrives, retries stop and the terminal-result deadline is **15 s**. That is an HMI wait limit, not a controller execution guarantee.
- Query starts are at least **170 ms** apart. The round-robin order and freshness rules are in [data-flow §4](../02-architecture/data-flow.md#4-polling-and-freshness).

### Duplicate rules

- Same key, same content → the cached result, no repeated action. Same key, different content → `REJECTED/DUPLICATE_ID_CONFLICT`.
- **A resend after the ledger has forgotten (> 2 s) is a new command.** Automatic retries must stay inside the ~150 ms window.
- Queries (PING, GET_*) bypass the ledger and are answered afresh; repeating them is safe.
- Bench observation 2026-10-06 (`inferred` for the HMI): queries over the operational UART had a p50 latency of about 60 ms including USB and the PC; 30 back-to-back queries with no gap were all answered.
