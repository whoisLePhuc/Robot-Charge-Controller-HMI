# Robot Charge Controller HMI documentation

Touchscreen HMI (ESP32-3248S035R, PlatformIO + ESP-IDF + LVGL) that monitors and
controls the Robot Charge Controller over its operational UART.

**Status (2026-10-06):** the first integrated HMI image is built and flashed on the
ESP32-3248S035R through COM4. The display shows the five pages and touch navigation
works. `firmware/src/` now contains a bounded UART0 codec, polling/transaction task,
and LVGL pages with START confirmation and a persistent STOP action. The controller
has **not** been connected to P1, so response parsing, authorization, START/STOP and
the CH340C contention assumption are **not hardware verified**. The on-device codec
encoder self-test passed against the controller repository's independent wire vector;
the observed header was `LINK LOST` rather than `UART/PROTOCOL INIT FAILED`.

The flash chip on this board was read through COM4 as **4 MB**; `sdkconfig.defaults`
now pins 4 MB and the flashed image uses that size. Decisions HMI-D01 to HMI-D09
remain the design baseline. Touch measurements:
[hmi-hardware.md §6](hmi-hardware.md#6-touch-measured-behaviour).

**Next physical test:** disconnect USB, power the HMI through P1 from the controller,
wire TX/RX/GND as in [hmi-hardware.md §4](hmi-hardware.md#4-link-to-the-controller),
and run the HMI-D03 PING/GET_STATUS acceptance test. Verify `CONNECTED`, device
identity, status and measurement validity before exercising START/STOP. Record the
controller build flags and CONFIG selection; do not infer authority from the HMI
booting successfully.

## Reading order

A Python controller simulator is now available for exercising the real HMI through
COM4 without a connected RCC. Its host suite passed 33 tests, and an 8-second COM4
monitoring smoke test answered 46 HMI requests with no invalid frames. Touch-driven
START/STOP scenarios still require observation on the display; see the simulator
guide below. This does not verify the real controller or its relay path.

| Document | Content |
|---|---|
| [decisions.md](decisions.md) | Owner decisions: role, control authority, link UART, orientation, toolchain, LVGL, time source, documentation language, on-screen language |
| [controller-interface.md](controller-interface.md) | Contract with the controller: authority conditions, framing, byte layouts, code tables, START/STOP semantics, retries |
| [controller-simulator.md](controller-simulator.md) | Python UART controller simulator over COM4: interactive scenarios, START/STOP, fault injection, host tests |
| [hmi-hardware.md](hmi-hardware.md) | ESP32-3248S035R from schematic V1.1; wiring to the controller on P1 (UART0), fallback P3 |
| [ui-pages-and-information.md](ui-pages-and-information.md) | Every screen, state → label → control table, messages |
| [lvgl-implementation-plan.md](lvgl-implementation-plan.md) | Software architecture, transaction model, tests, implementation sequence |
| [controller-change-requests.md](controller-change-requests.md) | What the HMI needs the controller to add or fix (CR-01..05) |
| [mockups/](mockups/) | HTML mockups of the Overview screen |

## Sources of truth

1. The controller documents (ICD, `CAN.md`, `RS485.md`, `UART.md`) and controller
   firmware are normative for the protocol. `controller-interface.md` is a cited extract;
   if they differ, the controller is right.
2. The vendor schematic V1.1 is the source for hardware pins; the real board must be
   checked.
3. Every consequential statement carries a label: `confirmed`, `inferred`, `not-frozen`
   or `needs_verification`.

## Mockups

- `hmi-overview-landscape.html` is the reference (landscape, per HMI-D04).
  `hmi-overview-mockup.html` (portrait, Vietnamese labels) is **superseded** and kept only
  for history; do not implement from it.
- Mockups illustrate style; where they differ from `ui-pages-and-information.md`, the
  specification is right.
- Corrected 2026-10-06: contact feedback is always "none" (was "CONFIRMED"); the
  non-existent `ACTIVE`/`FAULT` states removed; overcurrent is `0x0402`; example current
  inside the 10 A envelope; idle VOUT not shown as 0.
- The stage strip groups `VREQ_VALIDATE` + `PRECHECK` as "PRE" and omits
  `COMPLETE`/`WAIT_REARM`; the full state table is in the specification §3.2.

## The `hardware/` folder

`hardware/ESP32-3248S035-3-5INCH-LCD-main/` holds the vendor material (schematics,
specification, datasheets, demos), copied from upstream `wegi1/ESP32-3248S035-3-5INCH-LCD`.
It is the evidence behind [hmi-hardware.md](hmi-hardware.md); keep at least `5-Schematic/`
and `2-Specification/`.
