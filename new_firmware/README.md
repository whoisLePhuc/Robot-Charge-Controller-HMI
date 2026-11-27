# HMI firmware (ESP-IDF)

Firmware for the Elecrow DLE06235B / ES3C35P ESP32-S3 display (480x320 LVGL HMI for the Robot Charge Controller).
Built with **ESP-IDF 6.1.0** and `idf.py`; PlatformIO is not used.

Status: phases 1 and 2 of `../docs/firmware-restructure-design.md` are done; not yet run on a board. Behaviour
and UI are those of `../firmware/` (PlatformIO), which this project replaces.

## Layout

```
main/          main.c -> app.c: the composition root (start order, wiring between modules)
components/    common / bsp / protocol / services / ui, one ESP-IDF component per module
test/host/     host unit tests (rcc_codec, hmi_present, controller_sim.py)
tools/         sim/ (PC simulator, pixel + morph regression), controller_sim.py, gen_icons.py
```

Layers, dependency rules and "where do I change X": `components/README.md`. UI internals: `components/ui/README.md`.

## Build, flash, monitor

From an ESP-IDF 6.1 PowerShell/cmd (EIM profile `C:\Espressif\tools\Microsoft.v6.1.PowerShell_profile.ps1`):

```
idf.py build
idf.py -p COMx flash monitor
```

`idf.py` does nothing when `MSYSTEM` is set (Git Bash). Set `PYTHONUTF8=1` if the console is not UTF-8.

Configuration:

| File | Role |
|---|---|
| `sdkconfig.defaults` | Board settings (16 MB QIO flash, 8 MB octal PSRAM, USB-Serial-JTAG console, rollback). `sdkconfig` is generated from it and is not tracked |
| `partitions.csv` | Dual OTA (`ota_0`/`ota_1` 3.5 MB each), NVS, 8 MB `storage` |
| `dependencies.lock` | Pins lvgl 9.6.0~1, esp_lvgl_port 2.9.0, esp_lcd_st77922 2.1.1 (each component's `idf_component.yml`) |

## Checks

```
CC=gcc sh test/host/run.sh        # host unit tests: rcc_codec, hmi_present
sh tools/sim/check.sh baseline    # once, on the reference code
sh tools/sim/check.sh             # PC simulator: 80 images pixel diff + retained-mode morph check
python -m unittest discover -s test/host -p test_controller_sim.py -v
```

The simulator compiles LVGL from `managed_components/`, so run `idf.py build` (or `idf.py reconfigure`) once first.
UI layering rules: `components/ui/README.md`.
