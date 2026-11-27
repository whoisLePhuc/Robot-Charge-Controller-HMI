# Getting started

From a fresh clone to something running, in the order that needs the least equipment. Each step says what it needs, what you should see, and whether it was actually run when this page was written (2026-11-27, commit `f96b6bb`, WSL2 Linux).

Read [project-overview](project-overview.md) first. This is a development prototype; nothing here operates charging hardware.

## Path at a glance

| Step | Needs | Equipment | Run when written? |
|---|---|---|---|
| 1. Get the code and read the map | git | none | n/a |
| 2. Host tests | C compiler, Python | none | **Yes** |
| 3. See the UI on your PC | `make`, `gcc`, Pillow, LVGL sources | none | **Yes** |
| 4. Build the firmware | ESP-IDF 6.1.0 | none | **No** (`idf.py` not installed there) |
| 5. Flash the board | USB cable | Elecrow board | No |
| 6. Talk to a simulated controller | 3.3 V USB-UART adapter | board + adapter | No |
| 7. Talk to the real controller | controller board | board + controller | No; read the safety page first |

## 1. Get the code and orient

```sh
git clone <repository-url> Robot-Charge-Controller-HMI
cd Robot-Charge-Controller-HMI
```

- Where things are: [repository-map](../02-architecture/repository-map.md).
- Terms: [glossary](glossary.md).
- The controller is a separate repository. Its firmware documents (`docs/firmware/`) are normative for the protocol. Keep it as a sibling directory if you want the relative links in these docs to resolve (`../Robot_Charge_Controller/`).

## 2. Run the host tests (no hardware)

```sh
cd firmware
CC=gcc sh test/host/run.sh
python3 -B -m unittest discover -s test/host -p test_controller_sim.py -v
```

Expect `ALL PASSED`, `test_hmi_present: 308 checks, 0 failed`, `rcc_link host tests passed`, then 33 tests `OK` (one skipped unless you set `RCC_REFERENCE_CODEC_PATH`). On Windows use `test/host/run.ps1` with MinGW `gcc` on PATH. Details: [testing-and-simulation](../07-development/testing-and-simulation.md).

## 3. See the UI on your PC (no hardware, no ESP-IDF)

The desktop simulator needs LVGL sources in `firmware/managed_components/lvgl__lvgl`. That folder is created by the ESP-IDF component manager during `idf.py build` / `idf.py reconfigure`. If you have not installed ESP-IDF yet, copy or check out LVGL 9.6.0 to that path.

```sh
cd firmware/tools/sim
pip install pillow
make -j"$(nproc)"                  # about 1.5 minutes the first time
python3 render.py out --scale 1    # 80 PNGs in ./out
```

Open `out/CHARGING-overview.png` (and others). The images use scenario data from `sim_scenarios.c`, not a real controller.

## 4. Build the firmware

Install **ESP-IDF 6.1.0** and open an ESP-IDF shell (PowerShell/cmd, or the dev container in `firmware/.devcontainer/`). Do not use Git Bash (`idf.py` silently does nothing there).

```
cd firmware
idf.py build
```

Details, configuration files and expected warnings: [build-and-flash](../07-development/build-and-flash.md).

## 5. Flash the board

Connect the Elecrow board by native USB.

```
idf.py -p COMx flash monitor
```

Compare the boot log with the expected sequence in [elecrow-s3-board §start-up](../03-hardware/elecrow-s3-board.md#start-up-sequence-and-failure-indications). The restructured firmware has **never been flashed** according to the repository's records, so treat your first boot as the first recorded evidence and write down what you see.

## 6. Connect a simulated controller

Wire a separate 3.3 V USB-to-UART adapter as in [controller-wiring](../03-hardware/controller-wiring.md), then:

```sh
cd firmware
python -m pip install -r tools/controller_sim_requirements.txt
python tools/controller_sim.py --port COMx
```

The header pill should turn `ONLINE`. Try `charging`, `fault overcurrent`, `authority deny` at the simulator prompt. If it stays `OFFLINE`, see [troubleshooting](../07-development/troubleshooting.md).

## 7. Connect the real controller

Only after steps 5 and 6 work, and only with the controller project's own procedures in hand:

1. Confirm the Elecrow connector that carries GPIO43/GPIO44 ([controller-wiring](../03-hardware/controller-wiring.md)).
2. Make sure the controller's CONFIG names the operational UART, node 0, if you expect START/STOP to be accepted ([commands-and-payloads §1](../04-controller-protocol/commands-and-payloads.md#1-conditions-for-hmi-control)).
3. Start with monitoring only. Read [operator-safety-behavior](../05-ui/operator-safety-behavior.md), including the known gaps.
4. Do **not** treat a successful link as permission to charge. A START that reaches `CHARGING` has never been verified from this HMI.

## Where to go next

| Goal | Page |
|---|---|
| Change how a screen looks | [lvgl-pro-workflow](../05-ui/lvgl-pro-workflow.md) |
| Add a field, page or module | [how-to-recipes](../07-development/how-to-recipes.md) |
| Understand a screen | [ui-pages-and-states](../05-ui/ui-pages-and-states.md) |
| Something broke | [troubleshooting](../07-development/troubleshooting.md) |
