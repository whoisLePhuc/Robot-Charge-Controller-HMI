# HMI design decisions

These decisions belong to the project owner. This page records the options, the
consequences and the decision once it is made. A decision is settled only when its
status says so, with a date. When one is settled, update every document it affects.

| ID | Question | Status |
|---|---|---|
| HMI-D01 | Does the HMI control the charger, or only monitor it? | **Settled 2026-10-06: control and monitor (A)** |
| HMI-D02 | How does the HMI get control authority? | **Settled 2026-10-06: controller bench image (A)** |
| HMI-D03 | Which UART carries the link? | **Settled 2026-10-06: UART0 on P1**, with an acceptance test |
| HMI-D04 | Screen orientation | **Settled 2026-10-06: landscape 480 × 320** |
| HMI-D05 | Build toolchain | **Settled 2026-10-06: PlatformIO** (framework `espidf`) |
| HMI-D06 | LVGL version | **Settled 2026-10-06: LVGL 9.6.0** (exact pin `9.6.0~1`), with `esp_lvgl_port` 2.9.0; confirmed by the first build |
| HMI-D07 | Time source for `SET_TIME` | **Settled 2026-10-06: the HMI does not send `SET_TIME`** |
| HMI-D08 | Documentation language | **Settled 2026-10-06: English only** |
| HMI-D09 | On-screen language | **Settled 2026-10-06: English** |

## HMI-D01: Role of the HMI

The controller has exactly **one** active control interface at a time, selected by
CONFIG (ICD §10). Every other interface may only monitor.

| Option | Meaning | Consequence |
|---|---|---|
| A. HMI controls | CONFIG `operational_interface = OPERATIONAL_UART`, node 0 | The HMI can send START/STOP. RS485 and CAN on the robot side **lose** START/STOP, including the RS485/CAN emergency broadcast STOP |
| B. HMI only monitors | CONFIG stays on RS485 or CAN | The HMI shows state, measurements, faults and the log; no START/STOP |
| C. Switch by mode | change CONFIG between A and B | Each switch is a CONFIG transaction over the service UART; impossible from the HMI |

**Decision (2026-10-06, project owner): A.**

| # | Consequence | Owner |
|---|---|---|
| 1 | Controller CONFIG must be set to `operational_interface = OPERATIONAL_UART` (3), `operational_node = 0`, through a CONFIG transaction on the service UART. `ENTER_CONFIG_MODE` is accepted only once the controller proves no charger is present; the current bench board (INA240 removed, `INHIBITED`) refuses it | Controller / bench |
| 2 | RS485 and CAN on the robot side lose START and STOP, including the broadcast emergency STOP. They keep monitoring | Robot integration documentation |
| 3 | Automatic charging still works: the controller starts a session by itself on a valid VOUT charge request, with no START (ICD §11.2; `inferred` for this configuration, not yet run on the bench). START from the HMI is needed only for a manual start, for example after a STOP has set `REMOTE_INHIBIT` | — |
| 4 | Stopping a session is possible only from the HMI or the hardware E-stop. The HMI must be available while charging; if its link drops, the session continues (ICD §19) | HMI, operation |
| 5 | Authority also depends on HMI-D02 | — |

The UI keeps a defined behaviour for `UNAUTHORIZED_SOURCE`
([ui-pages-and-information.md §3.4](ui-pages-and-information.md#34-when-the-hmi-has-no-control-authority)),
which occurs whenever CONFIG or the controller image is not set up for the HMI.

## HMI-D02: Control authority

The current controller firmware trusts the operational node only when built with
`-DRCC_BENCH_OPERATIONAL_NODE=1`, i.e. **on a bench image only**. There is no
production mechanism (`FDD10-OPEN-005`, `ICD-OPEN-006`).

| Option | Consequence |
|---|---|
| A. Use a bench image until the controller has a production mechanism | Enough for development and demos; not a product |
| B. Ask the controller for a controlled authorization mechanism | [controller-change-requests.md](controller-change-requests.md) CR-04; decided in the controller repository |

**Decision (2026-10-06, project owner): A.**

Controller image used with the HMI:

| Controller build knob | Value | Why |
|---|---|---|
| `RCC_BENCH_OPERATIONAL_NODE` | `1` | the controller trusts the operational node, so the HMI may START/STOP |
| `RCC_BENCH_RELAY_ENABLE` | `0` while developing the UI; `1` only to test a real session on the low-energy bench | keeps the relay gated when it need not close; matches the controller's negative/positive control procedure |
| `RCC_BRINGUP_PROVISION`, `RCC_FIRMWARE_REVISION`, `RCC_BENCH_FAULT_INJECT` | `0` | as for any ordinary bench image |

Consequences:

- This is **not** a product configuration. Every HMI test result records the
  controller image hash and build flags from GET_DEVICE_INFO.
- The HMI shows a permanent "bench image" notice on the Device page when the
  `provisional` or `operational_node` build flag is set.
- Before the HMI is used off the bench, the controller needs a controlled
  authorization mechanism ([CR-04](controller-change-requests.md#cr-04-production-authorization-of-the-operational-node)).
  That is undecided and belongs to the controller repository.

## HMI-D03: Link UART

Evidence: [hmi-hardware.md](hmi-hardware.md) §3–§4.

| Option | Consequence |
|---|---|
| A. UART1 on header P3: TX = IO22, RX = IO35 | does not touch CH340C/UART0; USB flashing and logging stay available; IO35 needs an external pull-up |
| B. UART2 on IO16/IO17 (RGB LED pins) | soldered wires; the LED blinks with the data |
| C. UART0 on header P1 | no hardware change if CH340C does not drive its TXD without USB; ROM boot output reaches the link; no USB flashing or logging while the controller is connected |

**Decision (2026-10-06, project owner): C — UART0 on P1, USB not connected in
operation.** Owner's rationale: in operation the HMI takes 5 V from the control board
through P1 and no USB cable is connected, so CH340C does not conflict.

Assumption to verify (`needs_verification`): per schematic V1.1, CH340C is powered
from the board's 3.3 V rail, so it is powered without USB too. The decision holds if,
without USB, CH340C **does not drive** its TXD pin (wired straight to U0RXD). If it
does, the controller's TX through R6 (100 Ω) cannot pull U0RXD low and the HMI
receives nothing.

Acceptance test (at bring-up, before any control UI is written):

1. Power the HMI from the control board through P1, **no USB**, TX/RX/GND wired to the
   controller's operational UART.
2. The HMI sends PING and GET_STATUS continuously for 10 minutes: every reply arrives,
   no CRC error.
3. Optional direct measurement: disconnect the controller's TX, connect 1 kΩ from P1
   pin 3 to GND. About 0 V: CH340C does not drive, pass. About 3.0 V: it drives, fail.

If it fails: switch to A (UART1 on P3, IO22/IO35). No hardware change; firmware changes
only the UART number and pins in `hmi_board`.

Operating conditions:

| Condition | Reason |
|---|---|
| ESP-IDF console/log disabled on UART0 in the HMI firmware | otherwise log text goes straight into the controller's operational UART |
| ROM boot output at HMI reset is accepted | the controller discards it as garbage (checked by the noise stress run, 2026-10-06) |
| **Disconnect the controller from P1 whenever the HMI is flashed over USB** | with USB attached CH340C does drive U0RXD and fights the controller's TX; the controller also receives the whole flashing stream as garbage |
| No USB in operation | the decision's assumption |
| HMI supply current checked against the control board's +5V rail | the LCD backlight draws current; not yet compared (`needs_verification`) |

## HMI-D04: Screen orientation

**Decision (2026-10-06, project owner): landscape, 480 × 320** (ST7796 rotated 90°).

Consequences: the UI specification follows the landscape grid of
`mockups/hmi-overview-landscape.html` (left navigation rail, STOP/START button across
the bottom). The portrait mockup is superseded. Rotation is applied in one layer only
(panel driver or LVGL), and the resistive touch calibration is done in the same
orientation.

## HMI-D05: Toolchain

| Option | Consequence |
|---|---|
| A. ESP-IDF directly (`idf.py`) | same tooling as the controller |
| B. PlatformIO with `framework = espidf` | IDE integration; the ESP-IDF version is whatever the PlatformIO platform package ships |

**Decision (2026-10-06, project owner): B — PlatformIO.**

Facts on the development PC (2026-10-06): PlatformIO platform `espressif32` 7.1.3
with `framework-espidf` 4.60100.0, which is **ESP-IDF 6.1.0** (`sdkconfig`:
`CONFIG_IDF_INIT_VERSION="6.1.0"`), the same ESP-IDF line as the controller. The
`firmware/` project exists with an empty `app_main`; it has not yet produced a binary.

Consequences:

- Pin the platform version in `platformio.ini` (`platform = espressif32@7.1.3` or
  the version proven by the first build); do not float it.
- ESP-IDF components (`esp_lcd`, `esp_lvgl_port`, LVGL, touch driver) come through
  the ESP-IDF component manager (`idf_component.yml`), which PlatformIO's espidf
  framework supports. Prove it with the first build.
- Host tests for the protocol codec run outside the target build (PlatformIO `native`
  environment or a plain CMake host project); decide when the codec is written.

## HMI-D06: LVGL version

**Decision (2026-10-06, project owner): LVGL 9.6.0, pinned exactly.** The owner first
proposed 9.6.1; that release did not exist on 2026-10-06 (the Espressif registry and
GitHub `lvgl/lvgl` both end at 9.6.0, released 2026-09-16), so 9.6.0 is used.

Pins for `idf_component.yml` (from the Espressif registry, 2026-10-06; not yet built):

| Component | Pin | Note |
|---|---|---|
| `lvgl/lvgl` | `9.6.0~1` | exact, no range. `~1` is the registry's repackaging of 9.6.0 |
| `espressif/esp_lvgl_port` | `2.9.0` | requires ESP-IDF ≥ 5.2 and LVGL ≥ 8, < 10 |

Do not use a floating range such as `~9.6.0`. Moving to 9.6.1 or later is a deliberate,
separate change with a rebuild, never combined with an ESP-IDF or driver upgrade.

The original selection rule is kept for the record:

How the version is fixed:

1. Minimal app: SPI, ST7796, XPT2046, `esp_lvgl_port`, one LVGL screen, built with
   PlatformIO.
2. Start from the newest LVGL release that the newest `esp_lvgl_port` supports; step
   back only if the build or the display fails.
3. Pin the exact LVGL and `esp_lvgl_port` versions in `idf_component.yml` and record
   them here, with the date of the build test.

Status: the pins above are **inferred from registry metadata**. They become confirmed
only when the first PlatformIO build with them succeeds (that build also shows whether
the four extra LVGL 9.6 dependencies — freetype, libjpeg-turbo, libpng, lz4 — are pulled
in and what they cost in flash). If it fails, step back one LVGL release at a time and
record the result here.

## HMI-D07: Time source

**Decision (2026-10-06, project owner): the HMI does not send `SET_TIME`.**

Consequences:

- The HMI has no time source to manage (the board has no RTC on the schematic).
- Because the HMI is the active control interface (D01), and only that interface may
  send `SET_TIME`, the controller's UTC annotation will stay **unsynchronized**. Event
  log entries show relative time from `monotonic_us`, not wall-clock time.
- No safety decision depends on UTC; this is a display limitation only.

## HMI-D08: Documentation language

**Decision (2026-10-06, project owner): HMI documentation is English only.** All
documents in `docs/` were rewritten in English on 2026-10-06.

## HMI-D09: On-screen language

D08 covers the documents, not the text on the display.

| Option | Consequence |
|---|---|
| English | matches the documents and the landscape mockup; one font set |
| Vietnamese | operators read their own language; needs a font with Vietnamese diacritics (the LVGL built-in fonts do not cover them), more flash |
| Both, selectable | two string tables; more testing |

**Decision (2026-10-06, project owner): English.**

Consequences:

- All on-screen text uses the English labels of
  [ui-pages-and-information.md](ui-pages-and-information.md); one string table, no
  language switch.
- LVGL built-in Latin fonts are sufficient; no extra font for diacritics.
- Keep all strings in one table (not scattered in widget code) so a later translation
  stays a data change.
