# HMI UI layers

LVGL 9.6 UI for the 480x320 landscape HMI. `src/hmi_ui.c` only implements the public API in `hmi_ui.h`;
everything else lives here.

```
hmi_ui.c               public API (start, select page, open Wi-Fi, sim inject) -> thin wrappers
ui/hmi_shell.c         chrome (rail/header/banner), action bar + confirm modal, OTA progress overlay,
                       1 s rebuild/refresh timer, overlay service (hmi_shell_overlay_open/close)
ui/hmi_nav.c           visible page, page switch (hmi_nav_goto), tap-to-navigate, redraw request
ui/hmi_wifi_ui.c       Wi-Fi flow + on-screen keyboard, drawn on a shell overlay
ui/hmi_pages.c         page registry: {name, rail icon, fault badge, main bar, build, set_sub}
ui/pages/page_*.c      one build function per page (page_device.h: Device tab ids)
ui/hmi_widgets.c       absolute-positioned LVGL primitives (box, card, text, icon, chip, kv, scroll area...)
ui/hmi_theme.c         palette (hmi_theme.h, HC_*/HF_*) and tone -> colour mapping (hmi_theme_present.h)
ui/hmi_present.c       pure logic: text, tones, derived state. No LVGL, host-tested
```

Include edges (arrows point at what is included):

- `hmi_shell` -> `hmi_pages`, `hmi_nav`, `page_device.h` (tab ids for the header taps), `hmi_widgets`, `hmi_theme_present`
- `hmi_nav` -> `hmi_pages`, `hmi_widgets`
- pages -> `hmi_nav` (goto, redraw), `hmi_widgets`, `hmi_present`, `hmi_theme_present`
- `page_device` -> also `hmi_wifi_ui` (opens the Wi-Fi flow) and the settings/backlight/Wi-Fi/OTA services (see rules)
- `hmi_wifi_ui` -> `hmi_shell` (overlay service only), `hmi_widgets`; it includes no page and no `hmi_nav`
- `hmi_widgets` -> `hmi_theme` -> `lvgl`; `hmi_theme.c` -> `hmi_theme_present` -> `hmi_present` -> `rcc_link.h`

`hmi_theme.h` and `hmi_widgets.h` are LVGL-only: they do not pull in `hmi_present.h` or `rcc_link.h`.

## Rules

- Pages never call `rcc_link_*`. The shell takes one snapshot per rebuild and passes it in (`const rcc_snapshot_t *`,
  plus `hmi_page_ctx_t`: height, link state, the action-bar spec). Controller data reaches a page only that way.
  The Device page is the one exception for non-controller data: it also reads and writes `hmi_settings` and
  reads `hmi_wifi` and `hmi_ota`, and its tab is state kept in `page_device.c`. Every other page is a pure
  function of the snapshot and ctx.
- Per-page state stays in the page file (exported setter, wired through the registry's `set_sub`), not in the
  shell, nav or ctx. Per-page shell behaviour is a field of the registry row (`main_bar`, `fault_badge`).
- Logic goes into `hmi_present` (formatting, thresholds, state classification, gauge scaling), with a test in
  `test/host/test_hmi_present.c`. Pages only lay out what `hmi_present` returns and map a tone to a colour with
  `hmi_theme_present.h`.
- The shared palette is defined only in `hmi_theme.h` (`HC_CARD`, `HC_NAV_CUR`, one-off tints such as
  `HC_STOPPING_BG`, `HC_OTA_EDGE`). No hex literals in other UI files.
- Scroll areas use a per-page slot number (`enum` in the page file, below `HW_SCR_SLOTS`); offsets are cleared on
  every page change, so pages number their slots from 0.
- Overlays (full-screen flows) go through `hmi_shell_overlay_open()`; they never touch the shell's page container.
- Work happens under the LVGL port lock, from the LVGL task.

## Adding a page

1. Create `ui/pages/page_foo.c` with `void page_foo_build(lv_obj_t *parent, const rcc_snapshot_t *m, const hmi_page_ctx_t *ctx)`.
   `parent` is a 408 px wide container, `ctx->height` px high.
2. In `ui/hmi_pages.c`, add the prototype next to the others and one row to `hmi_pages[]`
   (`{name, icon, fault_badge, main_bar, build, set_sub}`; `NULL` for `set_sub`). Add an id to `hmi_page_id_t` in
   `hmi_pages.h` only if other code navigates to the page; otherwise append a plain row after the designated ones.

That is all for the build systems: `src/CMakeLists.txt` globs `ui/pages/page_*.c` (touch that file after adding
a page so CMake re-runs) and the simulator Makefile globs `ui/pages/*.c`. The rail and the page switch iterate the table; the rail pitch shrinks above five pages
(`HMI_PAGES_MAX` is 6; a seventh tab needs a scrolling or "More" rail). Files outside `ui/pages/` must still be
listed in `src/CMakeLists.txt` and in `APPSRC` in `tools/sim/Makefile`.

## Checks

```sh
sh tools/sim/check.sh                    # build the PC simulator, render 80 PNGs, diff against the baseline
CC=gcc sh test/host/run.sh               # host unit tests (rcc_codec, hmi_present)
```

`check.sh` must print `PIXEL DIFF: none` for a pure refactor. The baseline must be recorded from the original
(pre-change) commit with `sh tools/sim/check.sh baseline`; `/tmp` is volatile, so redo it after a reboot. Re-record
it only after an intentional visual change. The 80 images cover every scenario x page, the Device tabs, the
command/confirm overlays, the Wi-Fi flow (list, scroll, keyboard, connecting, info, failure) and the OTA overlay.
See `tools/sim/README.md`.

## LVGL Pro / XML

LVGL 9.5+ removed the XML runtime, so LVGL Pro (Editor) output cannot be loaded at run time. If it is used,
export it as C and place the result in `ui/gen/`. Never hand-edit generated files; regenerate them. Bind the
generated widgets through the same layers: colours and fonts from `hmi_theme`, text and state from
`hmi_present`, navigation through `hmi_nav.h`, and data via the snapshot handed to the page.
