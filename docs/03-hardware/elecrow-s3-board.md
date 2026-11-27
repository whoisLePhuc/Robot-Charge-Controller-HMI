# Elecrow DLE06235B / ES3C35P board

What the HMI board provides and how the firmware binds it. For wiring to the controller see [controller-wiring.md](controller-wiring.md); for build and flashing see [build-and-flash.md](../07-development/build-and-flash.md).

Source bindings below were carried over from the previous bring-up guide (checked against revision `28d2253`, 2026-10-30) and spot-checked on 2026-11-27 against `firmware/components/common/hmi_common/include/hmi_config.h`, `firmware/partitions.csv` and `firmware/sdkconfig.defaults`. The pin numbers in `bsp_pins.h` were **not** re-compared. A binding in source is **not** a verified connector pinout, and nothing here verifies the board running the current image ([verification-status](../08-status/verification-status.md)).

## Hardware configuration

| Function | Current binding |
|---|---|
| Memory | 16 MB QIO flash at 80 MHz; 8 MB OPI PSRAM at 80 MHz |
| Console | Native USB Serial/JTAG for application logs and USB flashing |
| LCD | ST77922 QSPI: CS 10, CLK 12, D0 11, D1 13, D2 14, D3 9; 20 MHz |
| Resolution | Native 320 × 480, LVGL rotation to landscape 480 × 320 |
| Backlight | GPIO41, LEDC PWM at 5 kHz; settings-controlled brightness/dimming |
| Touch | ST77922 I²C at `0x55`, SDA 38, SCL 39, reset 48, interrupt 47; 100 kHz |
| Controller link | UART1, TX GPIO43 / RX GPIO44; 115200 8N1, no flow control, 3.3 V logic |

The touch driver reads and acknowledges up to five contacts; LVGL uses the first as its pointer. LCD initialisation is based on the [Elecrow product resources](https://www.elecrow.com/wiki/3.5_ESP32-S3_Display_with_320x480_Capacitive_IPS_Touch_Panel.html).

Where it lives in code:

| What | File |
|---|---|
| LCD, touch, backlight pins | `firmware/components/bsp/bsp_elecrow_s3/src/bsp_pins.h` |
| Controller UART port, pins, baud; watchdog timeout | `firmware/components/common/hmi_common/include/hmi_config.h` |
| Flash/PSRAM mode, rollback, LVGL memory | `firmware/sdkconfig.defaults` |
| Partition layout | `firmware/partitions.csv` |

## Flash layout

| Partition | Offset | Size | Purpose |
|---|---|---|---|
| `nvs` | `0x9000` | `0x6000` | Settings and Wi-Fi credentials |
| `otadata` | `0xF000` | `0x2000` | OTA slot selection/state |
| `phy_init` | `0x11000` | `0x1000` | PHY initialisation data |
| `ota_0` | `0x20000` | `0x380000` (3.5 MiB) | Application slot 0 |
| `ota_1` | `0x3A0000` | `0x380000` (3.5 MiB) | Application slot 1 |
| `storage` | `0x720000` | `0x800000` (8 MiB) | Reserved SPIFFS partition |

There is no factory app slot. Declaring `storage` does not establish that the application mounts or uses it. Rollback is enabled by `CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE=y`. USB flashing establishes the bootloader and partition table; Web OTA transfers only an application image and does not migrate an old table (see [settings-wifi-ota](../06-services/settings-wifi-ota.md)).

## Start-up sequence and failure indications

`firmware/main/app.c` (`app_run`) starts the system in this order. This **replaces** the older description in which display initialisation started the link; the link and UI are now started by `app.c`.

1. **Boot check:** flash/PSRAM configuration; stop before building the UI on a mismatch.
2. **Task watchdog:** `HMI_TWDT_TIMEOUT_MS` = 5000 ms.
3. **Services:** `hmi_settings_init()` (NVS), `hmi_ota_init()`, default event loop, handler for `HMI_WIFI_EVENT`, then `hmi_wifi_init()`. A Wi-Fi failure is logged and start-up continues.
4. **Board and UI:** `bsp_display_start()`, then under the LVGL lock `rcc_link_start()`, `hmi_ui_start(link_ok)`, `bsp_touch_start(display, hmi_power_on_touch)`; then `hmi_power_start()`.
5. **Confirm image:** `hmi_ota_mark_valid()` cancels rollback.
6. **Alive loop:** logs free internal heap, free PSRAM and stack minimum periodically.

Failure indications:

- UART initialisation failure: header `UART FAILED`, banner `UART INIT FAILED · NO CONTROLLER LINK`. The UI still runs.
- UART started but no recent replies: `OFFLINE` and `LINK LOST · CHARGING MAY CONTINUE`.
- UART initialisation does not establish connectivity or control authority. The "controller link started" log line is unconditional after display start-up; diagnose with the UI, error logs and actual replies.
- `hmi_ota_mark_valid()` is reached after display start-up, **before** any controller communication has been verified.
- Touch initialisation failure is logged; the UI keeps running.
