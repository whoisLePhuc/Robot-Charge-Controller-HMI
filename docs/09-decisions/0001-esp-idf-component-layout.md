# ADR-0001: Build with `idf.py` and split the firmware into layered ESP-IDF components

- **Status:** Implemented (phases 1 and 2 complete; not yet run on a board)
- **Date:** 2026-11-27
- **Source:** summarised from [the archived design record](archive/firmware-restructure-design.md) (translated from Vietnamese). Where this page and that record differ, this page reflects the final state.

## Context

- PlatformIO was only a wrapper: the project already used `framework = espidf`, a single `src/` component, `idf_component.yml` and `dependencies.lock`.
- The code had become tangled:
  - one flat component held drivers, protocol, services and UI;
  - display start-up also started `rcc_link` and the UI;
  - the controller data contract (`rcc_snapshot_t`) lived in the UART transport header;
  - `hmi_wifi.c` called the OTA start/stop functions;
  - the backlight file was both an LEDC driver and a dim/sleep policy that read settings.
- Contributors lost time finding where code lives, adding controller data, touching Settings/Wi-Fi/OTA and hardware/start-up.

## Decision

1. Drop PlatformIO; build with `idf.py` (ESP-IDF 6.1.0) in `firmware/`.
2. Migrate in two phases: (1) move as-is and prove identical behaviour; (2) restructure step by step.
3. **Layers as directories** `common / bsp / protocol / services / ui`, each module an ESP-IDF component with a public `include/` and private `src/`.
4. **Plain C functions** with a module prefix (for example `rcc_link_snapshot()`); no function-pointer structs.
5. **Services do not call each other.** Runtime dependencies go through `esp_event`; wiring lives in `main/app.c`.

Rules and recipes that implement this are in [`firmware/components/README.md`](../../firmware/components/README.md).

## Why

- With `PRIV_REQUIRES`, an include that crosses a layer the wrong way **fails the build** instead of relying on convention.
- Direct functions keep go-to-definition working. The simulator and host tests replace modules by link-time stubs (`tools/sim/sim_stubs.c`), so no extra indirection is needed.
- Each module has a single real implementation, so function-pointer structs would add a layer for nothing. Modules are singletons, and the LVGL heap is tight, so `create()/destroy()` on the heap was rejected.

## Alternatives considered

| Option | Why not |
|---|---|
| One component per layer | Does not stop services calling each other within a layer, which is the tangle being removed |
| Slice by feature | Spreads LVGL across every slice and breaks the UI layers that are tested (`hmi_present`, retained mode) |
| Function-pointer structs (as in a sibling project) | Indirection with one implementation |
| `create()/destroy()` on the heap | Singletons; LVGL heap budget is monitored closely |

## What changed from the first draft

| First draft | Final |
|---|---|
| OTA listens to raw `IP_EVENT_STA_GOT_IP` / `WIFI_EVENT_STA_DISCONNECTED` | `hmi_wifi` posts `HMI_WIFI_EVENT_CONNECTED` / `_LINK_LOST` at the two former call sites; `app.c` starts/stops the OTA server. This preserves the old behaviour (stop only when an *established* connection drops) |
| UART pins in `bsp_pins.h`, configured from `app.c` | `RCC_LINK_*` constants stay in `hmi_config.h` (`hmi_common`) |
| `bsp_display_start()` also starts touch | `bsp_touch_start(display, cb)`, called after `hmi_ui_start()` to keep the old order |
| (not planned) | Touch calls a callback (`bsp_touch_activity_cb_t`) instead of calling the backlight policy; `app.c` passes `hmi_power_on_touch` |
| bsp depends on `hmi_common` | bsp depends on no project component |
| Fonts declared in `hmi_theme.h` | In `ui_assets/include/hmi_fonts.h` |
| `hmi_backlight_is_dimmed()` public | removed; `hmi_power` API is `start`, `on_touch`, `set_brightness`, `tick` |

## Consequences

- Positive: layer violations are build errors; one place (`app.c`) shows start order and wiring; modules testable on a PC.
- Cost: more `CMakeLists.txt` files; a new layer directory must be added to `EXTRA_COMPONENT_DIRS`; every new module needs a stand-in in the simulator stubs if the UI uses it.
- `sdkconfig` now comes from `sdkconfig.defaults`; it matched the former PlatformIO configuration except the partition file name (reported, phase 1).

## Risks recorded and their mitigation

| Risk | Mitigation |
|---|---|
| Generated `sdkconfig` differs from the PlatformIO one | Diffed in phase 1; accepted only with a written explanation |
| Managed component versions drift | `dependencies.lock` copied; IDF pinned to 6.1.0 |
| Start order changes when moved to `app.c` | Order kept and logged step by step |
| OTA handler registered after Wi-Fi has an IP would miss the event | `app.c` registers it before `hmi_wifi_init()` |
| A refactor changes the UI unnoticed | Pixel diff and morph diff must stay clean after every step |
| Tools outside `firmware/` point at old paths | Updated: `tools/snapshot/*`, `gen_icons.py`, `tools/sim/Makefile`, `test/host/run.sh` |

## Out of scope (at the time)

UI changes, new pages, moving the Device page to retained mode, protocol or `rcc_snapshot_t` changes, heap-allocated module instances, CI.

## Follow-ups

- The UI source later moved to an LVGL Pro project (commit `f96b6bb`). That decision has no ADR yet: [ADR-0002](README.md) is planned.
- Flashing the restructured image and comparing the boot log remains open ([verification-status](../08-status/verification-status.md)).
