# Local settings, Wi-Fi and Web OTA

This describes HMI revision `28d2253`, checked on 2026-10-30. These features
are compiled into the Elecrow target; hardware acceptance is pending. They
configure the HMI, not controller charging limits, calibration or authority.

## Local display settings

Open Device → SETTINGS. `hmi_settings.c` stores a blob in NVS namespace
`hmi_cfg`, key `settings`.

| Setting | UI choices | Default / current behavior |
|---|---|---|
| Backlight | 25%, 50%, 75%, 100% | 100%; backend accepts 10–100% |
| Sleep timeout | OFF, 1 min, 3 min, 5 min | 3 min; OFF disables inactivity dimming |
| Idle dim level | Displayed read-only | 15%; backend setting exists |
| Language | English, Tiếng Việt | Selection is saved; main labels remain hard-coded English |

“Sleep” means backlight dimming, not ESP32 deep sleep or suspension of UART.
Touch restores brightness; the touch driver consumes the wake touch. Dimming
is suppressed while the UI has fresh OPERATIONAL state in stages
VREQ_VALIDATE through CHARGING. It does not explicitly suppress dimming for
every fault, lost-link or OTA state.

Settings callbacks do not present NVS save errors to the operator. A selected
value on screen alone is not proof it survived a restart.

## Wi-Fi station

Open Device → SETTINGS → Wi-Fi, or use the header Wi-Fi control. Screens
support scanning, network selection, password entry/show-hide, connection
progress, retry/cancel, network information, disconnect and forget. The backend
keeps up to 12 scan results ordered by signal strength.

Credentials from a successful connection are saved in NVS; boot attempts to
reconnect to the saved network. Link loss triggers reconnection while
auto-connect is enabled. Forget removes saved credentials. IP address,
gateway, MAC and RSSI are available on the Wi-Fi screens.

Wi-Fi provides HMI networking and Web OTA. RCC monitoring and START/STOP still
travel over UART1; Wi-Fi does not grant controller authority.

## Web OTA procedure

1. Establish the current dual-OTA partition layout using the USB workflow in
   [the bring-up guide](elecrow-s3-bringup.md).
2. Build the Elecrow application with `pio run` from `firmware/`.
3. Connect the HMI to Wi-Fi and read its IP address on the Wi-Fi screen.
4. From a computer on the same reachable network, open `http://<HMI-IP>/`.
5. Select `firmware/.pio/build/elecrow_s3/firmware.bin` and upload it. Use the
   application binary, not a bootloader, merged image or partition table.
   Keep power connected while writing and rebooting.

`hmi_wifi.c` starts the HTTP server after the station obtains an IP address.
It uses port 80: GET `/` and GET `/update` serve the upload page, GET
`/api/status` reports state, and POST `/update` receives the raw binary.

The updater writes the next app slot with the ESP-IDF OTA API, checks the image
magic and finalizes with `esp_ota_end()`, then selects the boot slot. Success
schedules a restart after about 2 s. An on-screen progress overlay is provided;
it is not a controller STOP operation.

Rollback is configured in the bootloader defaults. After display startup,
`hmi_ota_mark_valid()` confirms a pending image and cancels rollback. This
initialization milestone does not check UART connectivity, charging behavior
or every HMI feature. Rollback and power-loss acceptance tests remain pending.

## Current implementation limits

- HTTP upload handlers implement no login, TLS or application-level image
  signature verification. Format/integrity validation does not establish
  publisher identity. Anyone with network access to the service can attempt
  an upload; use a controlled bench network for this prototype.
- The updater has no charging-state gate and can reboot the HMI during a
  session. Losing the HMI link does not automatically stop charging. Run
  update acceptance checks with the charging path inactive.
- Version strings in settings/OTA screens are hard-coded labels, not verified
  source revisions or image identities.
- Failure-state overlay recovery, network interruption, repeated uploads and
  recovery from a failed image need hardware acceptance checks.

Sources: `firmware/src/hmi_settings.c`, `hmi_backlight.c`, `hmi_wifi.c`,
`hmi_ota.c`, `hmi_ui.c`, `bringup/elecrow_s3_main.c`,
`firmware/config/elecrow_s3.defaults` and `firmware/partitions/elecrow_s3.csv`.
