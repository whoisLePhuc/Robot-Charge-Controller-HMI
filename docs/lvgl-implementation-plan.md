# HMI LVGL implementation plan

> Design and implementation checklist, not firmware. Goal: a dedicated HMI application
> for the `ESP32-3248S035R` board, built with PlatformIO (framework `espidf`) and LVGL,
> talking to the Robot Charge Controller over its operational UART. Protocol data:
> [controller-interface.md](controller-interface.md). Hardware:
> [hmi-hardware.md](hmi-hardware.md). Decisions: [decisions.md](decisions.md).

## 1. Goal and scope

The HMI provides a touchscreen to:

- Monitor charging state, voltage/current, faults and link state.
- Send `START_CHARGE` and `STOP_CHARGE`, since the HMI is the active control interface
  ([HMI-D01](decisions.md#hmi-d01-role-of-the-hmi)). It does not send `SET_TIME`
  ([HMI-D07](decisions.md#hmi-d07-time-source)).
- Show clearly whether a command was accepted, completed, rejected, failed or is
  unconfirmed.
- Put safety first: a lost link is never read as "relay safe" or "session stopped";
  stale data is marked as stale.

The HMI is an operating panel, not a service console: no CONFIG, CALIBRATION or
CLEAR_FAULT. It does not replace the hardware E-stop or the controller's interlocks.

## 2. Baseline decisions

| Item | Decision |
|---|---|
| Board | ESP32-3248S035R (ESP32-WROOM-32, 3.5" resistive touch), schematic V1.1 |
| Display | ST7796, SPI, **landscape 480 × 320** ([HMI-D04](decisions.md#hmi-d04-screen-orientation)) |
| Touch | XPT2046 on the LCD SPI bus |
| Toolchain | **PlatformIO**, `framework = espidf` ([HMI-D05](decisions.md#hmi-d05-toolchain)); on the development PC: platform `espressif32` 7.1.3, ESP-IDF 6.1.0 |
| UI library | **LVGL 9.6.0** (`lvgl/lvgl` `9.6.0~1`) with `esp_lvgl_port` 2.9.0, exact pins, to be confirmed by the first build ([HMI-D06](decisions.md#hmi-d06-lvgl-version)) |
| Controller link | **UART0 on header P1** ([HMI-D03](decisions.md#hmi-d03-link-uart)); no USB in operation; fallback UART1 on P3 (IO22/IO35) |
| Control authority | controller bench image with `RCC_BENCH_OPERATIONAL_NODE=1` ([HMI-D02](decisions.md#hmi-d02-control-authority)) and CONFIG on the operational UART, node 0 |
| Role on the link | client; frames to controller `0x01`; fixed `source` byte, never `0x01` or `0xFF` |

No PSRAM is assumed. Use partial draw buffers in DMA-capable internal SRAM.

## 3. Hardware and wiring

Details and schematic evidence: [hmi-hardware.md](hmi-hardware.md).

### 3.1 Display pins (`confirmed` by schematic V1.1)

| Signal | GPIO | Note |
|---|---:|---|
| SPI SCLK | 14 | LCD + touch |
| SPI MOSI | 13 | LCD + touch |
| SPI MISO | 12 | MTDI strapping pin; keep touch CS high through reset |
| LCD CS | 15 | MTDO strapping pin |
| LCD D/C | 2 | strapping pin |
| LCD RESET | — | tied to EN; no dedicated GPIO |
| Backlight | 27 | through MOSFET Q2; PWM possible |
| Touch CS | 33 | |
| Touch IRQ | 36 | input only, 10 kΩ pull-up |

Bring-up: LCD at 26–40 MHz, touch at 2–2.5 MHz; raise only after the image and touch
are stable.

### 3.2 Link to the controller

| HMI header P1 | Controller (operational UART, J2) |
|---|---|
| pin 1, VIN | +5V (HMI supply) |
| pin 2, U0TXD through R5 100 Ω | GPIO21 (controller RX) |
| pin 3, U0RXD through R6 100 Ω | GPIO19 (controller TX) |
| pin 4, GND | GND |

- 3.3 V logic on both sides.
- **Disable the ESP-IDF console on UART0** (console output set to none in sdkconfig).
  HMI diagnostics are shown on the Device page, not over UART0.
- ROM boot output at HMI reset still leaves on U0TXD; the controller discards it. The
  HMI parser must also discard garbage at start-up.
- No USB in operation. When flashing over USB: **disconnect the controller from P1**
  (CH340C drives U0RXD whenever USB is attached).
- The assumption that CH340C leaves U0RXD alone without USB is checked by the acceptance
  test of [HMI-D03](decisions.md#hmi-d03-link-uart). On failure, switch to UART1 on P3:
  only the UART number and pins in `hmi_board` change.

## 4. Software structure

PlatformIO project in `firmware/`:

```text
Robot-Charge-Controller-HMI/
├── docs/
├── firmware/
│   ├── platformio.ini            # platform pinned (espressif32@<version>), framework = espidf
│   ├── sdkconfig.defaults        # console off on UART0, LVGL/port options
│   ├── src/                      # app_main, hmi_board.c/.h (LCD, touch, LVGL port), idf_component.yml
│   ├── components/
│   │   ├── hmi_board/            # ST7796, XPT2046, backlight, board pins, link UART pins
│   │   ├── hmi_link/             # UART driver, framing, reassembly, transaction manager
│   │   ├── rcc_protocol/         # COBS, CRC-16, CRC-32, object codec, response decoders
│   │   ├── hmi_model/            # UI state independent of LVGL
│   │   └── hmi_ui/               # screens, widgets, theme, navigation
│   └── test/                     # host tests (PlatformIO native env or plain CMake)
└── hardware/ESP32-3248S035-3-5INCH-LCD-main/   # vendor material (keep provenance)
```

The HMI's `rcc_protocol` is its own implementation, checked against the controller's
golden vectors (`CAN.md` §5.3, `test_rcc_serial_frame.c`); it never copies the
controller's internal structs. `rcc_protocol` and `hmi_model` must not depend on
ESP-IDF or LVGL so that they build on the host.

### 4.1 Dependencies

- `esp_lcd` plus an ST7796 panel driver that supports ESP-IDF 6.1.
- `esp_lvgl_port` (tick, task, lock, flush).
- An XPT2046 driver compatible with ESP-IDF 6.1; check its maintenance status.
- LVGL `9.6.0~1` and `esp_lvgl_port` `2.9.0` (D06), pinned exactly in `idf_component.yml`.
- Pin the PlatformIO platform in `platformio.ini`; no floating versions in a release.

First step: a minimal app (SPI, ST7796, touch, one LVGL screen) built with PlatformIO.
Record the platform, ESP-IDF, LVGL and `esp_lvgl_port` versions that work in
[decisions.md](decisions.md#hmi-d06-lvgl-version). Never upgrade LVGL, ESP-IDF and the
LCD driver in the same change.

### 4.2 Display and memory

- One SPI host, two devices (LCD, touch); touch at a lower clock.
- Partial buffers of about 20–40 lines of 480 px RGB565 (≈ 19–38 KB each), DMA-capable.
- Check byte order and colour with a test chart; apply rotation in **one** layer only,
  and calibrate touch in the same landscape orientation.
- Splash as soon as the LCD is ready; never wait for the controller before showing UI.
- **Touch bus clock is 100 kHz**, not the usual 1–2.5 MHz: the XPT2046 on this board reads
  zeros above it ([hmi-hardware.md §6](hmi-hardware.md#6-touch-measured-behaviour)).
  Budget about 1.5 ms per touch read; run the touch read in the LVGL task, not in the
  protocol path.
- Backlight dimming when idle is optional; the first touch only wakes the backlight and
  must not activate the control underneath.

## 5. Tasks and data flow

No LVGL calls from an ISR or the protocol task. Only the context locked by
`esp_lvgl_port` changes widgets; background tasks post model events through bounded
queues.

| Block | Responsibility |
|---|---|
| UI task | touch, navigation, rendering; reads a consistent model snapshot |
| UART RX/TX | collect bytes, split frames at delimiters, send frames; no parsing in an ISR |
| Protocol worker | COBS → CRC-16 → header → reassembly → CRC-32 → object → response; hands results to the transaction manager |
| Transaction manager | §6.4: sequential queries and long-lived control transactions |
| Model | status/measurement/fault snapshots, freshness, command state; no LVGL dependency |
| Poll scheduler | schedules queries; slows down when the link is lost or a page is in the background |

Queue overflows are counted and shown in diagnostics. Never hold the LVGL lock while
waiting on the UART.

## 6. Controller protocol

### 6.1 Framing and checks

Full format: [controller-interface.md §3](controller-interface.md#3-framing). Summary:
`0x00 || COBS(header12 || data1..128 || crc16) || 0x00`; object = 12-byte header +
payload + CRC-32; objects up to 512 bytes, multi-frame above 128 bytes (a GET_EVENT_LOG
page needs 3 frames). Check version, reserved fields, lengths, fragments, CRC-16 and
**CRC-32 before any interpretation**.

### 6.2 Control authority

The HMI cannot grant itself authority. Both are required
([controller-interface.md §1](controller-interface.md#1-conditions-for-hmi-control)):

1. Controller CONFIG `operational_interface = OPERATIONAL_UART`, node 0 (set over the
   service UART).
2. A controller build with `operational_node_authorized`: per
   [HMI-D02](decisions.md#hmi-d02-control-authority), the bench image
   `-DRCC_BENCH_OPERATIONAL_NODE=1` during development.

RS485/CAN on the robot then lose control. The HMI cannot see condition 1 in advance
(CR-01); handle `UNAUTHORIZED_SOURCE` per
[ui-pages §3.4](ui-pages-and-information.md#34-when-the-hmi-has-no-control-authority).

### 6.3 Commands and responses

Commands, byte layouts, code tables:
[controller-interface.md §4–§8](controller-interface.md#4-commands-the-hmi-uses).
Decode every field by offset, little-endian; never cast to a packed C struct.

### 6.4 Transaction model, timeouts and retries

Two kinds of transaction, managed separately:

**a) Queries** (PING, GET_*): sequential, one outstanding at a time.

- 50 ms timeout per attempt; at most 2 retransmissions with **the same `request_id` and
  the same content** (`UART.md` §7). When attempts run out, count a timeout and move to
  the next query.
- A late reply to an abandoned query is dropped by `request_id`.

**b) Control commands** (START, STOP): long-lived, running **in parallel** with the
query loop.

- A new non-zero `request_id` for every operator action.
- Transport retry only when nothing at all came back, with the same `request_id` and
  content, within about 150 ms. **Never retry later**: the controller ledger keeps
  results for 2 s (`provisional`), and a resend after that is a **new** command.
- After `ACCEPTED`, keep the transaction open and accept every frame carrying that
  `request_id`, even while polling, until the terminal result. A START result may arrive
  seconds later (once charge current is established); a STOP result arrives after the
  controller has written flash. These results must **not** be dropped as "late".
- The controller sets no deadline for START; the HMI shows the elapsed wait and the state
  from GET_STATUS and does not declare failure on its own.
- `boot_id` changes (controller reset) → cancel every open transaction and show
  `Controller restarted — command not confirmed`.
- At most one START pending (the controller enforces this too). STOP may be sent at any
  time, including while a START is pending.
- An operator pressing STOP again after a timeout is a new action with a new
  `request_id`. STOP is idempotent in the controller, so this is safe.

Counters: timeouts, CRC-16 errors, CRC-32 errors, malformed frames, overflows, results
matching no transaction.

### 6.5 UART load

The operational UART runs at 115 200 bit/s, about 11.5 kB/s. One round of GET_STATUS,
GET_MEASUREMENTS and GET_FAULTS is about 330 bytes in both directions. Polling status at
2 Hz, measurements at 5 Hz and faults at 1 Hz uses under 10 % of the bandwidth
(`inferred`). The 2026-10-06 bench stress run answered about 16 queries/s on the
operational UART with a p50 latency of about 60 ms including the PC.

## 7. State model and operating safety

### 7.1 Link and data quality

Keep apart: online/offline, age of the last valid reply, measurement validity, charging
state. A frame with a good CRC does not make all data fresh: status and measurements have
their own sequence, age and flags. Past the threshold, dim and mark `STALE`; with the
link lost, never present old numbers as current.

After boot or reconnect: GET_DEVICE_INFO → GET_STATUS → GET_MEASUREMENTS → GET_FAULTS;
check compatibility before enabling START. Never send START/STOP because the display
restarted.

### 7.2 START

1. START opens a confirmation: link online, data fresh, state allows it (table in
   [ui-pages §3.2](ui-pages-and-information.md#32-state--label--controls)).
2. Do not send if the link is lost or stale, another START is pending, or the state
   does not allow it. The controller still decides and may reject.
3. Send once (transport retry per §6.4).
4. `ACCEPTED` → `Request accepted — checking`. Terminal result:
   - `COMPLETED` → show `Charging` only when GET_STATUS also reports `CHARGING` with
     `charging_established = 1`;
   - `FAILED` + reason (`CHARGE_NOT_ESTABLISHED`, `ABORTED_BY_STOP`, …);
   - `REJECTED` + reason (`CHARGER_REQUEST_ABSENT`, `INVALID_STATE`,
     `UNAUTHORIZED_SOURCE`, …).
5. Whatever happens after `COMPLETED` (completion, maximum duration, fault, loss of the
   charge source) is visible only through state and events; there is no second result.

### 7.3 STOP

STOP is prominent and needs no second confirmation. Send at once; show `Stopping` until
the terminal result. Meaning of each result:
[ui-pages §3.3](ui-pages-and-information.md#33-startstop). Easy to get wrong:

- `REJECTED/QUEUE_FULL` for STOP: the STOP **took effect**, it just cannot be tracked;
  send again.
- `FAILED/PERSISTENCE_FAILED`: the relay was commanded open; only the flash write failed.
- A timeout proves neither that the relay opened nor that the STOP failed.
- UART has no broadcast STOP; the hardware E-stop remains the independent emergency means.

### 7.4 No inferred relay feedback

Relay command and contact feedback are different things. The current controller board has
no contact feedback: always show `Contact feedback: none`.

## 8. User interface

Full specification: [ui-pages-and-information.md](ui-pages-and-information.md).
Reference mockup: `mockups/hmi-overview-landscape.html` (see the note in the
[README](README.md)).

## 9. Testing and acceptance

### 9.1 Hardware bring-up

- Stable boot; LCD in the right landscape orientation and colours; no unexpected resets.
- Backlight; touch on the correct axes over the whole screen after calibration.
- SPI stable while drawing and reading touch together.
- **HMI-D03 acceptance:** supply through P1, no USB, PING + GET_STATUS for 10 minutes,
  every reply received, no CRC error.
- No ESP-IDF log byte on UART0 in operation: the controller's `OperationalUart rejected`
  counter does not rise while the HMI runs steadily.
- Flashing procedure: unplug P1, plug USB, flash, unplug USB, plug P1 back.

### 9.2 Host protocol tests

- Golden vectors for COBS, CRC-16 (`check = 0x29B1`), CRC-32, endianness and response
  layouts.
- Truncated frames, garbage bytes, consecutive delimiters, wrong lengths, bad CRCs, bad
  version/reserved fields, unexpected fragments, multi-frame objects, stale or mismatched
  response IDs.
- No panic, no buffer overrun, no deadlock; fuzz the codec as the controller's
  `test_rcc_fuzz` does.
- Simulated controller: online/offline, delay, duplicates, START `ACCEPTED` then
  `COMPLETED` 5 s later while polling continues, STOP `QUEUE_FULL`, STOP timeout,
  `UNAUTHORIZED_SOURCE`, `boot_id` changing mid-transaction.

### 9.3 UI / safety acceptance

- START cannot be pressed with a stale or lost link or while another START is pending.
- `ACCEPTED` never turns into `Charging` without confirmation.
- A late START terminal result is still shown correctly.
- STOP timeout is always "not confirmed"; STOP `QUEUE_FULL` is never shown as "nothing done".
- Link lost while charging: old readings are marked and a "charging may continue" warning
  is shown.
- No authority: no STOP button pretending to work.
- Contact feedback is always shown as "none".
- STOP reachable on every page; the UI does not freeze on timeouts or a large log.
- Restarting the HMI mid-session: queries only, no command sent by itself.

## 10. Implementation sequence

1. **Decisions:** D01–D09 settled (D06 version pinning happens in step 3).
2. **Hardware:** confirm the board is V1.1, wire P1 to the controller, run the HMI-D03
   acceptance test with a minimal PING firmware before any UI.
3. **Toolchain:** minimal PlatformIO app; pin platform, LVGL and `esp_lvgl_port`; record
   them in D06.
4. **Display:** panel, backlight, colour, landscape orientation, partial buffers, touch
   calibration.
5. **Host protocol work:** codec, golden vectors, simulated controller.
6. **UART link:** bounded parser, transaction manager (§6.4), counters.
7. **Read-only UI:** device info, status, measurements, faults, freshness; link-failure
   cases.
8. **Control:** START/STOP on a safe bench, with the controller bench image and CONFIG on
   the operational UART (§6.2).
9. **UX:** navigation, typography, warnings, event log.
10. **On-device validation:** checklist §9; record HMI and controller versions (image hash
    from GET_DEVICE_INFO) in a test record.

## 11. Build and flash

PlatformIO, from `firmware/`:

```powershell
pio run                        # build
pio run -t upload              # flash over USB (controller disconnected from P1)
pio device monitor             # USB console; only when the ESP-IDF console is enabled for debugging
```

Name Kconfig options only once the real project exists and they are verified.

## 12. To verify before any release

- The board matches schematic V1.1; the HMI-D03 acceptance test passes.
- HMI supply current fits the control board's +5V rail.
- Baud, timeouts, frame limits and schemas match the controller firmware actually running
  (compare `firmware_revision` and the image hash).
- The controller has CONFIG on the operational UART, node 0, and a build that trusts the
  operational node (§6.2); off the bench this needs CR-04.
- ST7796/XPT2046 drivers and `esp_lvgl_port` build with the pinned PlatformIO platform.
- Fault/inhibit bit positions are not frozen by the ICD (CR-02): recheck when the
  controller version changes.
- Final stale/timeout thresholds measured on real hardware.
- Product requirements for START confirmation, emergency stop and brightness.

## 13. References

- Controller: `robot_charge_controller`, `docs/firmware/interfaces/` (ICD, `CAN.md`,
  `RS485.md`, `UART.md`); checked at commit `f72ec7b`.
- Vendor hardware: `hardware/ESP32-3248S035-3-5INCH-LCD-main/` (schematics in `5-Schematic/`).
- When the controller changes a schema or the protocol: update `controller-interface.md`
  and the golden vectors together.
