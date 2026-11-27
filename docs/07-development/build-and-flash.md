# Build and flash

How to build the HMI firmware and put it on the board. Sources: `firmware/README.md`, `firmware/CMakeLists.txt`, `firmware/sdkconfig.defaults`, the former bring-up guide. The build itself was **not** re-run for this page ([verification-status](../08-status/verification-status.md)).

## Requirements

- **ESP-IDF 6.1.0** (the project is pinned to it). `idf.py` is the only build tool; PlatformIO is no longer used.
- An ESP-IDF shell: PowerShell/cmd from the ESP-IDF installation (EIM profile `C:\Espressif\tools\Microsoft.v6.1.PowerShell_profile.ps1`), or the dev container in `firmware/.devcontainer/` (pinned to v6.1).
- **Do not use Git Bash.** `idf.py` silently does nothing when `MSYSTEM` is set. Set `PYTHONUTF8=1` if the console is not UTF-8.

## Build

```
cd firmware
idf.py build
```

- First build downloads managed components into `firmware/managed_components/` (gitignored), pinned by `dependencies.lock`: lvgl 9.6.0~1, esp_lvgl_port 2.9.0, esp_lcd_st77922 2.1.1.
- `firmware/sdkconfig` is generated from `sdkconfig.defaults` and is not tracked. If you have an old generated copy, regenerate it or compare with `idf.py menuconfig`; LVGL settings (`CONFIG_LV_USE_OBJ_NAME=y`, `CONFIG_LV_MEM_SIZE=131072`) must match the defaults.
- On a busy workstation, `ninja -C build -j 4` after the first `idf.py build` limits parallel compile jobs.
- Project components compile with `-Wall -Wextra -Werror`. Generated LVGL layout sources emit deprecation warnings (see the status page).

## Flash and monitor over USB

```
idf.py -p COMx flash monitor
```

Replace `COMx` with the board's current port. Application logs come over **native USB Serial/JTAG**, which is also the flashing port. Close any other serial monitor on that port before flashing.

USB flashing writes the bootloader, partition table and application. It is required to establish the dual-OTA layout; Web OTA later transfers only an application image ([settings-wifi-ota](../06-services/settings-wifi-ota.md), partition table in [elecrow-s3-board](../03-hardware/elecrow-s3-board.md#flash-layout)). The older factory-only layout is obsolete.

## Configuration files

| File | Role |
|---|---|
| `sdkconfig.defaults` | Board settings: 16 MB QIO flash, 8 MB octal PSRAM, USB-Serial-JTAG console, rollback, LVGL options |
| `partitions.csv` | Dual OTA (`ota_0`/`ota_1` 3.5 MiB each), NVS, 8 MiB `storage` |
| `dependencies.lock` | Pinned managed-component versions |
| each component's `idf_component.yml` | Managed dependencies of that component |

## What to expect on first boot

The boot log should follow the start order in `firmware/main/app.c` (see [elecrow-s3-board](../03-hardware/elecrow-s3-board.md#start-up-sequence-and-failure-indications)): boot reason, `Flash: 16777216 bytes; PSRAM: 8388608 bytes`, then the "Five-page HMI ready" line, then periodic "Alive" lines with heap figures. This comparison has **not** been done on the current image.

## Related

- Tests and simulators: [firmware/README](../../firmware/README.md#checks), [testing-and-simulation](testing-and-simulation.md), [desktop UI simulator](../../firmware/tools/sim/README.md).
- Editing screens before building: [lvgl-pro-workflow](../05-ui/lvgl-pro-workflow.md).
