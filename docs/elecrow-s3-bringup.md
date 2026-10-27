# Elecrow DLE06235B / ES3C35P firmware

The repository now builds for the Elecrow ESP32-S3 display by default. The
firmware includes the five-page HMI, ST77922 QSPI LCD driver and integrated
capacitive touch. The controller link remains offline until its UART pins and
electrical connection are confirmed.

## Hardware configuration

- ESP32-S3 at 240 MHz, 16 MB QIO flash at 80 MHz and 8 MB OPI PSRAM at 80 MHz.
- Native USB Serial/JTAG is the console; it is separate from the controller
  transport.
- LCD uses ST77922 QSPI: CS 10, CLK 12, D0 11, D1 13, D2 14, D3 9;
  backlight GPIO 41. Native resolution is 320 × 480 and LVGL rotates to
  480 × 320 landscape.
- Touch is integrated into the ST77922 at I²C address `0x55`, SDA 38, SCL 39,
  reset 48 and interrupt 47. The driver reads and acknowledges all five
  supported touch points; LVGL uses the first contact as its pointer.
- Factory application partition is 3 MB. OTA and data partitions are not
  configured.

The register initialization table is based on the official
[Elecrow product wiki and resource pack](https://www.elecrow.com/wiki/3.5_ESP32-S3_Display_with_320x480_Capacitive_IPS_Touch_Panel.html).

## Build and upload

From `firmware/`:

~~~powershell
pio run
pio run -t upload --upload-port COM15
pio device monitor --port COM15 --baud 115200
~~~

Change `COM15` if Windows assigns a different port. Close the serial monitor
before uploading. Build dependencies are pinned in `dependencies.lock`; target
defaults are in `config/elecrow_s3.defaults` and the partition table is
`partitions/elecrow_s3.csv`.

The LCD and touch were visually exercised on the physical board. The five-page
HMI image builds and has been flashed; visual layout/navigation acceptance is
pending. The current UI shows `CONTROLLER LINK DISABLED`. The offline link
backend rejects START and STOP, and no controller commands are transmitted.

## Remaining integration

Confirm the Elecrow UART pins and wiring before replacing the offline link
backend with the RCC protocol transport. Keep USB Serial/JTAG for logging and
flashing. Then verify the simulator, controller identity/status, command
authorization and touch navigation. Display/touch checks do not verify a real
controller, charging session or relay operation.
