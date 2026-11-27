# ADR-0002: Author UI layouts in an LVGL Pro project (`lvgl_ui/`)

- **Status:** Accepted and implemented. The rationale below was stated by the project owner on 2026-11-27. Open questions remain.
- **Date of the change:** 2026-11-27 (commit `f96b6bb`, "Track the LVGL Pro HMI project and integrate its C export")
- **Sources:** *Context*, *Decision* and *Consequences* come from the repository. *Why* is the project owner's statement, recorded as given. Statements about LVGL Pro features in *Why* are the owner's; the repository does not yet contain a Figma-based workflow, so that benefit is **expected, not demonstrated**.

## Context (from the repository)

- Before this change the screens were built in hand-written C (`hmi_shell.c`, `page_*.c`, `hmi_widgets`, in the `ui_core`/`ui_app` components). A browser mockup (`docs/mockups/hmi-ui-landscape.html`) was kept "in step" by hand with that code.
- The PC desktop simulator (`firmware/tools/sim`) rendered those C components, with an 80-image pixel baseline and a morph check for retained pages.
- The firmware also targets a constrained LVGL heap (the earlier design record cites 64 KB; the current defaults set 128 KB).

## Decision (from the repository)

- Add `lvgl_ui/`, an LVGL Pro Editor project (LVGL 9.6.0, 480 × 320), under version control.
- Compile only the generated layout components `lvgl_ui/components/hmi/*/*_gen.c` into `ui_app`, through a force-included shim (`hmi_lvgl_export.h`).
- Keep data, behaviour and callbacks in hand-written C. The C finds named widgets with `lv_obj_find_by_name`.
- Keep production fonts and icons in `ui_assets`; the Editor's `lvgl_ui/fonts` and `images` are for previews.
- Enable `CONFIG_LV_USE_OBJ_NAME=y` and raise `CONFIG_LV_MEM_SIZE` to `131072` (`firmware/sdkconfig.defaults`). The UI README states this exceeds the earlier 64 KiB constraint and **requires board validation**.

Details: [lvgl-pro-workflow](../05-ui/lvgl-pro-workflow.md).

## Why (project owner, 2026-11-27)

The aim is to make later changes to the firmware's UI easier.

1. **Faster feedback.** With pure hand-written C, a layout change can only be seen after flashing the board, which costs a lot of time. With LVGL Pro the layout can be edited and previewed in the Editor (and in its built-in desktop simulator) without flashing.
2. **Easier UI design later.** LVGL Pro offers a design workflow with Figma, which the owner expects to make future UI design easier.

**Alternative named by the owner:** keep building the UI in pure C and wait for a flash to see the result, rejected for the reason in point 1. No other alternative was recorded.

**Notes**
- The repository already had a PC desktop simulator (`firmware/tools/sim`) that renders the C UI without a board, so point 1 compares against the *board* loop, not against having no preview at all. The Editor adds visual editing; the simulator remains the regression check.
- Point 2 depends on the Figma integration, which is not yet used or documented here. When it is, add the procedure to [lvgl-pro-workflow](../05-ui/lvgl-pro-workflow.md).

## Consequences observed in the repository

| Effect | Evidence |
|---|---|
| Layout and logic are split across two directories; renaming a widget in the Editor can break the firmware at run time, not build time | `hmi_data_widget()` asserts |
| Generated C is committed (224 `*_gen` files in `lvgl_ui/`) and must be committed with its XML | `git ls-files lvgl_ui` |
| LVGL heap setting doubled | `sdkconfig.defaults` |
| Pixel baseline after the move: 77 of 80 images identical to the pre-migration render; the Wi-Fi busy, list and scroll images differ | Re-run 2026-11-27, [testing-and-simulation §5](../07-development/testing-and-simulation.md#5-desktop-ui-simulator) |
| The HTML mockup and README previews are no longer produced from the same source as the firmware | [verification-status §4](../08-status/verification-status.md) |
| Generated layout sources build with LVGL deprecation warnings | README project status (carried over, not re-run) |

## Open questions

- Is the 128 KB LVGL heap acceptable on the board? No measurement has been recorded.
- Will Figma be part of the workflow? If so, who owns the design file and how does an import map to the named `rcc_*` components and widget names that the firmware looks up?
- Is the mockup still to be maintained, or retired in favour of Editor previews?
- Who may run the Editor and export, and is the Editor version pinned?
- Should a CI or script check that every name looked up by firmware exists in the layouts?
