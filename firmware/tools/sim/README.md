# HMI simulator (PC render of the real UI code)

Compiles `src/hmi_ui.c`, the generated fonts/icons and LVGL 9.6 (from `managed_components`)
for the PC, feeds the UI simulated controller snapshots (`sim_scenarios.c`) and writes PNGs.
No ESP-IDF needed. Layout, fonts, colours: compare with `docs/mockups/hmi-ui-landscape.html`.

```sh
cd firmware/tools/sim
make -j4                 # first build compiles LVGL (a few minutes)
python3 render.py out --scale 2     # every scenario x page, overlays, contact sheets (needs pillow)
/tmp/hmi-sim/hmi_sim CHARGING 2 out.ppm [tab=link] [cmd=stopping] [click=confirm]
```

Scenarios: CHARGING IDLE PRECHECK INHIBITED FAULT OFFLINE REMOTE. Pages 0-4 = Overview,
Measure, Faults, Session, Device. Command overlays: `cmd=stopping|stopped|timeout|queuefull|noauth|accepted`.
Edit the numbers in `sim_scenarios.c` to try other data. `lv_conf.h` mirrors `sdkconfig.esp32dev`.

Regenerate fonts / icons (needs node `lv_font_conv`, python `cairosvg pillow`):
`firmware/tools/gen_icons.py` (icons from the mockup SVGs) and `lv_font_conv` commands in
`firmware/src/hmi_fonts/README.md`.
