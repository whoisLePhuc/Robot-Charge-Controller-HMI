# Firmware components

Every module is an ESP-IDF component under `components/<layer>/<module>/`, with a public `include/` and a
private `src/`. `main/app.c` is the composition root: the only code that sees every component and the only
place where modules are started and connected to each other. Design record and rationale:
`../../docs/firmware-restructure-design.md`.

```
main/            main.c -> app_run() in app.c: boot check, watchdog, services, board, link + UI, alive loop
components/
  common/        hmi_common       rcc_types.h (rcc_snapshot_t: controller data contract), hmi_config.h
  bsp/           bsp_elecrow_s3   ST77922 QSPI LCD + LVGL port, touch, LEDC backlight; pins in src/bsp_pins.h
  protocol/      rcc_codec        controller frame codec (pure C, host-tested)
                 rcc_link         controller UART task, snapshot, START/STOP requests
  services/      hmi_settings     NVS settings (leaf store)
                 hmi_wifi         Wi-Fi station; posts HMI_WIFI_EVENT_CONNECTED / _LINK_LOST
                 hmi_ota          PIN-armed web OTA server, rollback confirmation
                 hmi_power        brightness, dim/sleep, wake on touch (over the BSP backlight)
  ui/            ui_assets        fonts, icons (generated)
                 ui_present       hmi_present: text/tone/views from rcc_snapshot_t (pure C, host-tested)
                 ui_core          theme and widgets (LVGL)
                 ui_app           hmi_ui.h API; shell, nav, pages, model, Wi-Fi flow (private)  -> ui/README.md
```

## Dependency rules

Dependencies point down only: `ui -> services -> protocol -> bsp -> common`; a layer may skip the one below
it (`ui_app` uses `rcc_link`). Nothing depends upward. These are the `REQUIRES` / `PRIV_REQUIRES` in each
`CMakeLists.txt`, so an include that breaks a rule fails the build.

| Component | Project components it may use | Public (`REQUIRES`) |
|---|---|---|
| `hmi_common` | none | none |
| `bsp_elecrow_s3` | none | `lvgl` (`lv_display_t` in the API) |
| `rcc_codec` | none | none |
| `rcc_link` | `rcc_codec`, `hmi_common` | `hmi_common` (`rcc_types.h`) |
| `hmi_settings` | none | none |
| `hmi_wifi` | none | `esp_event` (event base in the API) |
| `hmi_ota` | none | none |
| `hmi_power` | `bsp_elecrow_s3`, `hmi_settings` | none |
| `ui_assets` | none | `lvgl` |
| `ui_present` | `hmi_common` | `hmi_common` |
| `ui_core` | `ui_assets`, `ui_present` | `lvgl`, `ui_assets`, `ui_present` |
| `ui_app` | `ui_core`, `ui_present`, `ui_assets`, `hmi_common`, `rcc_link`, `hmi_settings`, `hmi_wifi`, `hmi_ota`, `hmi_power` | none |
| `main` | all of the above | n/a |

1. **Services do not call each other.** The one exception is `hmi_power -> hmi_settings` (the leaf store
   it reads and writes). When one service must react to another, the first posts an `esp_event` and
   `app.c` connects it: `HMI_WIFI_EVENT_CONNECTED` / `_LINK_LOST` start / stop the OTA server.
2. **The BSP calls nothing above it.** Where a driver needs policy, it takes a callback that `app.c` passes
   in: `bsp_touch_start(display, hmi_power_on_touch)` (a touch that only wakes the screen is swallowed).
3. **The UI does not use the BSP.** Board constants the Device page shows (controller UART, watchdog
   timeout) are in `hmi_config.h`.
4. **`rcc_snapshot_t` is the only controller data the UI sees.** Pages never call `rcc_link_*`; the shell
   takes one snapshot per refresh. UI rules: `ui/README.md`.
5. **No runtime indirection for a single implementation.** Modules expose plain C functions with their
   prefix. The simulator and host tests replace modules at link time (`tools/sim/sim_*.c`).
6. Prefer `PRIV_REQUIRES`; use `REQUIRES` only for a component whose types appear in your public header.
   Every project component compiles with `-Wall -Wextra -Werror`.

## Adding a field from the controller

| Step | Where | What |
|---|---|---|
| 1 | `protocol/rcc_codec` | decode it, only if the wire format changes; extend `test/host/test_rcc_codec.c` |
| 2 | `protocol/rcc_link/src/rcc_link.c` | copy the value into the snapshot (under the snapshot mutex) |
| 3 | `common/hmi_common/include/rcc_types.h` | add the field to `rcc_snapshot_t` |
| 4 | `ui/ui_present` | add it to the view struct and fill it in the `hp_*_view()`; extend `test/host/test_hmi_present.c` |
| 5 | `ui/ui_app/src/hmi_model.c` | only for a new view: a subject (see `ui/README.md`) |
| 6 | `ui/ui_app/src/pages/page_*.c` | create the widget in `enter`, set it in the `apply_*` observer |

Then give the simulator data for it in `tools/sim/sim_scenarios.c`.

## Adding a module

1. `components/<layer>/<module>/{CMakeLists.txt, include/<module>.h, src/<module>.c}`, using a sibling as
   the template (comment line with the layer and role, `target_compile_options ... -Werror`).
2. Declare only the dependencies the rules above allow. A managed component (lvgl, esp_lvgl_port...) also
   goes into the module's `idf_component.yml`, at the version pinned in `dependencies.lock`.
3. Start it from `app.c`, in the stage it belongs to. Connect it to other modules there.
4. If the UI uses it, add a stand-in to `tools/sim/sim_stubs.c` (and to `tools/sim/stub/` for any IDF header
   its public header includes).

A new layer directory also needs an entry in `EXTRA_COMPONENT_DIRS` in the top-level `CMakeLists.txt`.
