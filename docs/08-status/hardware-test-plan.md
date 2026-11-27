# Hardware test plan (pending)

Everything that could **not** be done without the board, the controller or an instrument, collected so it can be run later and recorded. This is a plan, not a result: no row here has been executed. Results go in the tables below and, once recorded, into [verification-status](verification-status.md).

Written 2026-11-27 for HMI commit `f96b6bb`. "Expected" values come from reading the source or the controller documents; they are what to compare against, not evidence that the behaviour exists.

## Ground rules

- **This plan does not authorise charging.** Tests marked *no load* must be run with the charge path inactive: no charger, no battery, relay output not connected to a load. Any test that closes the relay on a load belongs to the controller project's own procedures and a qualified person, and is out of scope here ([§ K](#k-blocked-until-the-controller-is-ready)).
- A bench result cannot waive a requirement. Record facts; the human design authority decides what they mean.
- Keep "tool ran", "test passed" and "requirement verified" separate.
- Record **every** result with the evidence tuple below, including failures and surprises.
- Stop and write down anything unexpected before changing the setup.

## Evidence tuple (fill one per session)

| Field | Value |
|---|---|
| Date, tester | |
| HMI commit (`git describe`, also Device › Settings) and `firmware.bin` SHA-256 | |
| ESP-IDF version, build host, warning count | |
| Board (module, revision, flash/PSRAM as read from the boot log) | |
| Peer: simulator (`controller_sim.py` commit, adapter) or real controller (image hash, `GET_DEVICE_INFO` revisions, CONFIG generation, `operational_interface`/`node`) | |
| Power source and cable/adapter used | |
| Instruments (scope, multimeter, logic analyser, serial capture) and their calibration status | |
| Raw logs and photos stored at | |

## Result key

`PASS` observed as expected · `FAIL` observed differently · `BLOCKED` could not run (say why) · `N/A`. Write what was **observed**, not only the verdict.

---

## A. Build, flash and first boot

| ID | Test | Steps | Expected | Result / observed |
|---|---|---|---|---|
| A1 | Firmware builds | In an ESP-IDF 6.1.0 shell: `cd firmware && idf.py build` | Build succeeds. Record the full warning list (generated LVGL layout sources are reported to emit deprecation warnings; hand-written code is `-Werror`) | |
| A2 | Image size and partition headroom | Read `firmware.bin` size | Fits an app slot (0x380000). The restructure record reported 0x157480 before the LVGL Pro sources; record the new size | |
| A3 | Flash over USB | `idf.py -p COMx flash monitor` | Flash succeeds on native USB; no other monitor holds the port | |
| A4 | Boot log matches the start order | Capture the log from reset | In order: boot reason; `Flash: 16777216 bytes; PSRAM: 8388608 bytes`; `TWDT initialized (5000 ms timeout, panic enabled)`; LVGL task subscribed to TWDT; `Five-page HMI ready; controller link started on UART1 (TX=GPIO43, RX=GPIO44)`; then `Alive; free internal heap…` every 5 s | |
| A5 | Memory-mismatch guard | (Only if a differently configured module is available) boot it | `Memory configuration does not match…` and no UI. Skip otherwise | |
| A6 | Warm and cold reset | Power-cycle, then press reset | Same sequence each time; no panic | |
| A7 | Sdkconfig sanity | `idf.py menuconfig` or inspect `sdkconfig` | `CONFIG_LV_USE_OBJ_NAME=y`, `CONFIG_LV_MEM_SIZE=131072`, rollback enabled, PSRAM octal 80 MHz | |

## B. Display, touch and appearance

| ID | Test | Steps | Expected | Result / observed |
|---|---|---|---|---|
| B1 | Orientation and geometry | Look at the first screen | Landscape 480 × 320, not mirrored or offset; rail at left, content from x = 64 | |
| B2 | Every page renders | Visit Overview, Measure, Faults, Session, Device (3 tabs) | No clipping, overlap or missing icons; text legible | |
| B3 | Navigation by touch | Tap each rail tab, the header chips and a tappable banner | Pages switch; chips go to Device › Controller / Comm link and Faults | |
| B4 | Touch accuracy at edges | Tap near all four corners and the START/STOP area | Taps land where expected; no ghost or double taps | |
| B5 | Compare with the design references | Photograph each page; compare with `assets/ui/*.png` and the mockup | Record every visible difference. The references use illustrative data and predate the LVGL Pro layouts | |
| B6 | The three Wi-Fi images that differ in the PC simulator | Open Wi-Fi busy, list and scroll views | Decide whether the scrollbar and spinner differences versus the pre-migration render are acceptable (`ovl-wifi-busy`, `ovl-wifi-list`, `ovl-wifi-scroll`) | |
| B7 | Backlight | Device › Settings: set 25 / 50 / 75 / 100 % | Brightness changes; no flicker | |
| B8 | Dimming and wake | Set sleep timeout 1 min; wait | Dims to the idle level (default 15 %); the first touch only wakes the screen and is not acted on | |
| B9 | Refresh pauses | Hold a finger down; open the START dialog; open Wi-Fi | Header/banner/state freeze meanwhile and resume afterwards (documented behaviour, see G3) | |
| B10 | Language setting | Choose Tiếng Việt | Selection stored; labels stay English (known limitation) | |

## C. Wiring and electrical

| ID | Test | Steps | Expected | Result / observed |
|---|---|---|---|---|
| C1 | **Connector mapping** | Find which Elecrow connector pins carry GPIO43 (TX) and GPIO44 (RX); record the pinout | Record it, then update [controller-wiring](../03-hardware/controller-wiring.md). **Not verified anywhere yet** | |
| C2 | Logic level | Measure TX idle and swing with a multimeter/scope, unloaded and connected | 3.3 V logic, not RS-232 or 5 V. Record measured levels and any tolerance concern | |
| C3 | Baud and framing | Scope or logic analyser on TX during polling | 115200 8N1; one request about every 170 ms or more; frame starts with `0x00` | |
| C4 | Crossing and ground | Connect HMI TX → peer RX, HMI RX → peer TX, common GND | Link comes up (see D1/E1) | |
| C5 | Controller side pin match | Confirm controller J2: TX GPIO19 / RX GPIO21 | Matches the HMI wiring table | |

## D. Link with the PC controller simulator (no controller)

Setup: separate 3.3 V USB-UART adapter, real controller disconnected ([controller-wiring](../03-hardware/controller-wiring.md)). All simulator data is synthetic.

| ID | Test | Steps | Expected | Result / observed |
|---|---|---|---|---|
| D1 | Link up | `python tools/controller_sim.py --port COMx` | Header `ONLINE · <age>` within a few seconds; Device › Controller `COMPATIBLE` | |
| D2 | Counters sane | Device › Comm link, 5 minutes idle | `timeouts` and `bad frames` stay 0 on a clean link | |
| D3 | Polling cadence | Capture with `--verbose` or an analyser | Order PING, STATUS, MEASUREMENTS, STATUS, FAULTS, EVENTS, STATUS, MEASUREMENTS, DEVICE_INFO; starts ≥ 170 ms apart | |
| D4 | State display | Simulator: `ready`, `charging`, `complete`, `fault overcurrent`, `inhibit remote`, `reboot` | Hero, stage strip, Faults page and banner match [ui-pages-and-states §5](../05-ui/ui-pages-and-states.md) | |
| D5 | Measurement states | `measurement valid\|invalid\|stale\|floor\|missing`, `values 48200 8000` | `STALE DATA`/dashes when not valid; `<=` for floor; current signed; power only when both valid | |
| D6 | START flow | From IDLE: tap START, confirm, SEND | Sequence SENDING → ACCEPTED → COMPLETED feedback; hero shows the new state | |
| D7 | START failures | `control accepted-only`, `start-fail`; then START | Accepted-but-pending shown; failure reason named; no false "charging" | |
| D8 | STOP flow | During `charging`: tap STOP | "STOPPING…" then "Stopped by command / Relay commanded open" | |
| D9 | STOP edge cases | `control stop-unconfirmed`, `stop-fail`, **`stop-queue-full`** | `NOT CONFIRMED`; "Stop took effect, not tracked"; for queue-full expect the same "Stop took effect, not tracked / Send STOP again" (**G2**, changed 2026-11-27; if you see "STOP rejected / QUEUE_FULL" the image predates the change) | |
| D10 | Authority denied | `authority deny`, then START | Rejection shown, banner `HMI HAS NO CONTROL AUTHORITY` for 15 s, no-authority bar | |
| D11 | Link silence | `link silence` during `charging` | After > 2 s: `OFFLINE`, banner `LINK LOST · CHARGING MAY CONTINUE`; state `UNKNOWN`; on `link normal` recovers | |
| D12 | Wire faults | `wire bad-crc`, `wire drop-next`, `delay 200`, `delay 20000` | Bad-frame counter rises; retries (3 sends) then `NOT CONFIRMED` for commands; recovery after `wire normal` | |
| D13 | Controller restart | `reboot` while a START is pending | Pending command becomes NOT CONFIRMED; events cleared; data re-read | |
| D14 | Event log | `events 1000` | Session page shows `m rotated`; list scrolls; no crash or leak in the alive log | |
| D15 | Simulated-measurement flags | If the simulator supports flag bit3/bit4 (otherwise N/A) | `SIMULATED V/I` banner on all five pages; START only when ready | |
| D16 | Authority states render | Needs a simulator extension or a real controller | Device › Controller chip shows `THIS HMI` / `NONE` / `UART` correctly (the PC simulator scenarios never set `is_control`) | |

## E. Real controller: monitoring only (no load)

Preconditions: controller on the bench with the charge path inactive, relay output unloaded, controller procedures followed. Record the controller image and CONFIG.

| ID | Test | Steps | Expected | Result / observed |
|---|---|---|---|---|
| E1 | Online and compatible | Connect J2; power both | `ONLINE`; chip `COMPATIBLE` (product `0x0001`, protocol 1) | |
| E2 | Identity | Device › Controller | Revisions, image id (8 bytes of SHA-256), boot id match what the controller reports through its own tool | |
| E3 | Status and measurements at rest | Overview/Measure with no source | Plausible state; VOUT `<=` floor value (about 3.38 V reported on the 2026-10-06 bench) with status bit6; current near 0 | |
| E4 | Fault names vs the controller registry | Compare `hp_fault_names` / inhibit names with the controller's current `rcc_fault_registry.h` / ICD | Same ids and bit order (**CR-02**: bit positions are not frozen). Record the controller commit | |
| E5 | Authority display against CONFIG | For each of: seed (`NONE`), CONFIG naming this HMI (`operational_interface = 3`, node 0), CONFIG naming another interface | Controller chip shows `NONE` / `THIS HMI` / that interface. Record the CONFIG generation | |
| E6 | Older controller behaviour | Only if a pre-CR-01 image exists | Fields read 0; HMI does not mislabel as "denied" | |
| E7 | Event log | Provoke a state change on the controller | Event appears with relative time; SYNC refreshes | |
| E8 | Controller restart | Reset the controller | HMI shows loss, then recovers and clears cached events | |
| E9 | Cable pull | Disconnect and reconnect the UART | `OFFLINE` after > 2 s with the banner; recovery without reboot | |

## F. Real controller: command path (no load, controller procedures apply)

Run only when the controller owner confirms the setup is safe. The relay must not be connected to a load.

| ID | Test | Steps | Expected | Result / observed |
|---|---|---|---|---|
| F1 | START without authority | CONFIG not naming this HMI; tap START | `REJECTED/UNAUTHORIZED_SOURCE`; no-authority banner/bar | |
| F2 | START with authority, no charge request | CONFIG naming this HMI; tap START with no source | `REJECTED/CHARGER_REQUEST_ABSENT`; no session created | |
| F3 | START state limits | Try START in `INHIBITED` and from non-IDLE states the UI allows | Matches `hp_start_allowed`; record controller replies | |
| F4 | STOP at rest | Tap STOP | Relay commanded OFF; `COMPLETED` after `REMOTE_INHIBIT` is written; inhibit shown (REMOTE) | |
| F5 | Repeated STOP | STOP twice | Second answered at once, nothing rewritten | |
| F6 | Result timing | Measure time from tap to terminal result on a scope/log | Record latency; check against the HMI's 15 s wait and 50 ms × 3 retry window | |
| F7 | Same-request retry | Inject a dropped reply (analyser/gap) | Identical bytes and `request_id` on retries; no second action on the controller | |
| F8 | Controller reset during a command | Reset the controller while a command is pending | Pending command shown as NOT CONFIRMED after `boot_id` change | |
| F9 | `QUEUE_FULL` on STOP | If it can be provoked on the controller | "Stop took effect, not tracked / Send STOP again" shown; record whether the relay command really is OFF and `REMOTE_INHIBIT` is set | |

## G. Behaviour gaps to retest (from [operator-safety-behavior §9](../05-ui/operator-safety-behavior.md#9-known-gaps))

These are decisions as much as tests. Observe first, then decide.

| ID | Gap | What to observe on the board | Decision / observed |
|---|---|---|---|
| G1 | START shown without authority | With `is_control = false` is START offered? How quickly does the operator learn it will be refused? | |
| G2 | STOP + `QUEUE_FULL` wording (**changed in software, awaiting review and hardware check**) | As D9/F9 | |
| G3 | STOP unreachable behind modal/overlay (**deferred**; still observe it so the decision has data) | Open START dialog, Wi-Fi, OTA overlay: is STOP reachable? Does the link-loss banner update? How long does an overlay typically stay open? | |
| G4 | START hidden in `VREQ_VALIDATE`/`PRECHECK` without a reason | Is it confusing in practice? | |
| G5 | `PRODUCT BUILD` label | Read the banner on a provisional and a non-provisional controller | |
| G6 | No reconnect button / CRC counters | Is it missed during link troubleshooting? | |

## H. Settings, Wi-Fi and Web OTA

| ID | Test | Steps | Expected | Result / observed |
|---|---|---|---|---|
| H1 | Settings persistence | Change brightness, sleep timeout, language; reboot | Values survive (NVS `hmi_cfg`/`settings`). Note: save errors are not shown on screen | |
| H2 | Wi-Fi scan | Open Wi-Fi | Up to 12 networks ordered by signal | |
| H3 | Connect and reconnect | Connect; reboot; drop the AP and bring it back | Credentials saved; reconnect on boot and on loss | |
| H4 | Forget and failure paths | Forget; wrong password; no AP; no IP | Correct failure screens; no hang; header shows state | |
| H5 | OTA arming | Connected: ENABLE OTA | A 6-digit PIN appears on the HMI only and is not in the log | |
| H6 | OTA refusal | Upload without enabling; with a wrong PIN; five wrong PINs | 403; 401; window closes after the fifth | |
| H7 | OTA PIN lifetime | Wait for 600 s; also LOCK OTA; also drop Wi-Fi | Window closes in each case; new PIN needed | |
| H8 | OTA success | Upload `firmware/build/firmware.bin` with the PIN | Progress overlay, verification, reboot after about 2 s; new version on Device › Settings; `hmi_ota_mark_valid()` reached | |
| H9 | Invalid images | Upload a bootloader, merged or partition image; a truncated file | Rejected (first byte must be `0xE9`); the running image untouched | |
| H10 | Power loss during OTA | Cut power mid-write (on a bench unit) | Boots the previous slot; no brick | |
| H11 | Rollback | Boot an image that panics before `mark_valid` | Bootloader reverts to the previous slot | |
| H12 | OTA with the controller link up | Update while the HMI is polling a (simulated) controller | Record link behaviour during and after; remember the updater has no charging-state gate | |
| H13 | HTTP exposure | Capture traffic during an enabled window | Confirms the PIN is visible on plain HTTP (documented limitation) | |

## I. Robustness, memory and timing (never measured)

Several of these need extra instrumentation (for example LVGL memory monitor output, stack high-water marks per task); add it temporarily and note that the image differs from the release build.

| ID | Measurement | How | Record | Result |
|---|---|---|---|---|
| I1 | Free internal heap and PSRAM over time | Alive log every 5 s, long run | Minimum, trend (leak?) | |
| I2 | LVGL heap headroom with 128 KB | `lv_mem_monitor` after visiting every page, Wi-Fi overlay, OTA overlay, event list full | Peak used, fragmentation. Answers the open question in [ADR-0002](../09-decisions/0002-lvgl-pro-ui-source.md) | |
| I3 | Stack high-water marks | `rcc_link` (6144 B), LVGL task, `sys_evt`, HTTP server, main | Minimum free per task | |
| I4 | Refresh duration | Time `rebuild()` per page (log with timestamps) | Worst case; margin versus the 5 s TWDT | |
| I5 | Task watchdog | Deliberately stall the LVGL task in a debug build | Panic after about 5 s and reboot | |
| I6 | UART load | Controller at maximum event rate; `delay`/`wire` faults | No overrun; `bad frames` count; UI still responsive | |
| I7 | Long run | Run connected to the simulator for a duration the owner chooses | No reboot, no memory trend, no stuck state | |
| I8 | Power dips / brownout | Controlled supply dip on a bench unit | Defined reboot behaviour; settings intact | |
| I9 | Touch latency | Video of tap-to-response | Record the time; compare with expectation | |
| I10 | Temperature | Run in the enclosure | Display and Wi-Fi stable | |

## J. Documentation items to close after testing

| Item | Action |
|---|---|
| Connector pinout | Put it in [controller-wiring](../03-hardware/controller-wiring.md) |
| Measured stack/heap/latency | Add to [firmware-architecture §4](../02-architecture/firmware-architecture.md#4-tasks-locks-and-timing) |
| Boot log | Add a real sample to [build-and-flash](../07-development/build-and-flash.md) |
| Troubleshooting causes | Replace "no recorded cause" entries in [troubleshooting](../07-development/troubleshooting.md) with observed ones |
| Preview images | Replace `assets/ui/*.png` with real captures, or label them clearly |
| Mockup | Decide to update or retire `docs/mockups/` ([ADR-0002](../09-decisions/0002-lvgl-pro-ui-source.md) open question) |
| Status | Move each recorded result into [verification-status](verification-status.md) with date, revision and evidence |

## K. Blocked until the controller is ready

Not testable from the HMI side yet. Listed so nobody assumes they are covered.

| Item | Why blocked | Owner |
|---|---|---|
| A START that reaches `CHARGING` and `CHARGING_ESTABLISHED` from this HMI | The controller records it as not verified (needs its own B5); relay has never closed on a charging session | Controller project |
| Real VOUT/IOUT accuracy shown on the HMI | Calibration on the controller is a seed, not a measurement | Controller project |
| Contact position | The board has no contact feedback; HMI can only show the relay command | Hardware design |
| Relay closed with a load | Only under the controller project's procedures and qualified review | Controller project |
| Production authority and release gates | Human decision; 13 of 19 controller fault rows are still unbound per the controller notes | Project owner |

## Session log

| Date | Tester | Sections run | HMI commit | Peer | Notes and link to raw logs |
|---|---|---|---|---|---|
| | | | | | |
