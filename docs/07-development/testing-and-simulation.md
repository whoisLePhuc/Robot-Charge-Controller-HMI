# Testing and simulation

Every automated check the project has, what each one proves, what it cannot prove, and how to run it. Replaces `docs/controller-simulator.md` and the summary in `firmware/tools/sim/README.md` (that README stays as the tool's own reference).

Commands were re-run on 2026-11-27 at commit `f96b6bb` in WSL2 (Linux; `gcc`, `make`, `cmake`, `python3`, Pillow 9.0.1, pyserial 3.5). `idf.py` is **not** installed in that environment, so no firmware build was run. **None of these checks touch a board.** Planned or reported tests are not verified results; evidence is recorded in [verification-status](../08-status/verification-status.md).

## 1. The test layers

| Layer | Command (from `firmware/`) | Needs | Proves | Does not prove |
|---|---|---|---|---|
| C host tests | `CC=gcc sh test/host/run.sh` | C compiler | Codec framing and CRC, presentation logic (`hmi_present`), `rcc_link` START gating and parsing against stubs | Anything on the target: UART timing, tasks, LVGL, touch |
| Controller-simulator tests | `python3 -B -m unittest discover -s test/host -p test_controller_sim.py -v` | Python (pyserial optional) | The Python controller model: framing, schemas, START/STOP lifecycle, duplicates, authorization, faults, event paging, reboot | That the real controller behaves the same; that the HMI handles the replies |
| Reference-codec comparison | same, with `RCC_REFERENCE_CODEC_PATH` set | Controller repo checkout | The simulator codec agrees with the controller repository's independent codec | Current controller firmware behaviour |
| Desktop UI simulator | `sh tools/sim/check.sh` | `make`, `gcc`, Pillow, LVGL in `managed_components/` | UI renders; pixel stability against a baseline; in-place updates equal fresh builds; command taps reach callbacks | Real display, fonts at panel gamma, touch, memory, timing |
| Controller simulator on hardware | `python3 tools/controller_sim.py --port ...` | HMI board, 3.3 V USB-UART adapter | (When run) the HMI link end to end with synthetic data | Real controller, relay, charging |
| Firmware build | `idf.py build` | ESP-IDF 6.1.0 | Compiles and links | Runtime behaviour |

## 2. C host tests

```sh
cd firmware
CC=gcc sh test/host/run.sh        # Windows: test/host/run.ps1 (MinGW gcc on PATH)
```

Builds and runs three programs with `-std=c11 -Wall -Wextra -Werror`:

| Program | Covers | Notes |
|---|---|---|
| `test_rcc_codec` | `rcc_codec` encode/decode against golden vectors | Should reuse the controller's vectors ([framing-and-transport §2.2](../04-controller-protocol/framing-and-transport.md#22-canonical-object-canmd-5)) |
| `test_hmi_present` | `hmi_present`: freshness, START gating, fault state, bar/banner selection, formatting | Pure C; needs only `rcc_types.h` |
| `test_rcc_link` | `rcc_link.c` included directly to reach its static state; ESP-IDF calls come from `test/host/stubs/` | Covers START admission (online, status age 1500 ms boundary, status length, simulated-ready) and STOP acceptance |

Result on 2026-11-27: `ALL PASSED`; `test_hmi_present: 304 checks, 0 failed` at `f96b6bb`, **308 checks, 0 failed** after the G2 change (4 new checks); `rcc_link host tests passed`.

`run.sh` writes its binaries to `/tmp/test_*`.

Gaps: G2 (STOP + `QUEUE_FULL` wording) is now asserted; there is still no host test for authority gating; see [operator-safety-behavior §9](../05-ui/operator-safety-behavior.md#9-known-gaps).

## 3. Controller-simulator tests

```sh
cd firmware
python3 -B -m unittest discover -s test/host -p test_controller_sim.py -v
```

33 tests cover, among others: CRC and COBS boundaries, maximum object with arbitrary chunking, fragment loss and expiry, wrong destination/source/id, query schemas, START lifecycle and same-id retry, duplicate and conflict handling, STOP before/after relay, STOP failed and `QUEUE_FULL` still turning the relay off, ledger expiry and fullness, measurement invalid/stale/floor/missing/signed, three-frame event pages and overwrite, reboot cancelling pending commands, link silence not stopping a session.

Results on 2026-11-27:

| Run | Result |
|---|---|
| Default | 33 tests, OK, **1 skipped** (the independent-codec test) |
| With `RCC_REFERENCE_CODEC_PATH=<controller repo>/tools/calibration/src/rcc_calibration_tool/protocol/codec.py` | 33 tests, **OK, 0 skipped** |

The reference file was last modified 2026-09-25 in the controller checkout, so this cross-check predates later controller protocol changes. With pyserial installed, the serial-loop test runs without hardware.

## 4. Controller simulator on a real HMI

`firmware/tools/controller_sim.py` pretends to be the controller on a serial port. **This workflow has never been verified in this repository** ([verification-status](../08-status/verification-status.md)).

Wiring: [controller-wiring §To the PC simulator](../03-hardware/controller-wiring.md#to-the-pc-simulator-instead-of-the-controller). Disconnect the real controller first. The simulator holds DTR/RTS deasserted so it does not toggle auto-reset or BOOT lines.

```sh
cd firmware
python -m pip install -r tools/controller_sim_requirements.txt     # pyserial==3.5
python tools/controller_sim.py --port COMx [--scenario S] [--verbose] [--no-console] [--duration N]
```

- `--scenario`: `ready` (default, OPERATIONAL/IDLE, valid data, no faults), `charging`, `fault` (overcurrent), `inhibited` (remote), `unauthorized` (`authority deny`).
- Interactive commands (`help` prints them): `ready`, `charging`, `complete`, `reboot`, `status`, `quit`; `fault overcurrent|voltage|reverse|adc-stale|internal|nvs|not-established|clear`; `inhibit remote|reset|recovery|clear`; `authority allow|deny`; `link normal|silence`; `measurement valid|invalid|stale|floor|missing`; `control normal|accepted-only|start-fail|stop-unconfirmed|stop-fail|stop-queue-full`; `wire normal|bad-crc|drop-next`; `delay 0..20000`; `values VOUT_mV IOUT_mA`; `events 1..1000`.
- To restore START after failure injection: `control normal`, `ready`, `measurement valid` on separate lines.
- All readings, states and relay values are **test data**: not measurements, timing guarantees, relay feedback or evidence of charging.

Useful checks once wired: `authority deny` then START (should show the no-authority banner for 15 s), `link silence` (offline banner, session unaffected), `control stop-queue-full` then STOP (observe gap G2 wording), `wire bad-crc` (bad-frame counter on Device › Comm link).

## 5. Desktop UI simulator

`firmware/tools/sim/` compiles the real UI components (`ui_app`, `ui_core`, `ui_present`, `ui_assets`) and the generated layouts from `lvgl_ui/components/hmi/*/*_gen.c` with LVGL on a PC. The link, services and board are replaced by `sim_*.c` stubs. No ESP-IDF is needed; LVGL sources come from `firmware/managed_components/lvgl__lvgl`, so run `idf.py build` or `idf.py reconfigure` once first (or ensure that directory exists).

```sh
cd firmware/tools/sim
make -j"$(nproc)"                         # default OUT=/tmp/hmi-sim; first build compiles LVGL (about 1.5 minutes here)
python3 render.py out --scale 1           # 80 PNGs: scenario x page, Device tabs, command/Wi-Fi/OTA overlays
```

Override `OUT=` to build elsewhere. `check.sh` always uses `/tmp/hmi-sim`, `/tmp/hmi-baseline`, `/tmp/hmi-after`.

### Regression workflow

```sh
sh tools/sim/check.sh baseline    # once, on the commit to compare against
sh tools/sim/check.sh             # after the change
```

It prints `PIXEL DIFF: none` when all images are identical to the baseline, then runs `check_morph.py` (`MORPH DIFF: none`) and `check_commands.py` (`COMMAND TAPS: ... passed`). Record the baseline from the **original** commit; `/tmp` is volatile.

Scenarios: `CHARGING IDLE PRECHECK INHIBITED FAULT OFFLINE REMOTE`, with suffixes `+aged`, `+evt`, `+sim`, `+simrdy`, `+stale`, `+nopri`, `+quiet`. Pages 0–4 are Overview, Measure, Faults, Session, Device. Overlays: `cmd=stopping|stopped|timeout|queuefull|noauth|accepted`; Wi-Fi `click=wifi=list|scroll|taps|info|open|pass|sym|show|shift|busy|done|fail|noap|noip`; `ota=receiving|verifying|success`. Edit `sim_scenarios.c` to change the data.

### Results on 2026-11-27

All commands were run into a scratch directory, not `/tmp`.

| Check | Result |
|---|---|
| Build | Succeeds. 442 warnings, all one kind: `LV_COLOR_DEPTH is deprecated` from `tools/sim/lv_conf.h`. No warning from UI sources |
| Render | 80 PNGs |
| `MORPH DIFF` | none (384 update transitions + 10 scroll checks identical to a fresh build) |
| `COMMAND TAPS` | 4 checks passed |
| Pixel comparison with the pre-LVGL-Pro commit `b6585c6` | 77 of 80 identical; the 3 that differ are `ovl-wifi-busy`, `ovl-wifi-list`, `ovl-wifi-scroll` (consistent with the recorded "Wi-Fi scrollbar and spinner differ") |

The comparison used a temporary worktree of `b6585c6` built and rendered the same way; it was removed afterwards. The differing images were **not** inspected, so whether they are acceptable is undecided.

### Coverage gaps

- Scenarios set `is_control = false` and `control_interface = 0`, so Device › Controller always shows `NONE`; the authority states (`THIS HMI`, `UART`, …) have no render coverage.
- The scenario build flags still set bit2, which is reserved and always 0 on current controllers.
- The `cmd=queuefull` overlay is the only render of `REJECTED/QUEUE_FULL` on STOP; since the G2 change it shows "Stop took effect, not tracked". Its scenario data still shows state `CHARGING` and relay `CLOSED` behind the message, because the simulated snapshot does not change after the STOP; on a real controller the state would follow. No scenario exercises `FAILED` on STOP.
- The baseline predates the LVGL Pro migration for the Wi-Fi views; re-record after an intended visual change.
- The simulator renders at 1×/2× on a PC; it does not reproduce the ST77922 panel, colour depth or memory limits.

## 6. Preview and mockup tools

`tools/snapshot/` (CMake) renders the UI components to 480 × 320 bitmaps; `export_previews.py` writes them to `assets/ui/` at 2× (needs Pillow). `export_mockup_previews.py` captures the HTML mockup with Playwright and Edge or Chrome at 3×. Both are for README previews, **not** regression checks. The commands are in the root README (“Regenerate the previews”). They were not re-run for this page.

## 7. What is not tested anywhere

Display and touch on the board, real UART with a real controller, memory headroom (LVGL 128 KB heap), task timing and watchdog behaviour, Wi-Fi and OTA, rollback, long runs. These are the gates listed in [verification-status §5](../08-status/verification-status.md#5-gates-to-close-this-page).
