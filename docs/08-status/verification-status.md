# Verification status

The single place that records what has been checked, when, on which revision, and what has not. Other pages link here instead of restating status.

**Rules for this page**

- *Implemented in source* is not *verified*. A feature counts as verified only with a recorded check below.
- Each row says whether the result was **re-run for this page** or only **carried over** from earlier repository notes.
- Update the row (date, revision) whenever you re-run a check. Do not edit a result without running it.

Current revision for this page: `f96b6bb` (branch `main`).

## 1. Checks re-run on 2026-11-27 at `f96b6bb`

Environment: WSL2 (Linux), `gcc`, `python3`, `pyserial` absent.

| Check | Command (from `firmware/`) | Result |
|---|---|---|
| C host tests | `CC=gcc sh test/host/run.sh` | `ALL PASSED` (`rcc_codec`); `test_hmi_present: 304 checks, 0 failed`; `rcc_link host tests passed`. Re-run after the G2 change: `test_hmi_present: 308 checks, 0 failed` |
| Controller simulator tests | `python3 -B -m unittest discover -s test/host -p test_controller_sim.py` | 33 tests, OK, 1 skipped |

Added later the same day (same revision, scratch build directory, nothing written to the repository):

| Check | Command | Result |
|---|---|---|
| Controller-simulator tests with the controller repository's independent codec | `RCC_REFERENCE_CODEC_PATH=<controller>/tools/calibration/src/rcc_calibration_tool/protocol/codec.py` + the unittest command above | 33 tests OK, 0 skipped (the reference file dates from 2026-09-25) |
| Desktop UI simulator build and render | `make` in `firmware/tools/sim`; `render.py` | Builds (442 warnings, all `LV_COLOR_DEPTH is deprecated` from `tools/sim/lv_conf.h`); 80 PNGs |
| Retained-page morph check | `check_morph.py` | `MORPH DIFF: none` (384 transitions + 10 scroll checks) |
| Command-tap check | `check_commands.py` | 4 checks passed |
| Pixel comparison with pre-LVGL-Pro commit `b6585c6` | same simulator built and rendered from a temporary worktree | 77 of 80 identical; `ovl-wifi-busy`, `ovl-wifi-list`, `ovl-wifi-scroll` differ. The differing images were not inspected |

These verify logic and rendering on a PC only. They say nothing about the board. `idf.py` is not installed in that environment, so the firmware build was **not** re-run.

## 2. Results carried over, not re-run for this page

Source: `README.md`, the old `docs/README.md`, and the archived [restructure design](../09-decisions/archive/firmware-restructure-design.md). Treat as *reported*.

| Area | Reported result | Reported date | Note |
|---|---|---|---|
| Firmware build | ESP-IDF 6.1.0 build succeeds; generated LVGL layout sources emit deprecation warnings | 2026-11-27 | Earlier phase-1/2 builds were reported with 0 warnings before the LVGL Pro sources were added |
| `sdkconfig` equivalence | Generated config matches the former PlatformIO config except the partition file name | phase 1 | |

## 3. Not verified

Nothing below has a recorded result. Do not describe it as working.

| Area | Status |
|---|---|
| **Flashed image** | The restructured ESP-IDF + LVGL Pro image **has not been flashed to a board**. Earlier PlatformIO-era bring-up reported LCD/touch operation and flashing of a five-page image; that does not cover the current image |
| Display and touch on the current image | Not verified |
| Boot log versus expected stages (Flash/PSRAM, TWDT, UART1 start, OTA mark valid) | Not compared |
| Controller connector mapping | Source assigns UART1 to GPIO43/GPIO44; the physical connector pinout is **not verified** |
| HMI ↔ simulator over a real adapter | Not recorded for the current image |
| HMI ↔ real controller communication | Not verified |
| START / STOP authority end to end | Controller side reported done on branch `feat/single-image` and bench-checked once (2026-10-30, see [controller-change-requests.md](../04-controller-protocol/controller-change-requests.md)); not repeated on the current HMI image |
| Charging session, relay operation | **Never exercised from this HMI.** No claim is made. The controller project records (2026-11-20, `firmware-open-work.md` B6) an *owner report* that the HMI was tested on a controller board and works, with **no bench record** (image, CONFIG, observations). That is not verification evidence, and a START reaching `CHARGING` is explicitly listed there as not verified |
| Operator-behaviour gaps G1–G6 | Documented in [operator-safety-behavior §9](../05-ui/operator-safety-behavior.md#9-known-gaps); none assessed or accepted by the human design authority. **G2 has a software change** (2026-11-27): host-tested and rendered on the PC simulator only; no review, no hardware check |
| Settings persistence | Not verified on hardware |
| Wi-Fi scan/connect/forget | Not verified on hardware |
| Web OTA, rollback injection | Not verified |
| Long-duration run, memory headroom (LVGL 128 KB heap setting) | Not verified |
| Visual acceptance of all pages and states on the LCD | Pending |
| Language switching | Stored only; main labels remain English |

## 4. Known inconsistencies in the older documentation

Found while building this documentation set. Tracked so they can be fixed during migration.

| Item | Where | Problem |
|---|---|---|
| UI source of truth | `docs/mockups/`, `assets/ui/` | Captured from the HTML mockup with illustrative data; **not** compared with the current layouts. `ui-pages-and-information.md` was rebuilt from source as `05-ui/ui-pages-and-states.md` and removed |
| Revision stamp | `06-services/settings-wifi-ota.md` | Stamped `28d2253` (2026-10-30), before the ESP-IDF restructure and LVGL Pro; the settings page was partly spot-checked |
| Protocol extract | `04-controller-protocol/code-tables.md` | Fault/inhibit tables carried over from controller commit `f72ec7b`; not re-compared with current controller headers. The former condition A2 (`operational_node_authorized`) was found obsolete and removed (retired 2026-10-30) |
| Resolved in the split | former `controller-interface.md`, `elecrow-s3-bringup.md` | GET_STATUS byte 19/bit5 were listed as reserved; start-up order said display init started the link. Both corrected in the new pages |
| Obsolete paths | archived `firmware-restructure-design.md` | Uses `new_firmware/...`, now `firmware/...` (the document says so itself) |
| Preview images | `README.md`, `assets/ui/` | Captured from the HTML mockup with illustrative data, not from the device or the current layouts |

## 5. Gates to close this page

The detailed, per-test procedures and record tables are in [hardware-test-plan](hardware-test-plan.md).

In order, each on a recorded revision and hardware:

1. Build the current revision and record the warning list.
2. Flash a board; compare the boot log with the start order in `firmware/main/app.c`.
3. Confirm display, touch and every page visually.
4. Wire the controller simulator; run the scenarios in the simulator guide.
5. Connect a real controller (no charging); verify read commands and the authority display.
6. Only then consider START/STOP and any charging test, under the controller project's own procedures.
7. Wi-Fi, OTA and rollback on a bench unit.

No step here authorises charging operation; that decision belongs to the controller project and a qualified person.
