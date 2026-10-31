# HMI simulator (PC render of the real UI code)

Compiles `src/hmi_ui.c`, `src/ui/` (theme, widgets, pages, shell, Wi-Fi flow, presentation logic), the generated fonts/icons and LVGL 9.6 (from `managed_components`)
for the PC, feeds the UI simulated controller snapshots (`sim_scenarios.c`) and writes PNGs.
No ESP-IDF needed. Layout, fonts, colours: compare with `docs/mockups/hmi-ui-landscape.html`.

```sh
cd firmware/tools/sim
make -j4                 # first build compiles LVGL (a few minutes)
python3 render.py out --scale 2     # every scenario x page, overlays, contact sheets (needs pillow)
/tmp/hmi-sim/hmi_sim CHARGING 2 out.ppm [tab=link] [cmd=stopping] [click=confirm]
```

Refactor check: `sh check.sh` (Git Bash on Windows too) rebuilds the simulator, renders every scenario/page/overlay
(80 PNGs: scenario x page, Device tabs, command overlays, Wi-Fi flow views incl. keyboard, OTA progress) into
`/tmp/hmi-after` and compares pixel-for-pixel with `/tmp/hmi-baseline`; it prints `PIXEL DIFF: none` when identical.
The baseline must come from the original (pre-change) commit: check it out and run `sh check.sh baseline`
(`/tmp` is volatile, so repeat after a reboot; only re-record after an intended visual change). Without a baseline,
`check.sh` stops with a message saying so. Render and build failures print the tail of their log.
Layering and how to add a page: `src/ui/README.md`.

Scenarios: CHARGING IDLE PRECHECK INHIBITED FAULT OFFLINE REMOTE. Pages 0-4 = Overview,
Measure, Faults, Session, Device. Command overlays: `cmd=stopping|stopped|timeout|queuefull|noauth|accepted`. Wi-Fi flow: `click=wifi=list|scroll|taps|info|open|pass|sym|show|shift|busy|done|fail|noap|noip`.
OTA progress overlay: `ota=receiving|verifying|success`.
Edit the numbers in `sim_scenarios.c` to try other data. `lv_conf.h` mirrors the Elecrow LVGL font configuration.

Regenerate fonts / icons (needs node `lv_font_conv`, python `cairosvg pillow`):
`firmware/tools/gen_icons.py` (icons from the mockup SVGs) and `lv_font_conv` commands in
`firmware/src/hmi_fonts/README.md`.
