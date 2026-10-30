# Elecrow DLE06235B / ES3C35P firmware

The default `elecrow_s3` target includes the five-page HMI, ST77922 QSPI LCD,
integrated touch, RCC UART, NVS settings, Wi-Fi and Web OTA. Source bindings
below were checked against revision `28d2253` on 2026-10-30; physical
integration and feature acceptance remain separate checks.

## Hardware configuration

| Function | Current binding |
|---|---|
| Memory | 16 MB QIO flash at 80 MHz; 8 MB OPI PSRAM at 80 MHz |
| Console | Native USB Serial/JTAG for application logs and USB flashing |
| LCD | ST77922 QSPI: CS 10, CLK 12, D0 11, D1 13, D2 14, D3 9; 20 MHz |
| Resolution | Native 320 × 480, LVGL rotation to landscape 480 × 320 |
| Backlight | GPIO41, LEDC PWM at 5 kHz; settings-controlled brightness/dimming |
| Touch | ST77922 I²C at `0x55`, SDA 38, SCL 39, reset 48, interrupt 47; 100 kHz |
| Controller | UART1, TX GPIO43 / RX GPIO44; 115200 8N1, no flow control, 3.3 V logic |

The touch driver reads and acknowledges up to five contacts; LVGL uses the
first as its pointer. LCD initialization is based on the
[Elecrow product resources](https://www.elecrow.com/wiki/3.5_ESP32-S3_Display_with_320x480_Capacitive_IPS_Touch_Panel.html).

### Controller wiring

Confirm which physical Elecrow connector exposes GPIO43/GPIO44 before wiring.
The source assignment is not a verified connector pinout.

| HMI | Controller operational UART (J2) |
|---|---|
| GPIO43, TX | GPIO21, RX |
| GPIO44, RX | GPIO19, TX |
| GND | GND |

Use 3.3 V UART logic. For the PC simulator, replace the controller with a
separate USB-to-UART adapter: adapter TX to HMI RX, adapter RX to HMI TX and
common GND. Native USB is the console, not this link.

Device → COMM LINK shows the same binding (`UART1`, `GPIO43 / 44`) as
`rcc_link.c`.

## Build and USB upload

PlatformIO pins Espressif32 `7.1.3`; the documented baseline is ESP-IDF `6.1.0`
and LVGL `9.6.0`. From `firmware/`:

~~~powershell
pio run
pio run -t upload --upload-port COM15
pio device monitor --port COM15 --baud 115200
~~~

Use the actual board port in place of `COM15`. Close the monitor before upload.
Dependencies are pinned in `dependencies.lock`; defaults are in
`config/elecrow_s3.defaults` and partitions in `partitions/elecrow_s3.csv`.
USB flashing establishes the current bootloader and partition layout. The older
factory-only layout is obsolete; Web OTA transfers an application and does not
migrate an old partition table.

## Flash layout

| Partition | Offset | Size | Purpose |
|---|---|---|---|
| `nvs` | `0x9000` | `0x6000` | Settings and Wi-Fi credentials |
| `otadata` | `0xF000` | `0x2000` | OTA slot selection/state |
| `phy_init` | `0x11000` | `0x1000` | PHY initialization data |
| `ota_0` | `0x20000` | `0x380000` (3.5 MiB) | Application slot 0 |
| `ota_1` | `0x3A0000` | `0x380000` (3.5 MiB) | Application slot 1 |
| `storage` | `0x720000` | `0x800000` (8 MiB) | Reserved SPIFFS partition |

There is no factory app slot. Declaring `storage` does not establish that the
application mounts or uses it. Rollback is configured by
`CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE=y`. See
[settings, Wi-Fi and OTA](settings-wifi-and-ota.md) for the update lifecycle.

## Startup and failure indications

The app checks flash/PSRAM, configures a 5 s task watchdog, initializes NVS
settings and Wi-Fi, then starts the display/UI. Display initialization attempts
`rcc_link_start()` and passes its result to the UI. The LVGL refresh and RCC
link tasks subscribe to the watchdog.

- UART initialization failure: header `UART FAILED`, banner
  `UART INIT FAILED · NO CONTROLLER LINK`.
- UART started without recent replies: `OFFLINE` and
  `LINK LOST · CHARGING MAY CONTINUE`.
- UART initialization does not establish connectivity or control authority.
  The app's UART-started log is unconditional after display startup; use the
  UI/error logs and actual replies to diagnose the link.
- After display startup, `hmi_ota_mark_valid()` confirms a pending OTA image,
  before end-to-end controller verification.

## Verification status

Earlier bring-up reports LCD/touch operation and flashing of a five-page image.
Source inspection confirms the bindings above but does not verify the current
image on the board. Remaining checks include connector mapping, simulator/HMI
communication, controller identity/status, START/STOP authority, navigation,
settings persistence, Wi-Fi, OTA/rollback and long runs. No real charging
session or relay operation is claimed here.
