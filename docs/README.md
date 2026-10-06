# Robot Charge Controller HMI documentation

Touchscreen HMI (ESP32-3248S035R, PlatformIO + ESP-IDF + LVGL) that monitors and
controls the Robot Charge Controller over its operational UART.

**Status (2026-10-06):** design documents; no firmware yet (`firmware/src/main.c` is
empty). Decisions HMI-D01 to HMI-D09 are settled (the exact LVGL version is pinned by the
first build, per HMI-D06). See [decisions.md](decisions.md).

## Reading order

| Document | Content |
|---|---|
| [decisions.md](decisions.md) | Owner decisions: role, control authority, link UART, orientation, toolchain, LVGL, time source, documentation language, on-screen language |
| [controller-interface.md](controller-interface.md) | Contract with the controller: authority conditions, framing, byte layouts, code tables, START/STOP semantics, retries |
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
