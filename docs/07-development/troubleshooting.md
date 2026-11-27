# Troubleshooting

Symptoms, likely causes and what to check. Every cause below is derived from the **source code or existing project documents**, and the source is named. Nothing was reproduced on a board (the current image has never been flashed), so treat each row as a place to start looking, not a diagnosis. Where the repository has no recorded cause, the entry says so.

Written 2026-11-27 for commit `f96b6bb`. If you find a real cause on hardware, add it here with the date, image and evidence.

## 1. Build and tooling

| Symptom | Likely cause | Check / fix | Source |
|---|---|---|---|
| `idf.py` prints a warning and does nothing | Running from Git Bash (`MSYSTEM` is set) | Use the ESP-IDF PowerShell/cmd shell or the dev container | `firmware/README.md` |
| Garbled output or Unicode errors on Windows | Console not UTF-8 | Set `PYTHONUTF8=1` | `firmware/README.md` |
| Flash fails, port busy | Another monitor owns the port | Close it; use the board's native USB port | `build-and-flash.md` |
| Device asserts when a page opens (widget not found) | A named widget was renamed or removed in the LVGL Pro layout | `grep -rn '"name"' firmware/components/ui`; restore the name or change both sides | `hmi_data_widget()` asserts; [lvgl-pro-workflow](../05-ui/lvgl-pro-workflow.md) |
| Name lookups fail or LVGL runs out of memory after pulling | Old generated `firmware/sdkconfig` without `CONFIG_LV_USE_OBJ_NAME=y` / `CONFIG_LV_MEM_SIZE=131072` | Regenerate `sdkconfig` from `sdkconfig.defaults` (or set both via `idf.py menuconfig`) | `lvgl_ui/HMI.md`, `sdkconfig.defaults` |
| New `page_*.c` or layout `_gen.c` is not compiled | `file(GLOB)` is not re-scanned | Touch `ui_app/CMakeLists.txt`, rebuild (*inferred* from the glob comment) | `ui_app/CMakeLists.txt` |
| Generated layout files overwritten with different content | Hand-edit of `*_gen.c`, or an Editor version change | Edit the XML; commit XML and `_gen` together | [lvgl-pro-workflow](../05-ui/lvgl-pro-workflow.md) |
| Desktop simulator build fails: LVGL not found | `firmware/managed_components/lvgl__lvgl` is missing | Run `idf.py reconfigure` once, or provide LVGL 9.6.0 there | `tools/sim/Makefile` |
| `check.sh`: "no baseline" or "Pillow required" | Baseline not recorded (`/tmp` is volatile), or no Pillow | `sh tools/sim/check.sh baseline` on the original commit; `pip install pillow` | `check.sh` |
| `check.sh` shows PIXEL DIFF after an Editor change | Expected: layout changed | Inspect the images, then re-record the baseline deliberately | [testing-and-simulation](testing-and-simulation.md) |
| Host tests: "cc: not found" | No C compiler | Install gcc/MinGW; `CC=gcc sh test/host/run.sh` | `run.sh` |
| Simulator tests skip a test | pyserial or the reference codec is absent | `pip install -r tools/controller_sim_requirements.txt`; set `RCC_REFERENCE_CODEC_PATH` | `testing-and-simulation.md` |

## 2. Boot

| Symptom | Likely cause | Check / fix | Source |
|---|---|---|---|
| Log: `Memory configuration does not match the expected 16 MB flash / 8 MB PSRAM`; no UI | `boot_check()` returned false, so `app_run()` stops before building the UI | Check the module's flash/PSRAM size and `CONFIG_SPIRAM*` in `sdkconfig.defaults` | `main/app.c: boot_check` |
| Reboot loop with a task-watchdog panic after about 5 s | The LVGL task did not feed the TWDT (stalled refresh), or `rcc_link` stalled. TWDT is 5000 ms with panic | Read the panic backtrace; look for long blocking calls under the LVGL lock | `main/app.c`, `hmi_shell.c`, `rcc_link.c` |
| Log: `Wi-Fi unavailable; the Wi-Fi screens will say so` | `hmi_wifi_init()` failed; start-up continues | Wi-Fi screens show "Unavailable"; OTA will not start | `main/app.c` |
| Log: `rcc_link_start failed: ...` and header `UART FAILED` | UART driver, pin or task creation failed | See the error name; check pins in `hmi_config.h` and that nothing else owns UART1 | `main/app.c`, `rcc_link.c: rcc_link_start` |
| Log: `Touch init failed` | Touch driver start failed; UI still runs | Check touch I²C pins and reset/interrupt lines in `bsp_pins.h` | `main/app.c` |
| Blank or white screen, no log line about it | **No recorded cause.** The restructured image has never been run on a board | Capture the full boot log; compare with the sequence in [elecrow-s3-board](../03-hardware/elecrow-s3-board.md#start-up-sequence-and-failure-indications) | |

## 3. Controller link

| Symptom | Likely cause | Check / fix | Source |
|---|---|---|---|
| Header `OFFLINE`, banner `LINK LOST` from the start | No valid reply within 2 s: wiring, wrong UART on the controller, peer not running, wrong baud | TX↔RX crossed, common GND, 3.3 V logic; controller on its **operational** UART (J2); 115200 8N1; simulator running. Open Device › Comm link | [controller-wiring](../03-hardware/controller-wiring.md), `rcc_link.c` |
| Pill `ONLINE` then drops, `timeouts` counter rising | Controller slow, retries exhausted (3 sends, 50 ms each), noisy line | Device › Comm link counters; shorten cable; try the simulator with `delay` and `wire drop-next` | `rcc_link.c: transaction_tick` |
| `bad frames` counter rising | CRC/COBS errors, noise, partial frames | Check ground, cable, baud; simulate with `wire bad-crc` | `rcc_codec.c`, `controller_sim.py` |
| Online but values are dashes and `STALE DATA` | Measurement status bits not all set (`& 0x3F`), or age > 1500 ms | Measure › ADC matrix; controller state; `measurement` commands in the simulator | `hmi_present.c` (`hp_v_valid`) |
| Voltage shows `<=3.4` | VOUT at the measurement floor (status bit6) with no charger | Expected with no source connected (floor about 3.38 V on the 2026-10-06 bench) | [commands-and-payloads §3.3](../04-controller-protocol/commands-and-payloads.md#33-get_measurements-32-bytes-schema-1) |
| Device chip `INCOMPAT.` | `product_id != 0x0001` or `protocol_version != 1` | Check the controller image and protocol version | `hp_fw_compat` |
| Device › Controller shows `UNREAD` | No valid GET_DEVICE_INFO (offline, or wrong length) | Wait a polling cycle (device info is read once per 9 steps) | `rcc_link.c` |
| Everything shows unknown for a moment after a controller reboot | `boot_id` changed; the HMI cleared cached validity and events | Expected; pending START/STOP become `NOT CONFIRMED` | `rcc_link.c` |

## 4. START and STOP

| Symptom | Likely cause | Check / fix | Source |
|---|---|---|---|
| No START button on Overview | `hp_start_allowed` false: status not fresh (< 1500 ms), simulated V/I not ready, a command SENDING/ACCEPTED, or state not `INHIBITED` / `OPERATIONAL/IDLE` (not offered in `VREQ_VALIDATE` or `PRECHECK`) | Look at the hero state and header; `Start blocked: …` text shows only for a stale link or a latched fault | `hmi_present.c` |
| START dialog opens but nothing is sent | `rcc_link_request_start()` re-checks at send time and refused (status aged out, command already active) | Re-open after status is fresh | `rcc_link.c` |
| `START rejected` / `UNAUTHORIZED_SOURCE`, banner `HMI HAS NO CONTROL AUTHORITY` | The controller's CONFIG does not name this HMI as the active control interface | Device › Controller shows the interface that does; fix the controller CONFIG (service UART). In the simulator: `authority allow` | [operator-safety-behavior §5](../05-ui/operator-safety-behavior.md#5-authority) |
| `START rejected` / `CHARGER_REQUEST_ABSENT` | No charge request: VOUT below the request threshold with relay open | Expected without a charger; no session is created | [commands-and-payloads §4.2](../04-controller-protocol/commands-and-payloads.md#42-start_charge) |
| `START rejected` / `INVALID_STATE`, `INHIBITED`, `FAULT_ACTIVE` | Controller state forbids START | Read Faults page; clear the cause on the controller side | ICD §11.2 |
| `NOT CONFIRMED` after START or STOP | No terminal result within the HMI limits (50 ms ×3 before ACCEPTED; 15 s after) | The outcome is **unknown**. Check controller state on Overview and the event log before sending again | `rcc_link.c` |
| "Stop took effect, not tracked" / "Send STOP again" | Controller STOP ledger full (`REJECTED/QUEUE_FULL`) or STOP `FAILED` (for example persistence failed) | Per ICD the STOP **still took effect** (relay commanded OFF). Send STOP again, then verify the relay command and state on Overview. Before the G2 change the QUEUE_FULL case read "STOP rejected" | [operator-safety-behavior §9](../05-ui/operator-safety-behavior.md#9-known-gaps) |
| State `NOT_CONFIRMED` / `CHARGING NOT ESTABLISHED` | Controller reports `CHARGING` but `charging_established` is clear | Check measurements and controller logs | `hp_state_info` |

## 5. Display and input

| Symptom | Likely cause | Check / fix | Source |
|---|---|---|---|
| Screen content not updating | The 1 s refresh is paused while a finger is down, the START dialog is open, or a Wi-Fi overlay is open | Release, cancel the dialog or close the overlay | `hmi_shell.c: refresh` |
| STOP cannot be reached | START dialog or an overlay covers it (gap G3) | Cancel or close the overlay first | [operator-safety-behavior §7](../05-ui/operator-safety-behavior.md#7-behaviour-that-can-surprise-an-operator) |
| Screen dims | Sleep timeout (default 180 s), dim level 15 % | Device › Settings; dimming is suppressed only during a session. The first touch only wakes the screen | `hmi_power.c`, `hmi_settings.h` |
| Language choice has no effect | Stored but labels are English literals | Known limitation | [ui-pages-and-states](../05-ui/ui-pages-and-states.md) |
| A setting is gone after reboot | NVS save errors are not shown to the operator | Check logs; NVS namespace `hmi_cfg`, key `settings` | [settings-wifi-ota](../06-services/settings-wifi-ota.md) |
| Fault named wrongly | Controller moved a bit position (`not-frozen`, CR-02) | Compare `hp_fault_names` with the controller registry | [how-to-recipes §6](how-to-recipes.md#6-add-or-rename-a-fault-inhibit-or-reason-name) |

## 6. Wi-Fi and OTA

| Symptom | Likely cause | Check / fix | Source |
|---|---|---|---|
| No OTA option | Wi-Fi not connected; server starts only when the station has an IP | Connect Wi-Fi; check the IP on the Wi-Fi screen | `main/app.c: on_wifi_link` |
| Upload answers **403** | OTA not enabled on the touchscreen | Tap **ENABLE OTA** | `hmi_ota.c` |
| Upload answers **401** | Wrong or missing `X-OTA-PIN` | Re-read the PIN on the HMI | `hmi_ota.c` |
| PIN stopped working | Single-use; expires after 600 s; closes after 5 wrong PINs, **LOCK OTA**, or Wi-Fi loss | Enable OTA again for a new PIN | `hmi_ota.h` |
| Upload refused as invalid image | First byte is not `0xE9` (not an application image) | Upload `firmware/build/firmware.bin`, not a bootloader, merged or partition image | `hmi_ota.c` |
| HMI rebooted during an update | Expected after success (about 2 s); the updater has no charging-state gate | Do updates with the charging path inactive | [settings-wifi-ota](../06-services/settings-wifi-ota.md) |
| Rollback behaviour unclear | `hmi_ota_mark_valid()` runs after UI start-up, before link health is known | Rollback and power-loss tests are pending | `main/app.c` |

## 7. Reporting a new problem

Record: date, HMI commit and `git describe` (shown on Device › Settings), ESP-IDF version, the full boot log, controller or simulator identity, the screen shown (Device › Comm link counters help), and what you expected. Add the finding here and, if it changes what is verified, to [verification-status](../08-status/verification-status.md).
