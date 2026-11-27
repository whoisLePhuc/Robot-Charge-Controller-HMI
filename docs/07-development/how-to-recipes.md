# How-to recipes

Short, ordered procedures for the changes people make most often. Each recipe says where to work, what to test and what documentation to update. The detailed rules stay in `firmware/components/README.md` and `firmware/components/ui/README.md`; recipes link there instead of copying them.

Compiled on 2026-11-27 from source at `f96b6bb`. Recipes 1–3 and 6–7 rely on those READMEs and the code and were **not executed end to end**; no recipe was tried on a target.

Always finish with: `CC=gcc sh test/host/run.sh`, then (UI changes) `sh tools/sim/check.sh` ([testing-and-simulation](testing-and-simulation.md)), then update the docs listed in [§ Keep the docs in step](#keep-the-docs-in-step).

## 1. Show a new controller value

Use when the controller adds a field to a response. Reasons for each step: [data-flow §6](../02-architecture/data-flow.md#6-adding-a-controller-field-where-it-goes).

1. Confirm the field in the controller ICD and update [commands-and-payloads](../04-controller-protocol/commands-and-payloads.md) first. Mark it `confirmed`, `inferred` or `not-frozen`.
2. `rcc_codec`: only if the wire format changed; add a vector to `test/host/test_rcc_codec.c`.
3. `rcc_link.c: parse_reply_data`: copy the value into `s_model` (the function already runs under the model mutex). Add a case to `test_rcc_link.c` if gating depends on it.
4. `rcc_types.h`: add the field to `rcc_snapshot_t`.
5. `hmi_present`: add it to the relevant view and fill it in the `hp_*_view()`; extend `test_hmi_present.c`.
6. New view only: `hmi_model.c` (`views_t`, `fill()`, `tbl[]`, extern in `hmi_model.h`).
7. `pages/page_*.c`: create the widget in `enter`, set it in the observer.
8. Layout: add the widget in the LVGL Pro component with an explicit name ([lvgl-pro-workflow](../05-ui/lvgl-pro-workflow.md)).
9. Give the desktop simulator data in `tools/sim/sim_scenarios.c` and the controller simulator the field in `tools/controller_sim.py`.

## 2. Change how something looks

**Rule:** a UI-visible change is made in both `lvgl_ui` (layout) and `firmware/components/ui` (data and behaviour) in the same change. Work that needs both is planned and reviewed as one unit (see the deferral of G3 in [operator-safety-behavior](../05-ui/operator-safety-behavior.md#g3-options-recorded-for-later)).

[lvgl-pro-workflow](../05-ui/lvgl-pro-workflow.md): edit the component in the Editor, Ctrl+B, keep widget names, commit XML and generated C together, run `check.sh`. Expect pixel differences; re-record the baseline deliberately.

## 3. Add a widget to an existing page

1. Add the widget to the page's layout component in the Editor (explicit name).
2. Add the field to the view struct in `hmi_present.h`, fill it in `hp_*_view()`, add a host test.
3. In `page_*.c` `enter`, find the widget with `hmi_data_widget(root, "name")`, keep its pointer in file-static state, and set it in the existing `apply_*` observer. Clear the pointer in `leave`.
4. If the content's shape depends on data, use `update()` with a signature function in `hmi_present`; do not delete or rebuild from an observer.

Rules (z-order, re-measuring widths, scroll slots, 128 KB heap): `firmware/components/ui/README.md`, section "Retained mode".

## 4. Add a page

Follow "Adding a page" in `firmware/components/ui/README.md`: one row in `hmi_pages.c`, a `page_foo.c`, an optional id in `hmi_page_id_t`. After adding a `page_*.c` file touch `ui_app/CMakeLists.txt` so CMake re-runs. The rail holds up to `HMI_PAGES_MAX` = 6 tabs. Add the page to `MIGRATED` in `tools/sim/check_morph.py` when it becomes retained, and document it in [ui-pages-and-states](../05-ui/ui-pages-and-states.md).

## 5. Add a module (component)

"Adding a module" in `firmware/components/README.md`: layer directory, `CMakeLists.txt` with only the allowed dependencies, start it from `app.c`, add a stand-in to `tools/sim/sim_stubs.c` if the UI uses it. A new layer directory also goes into `EXTRA_COMPONENT_DIRS`. Update [firmware-architecture](../02-architecture/firmware-architecture.md).

## 6. Add or rename a fault, inhibit or reason name

Names live in `hmi_present.c`:

| What | Where |
|---|---|
| Fault long names and ids | `hp_fault_names[]`, `fault_ids[]` (same order = bit order); `HP_FAULT_COUNT` in `hmi_present.h` |
| Fault short names | `s[]` inside `hp_fault_short` |
| Fault action hints | `hp_fault_hint` (ranges by id) |
| Inhibit names | `hp_inhibit_names[]`; `HP_INHIBIT_COUNT` |
| Reason names | `names[]` in `hp_reason_text` (`0x00`–`0x16`) |
| Session end reasons | `hp_end_names[]`; `HP_END_COUNT` |

Steps: confirm against the controller registry first (`rcc_fault_registry.h`, ICD), because bit positions are `not-frozen` (CR-02). Update the tables **and** [code-tables](../04-controller-protocol/code-tables.md), extend `test_hmi_present.c`, check the Faults page layout still fits (list sizes), and re-render. Unknown bits must still show raw (`Unrecognized (bit N)`).

## 7. Change START/STOP, banner or action-bar behaviour

Read [operator-safety-behavior](../05-ui/operator-safety-behavior.md) first; this touches relay-related operator behaviour.

1. Write the intended behaviour and the controller reference (ICD section) in the change description.
2. Put the logic in `hmi_present` (`hp_start_allowed`, `hp_bar_spec`, `hp_banner`) with a host test; keep `rcc_link_request_*` consistent (the UI is advisory, `rcc_link` and the controller are the gates).
3. Run the controller-simulator scenarios that exercise it (`authority deny`, `control stop-queue-full`, `link silence`, `fault overcurrent`).
4. Update the safety page (rules table and known gaps) and [verification-status](../08-status/verification-status.md) in the same change.
5. A bench result cannot waive a requirement; record who reviewed it.

## 8. Add a desktop-simulator scenario

Edit `tools/sim/sim_scenarios.c`: add a branch in `scenario()` that starts from `base()` and sets the snapshot fields and `ev(...)` events. Add the name to `SCEN` in `tools/sim/render.py` (the morph check iterates the scenarios; see `check_morph.py`). Remember the existing scenarios never set `is_control` or `control_interface`.

## 9. Add a controller-simulator command

In `tools/controller_sim.py` add the case to `Controller.command` and the line to `HELP`. Add a test in `test/host/test_controller_sim.py`. Keep the schema identical to the controller ICD (the optional reference-codec test helps).

## 10. Regenerate icons or fonts

Icons: `python firmware/tools/gen_icons.py` (needs `cairosvg`, `pillow`); it writes `ui_assets/include/hmi_icons.h` and `src/hmi_icons.c`. Fonts: `lv_font_conv` commands in `firmware/components/ui/ui_assets/src/fonts/README.md`. Both change generated files: re-render and review pixel diffs. Fonts in `lvgl_ui/fonts` are Editor assets only and do not change the device ([lvgl-pro-workflow](../05-ui/lvgl-pro-workflow.md)).

## 11. Change a pin, UART setting or watchdog timeout

| Item | File |
|---|---|
| Controller UART port, pins, baud; watchdog timeout | `firmware/components/common/hmi_common/include/hmi_config.h` |
| LCD, touch, backlight pins | `firmware/components/bsp/bsp_elecrow_s3/src/bsp_pins.h` |

Update [elecrow-s3-board](../03-hardware/elecrow-s3-board.md) and [controller-wiring](../03-hardware/controller-wiring.md), and mark them unverified until measured. Do not change the watchdog timeout, task stack or priority, or LVGL heap size without a recorded measurement.

## Keep the docs in step

| You changed | Update |
|---|---|
| Wire fields, commands, codes | `04-controller-protocol/*` |
| What a screen shows | `05-ui/ui-pages-and-states.md` |
| START/STOP, authority, link-loss behaviour | `05-ui/operator-safety-behavior.md` |
| Layers, tasks, start-up order | `02-architecture/firmware-architecture.md`, `data-flow.md` |
| Pins, partitions, sdkconfig | `03-hardware/*`, `07-development/build-and-flash.md` |
| Tests or tooling | `07-development/testing-and-simulation.md` |
| Anything verified or not | `08-status/verification-status.md` (only with a recorded result) |
| A design decision | a new ADR in `09-decisions/` |
