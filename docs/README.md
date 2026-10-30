# HMI documentation

The default Elecrow DLE06235B / ES3C35P ESP32-S3 target includes the five-page
LVGL UI, LCD/touch, RCC UART1 transport, NVS settings, Wi-Fi station and Web OTA.
This documentation was reconciled with HMI source revision `28d2253` on
2026-10-30. Implementation statements describe source and configuration;
they do not establish hardware acceptance.

## Reading order

| Document | Content |
|---|---|
| [elecrow-s3-bringup.md](elecrow-s3-bringup.md) | Build, pin bindings, flash layout and hardware verification status |
| [controller-interface.md](controller-interface.md) | Controller protocol, commands and authority constraints |
| [ui-pages-and-information.md](ui-pages-and-information.md) | Implemented pages, command handling and known UI differences |
| [settings-wifi-and-ota.md](settings-wifi-and-ota.md) | Local settings, networking, update procedure and limits |
| [controller-simulator.md](controller-simulator.md) | PC simulator through an external USB-to-UART adapter |
| [controller-change-requests.md](controller-change-requests.md) | Open dependencies owned by the controller repository |
| [mockups/](mockups/) | Visual references; these can differ from the current UI |

## Implementation and verification

- The target compiles `src/rcc_link.c` and `src/rcc_codec.c`. Display startup
  calls `rcc_link_start()`; UART1 is bound to TX GPIO43 / RX GPIO44 at
  115200 8N1. The former offline stub backend has been removed.
- Earlier bring-up records report LCD/touch operation and flashing of a
  five-page image. They do not verify all features of the current source.
  Current visual and navigation acceptance remains pending.
- Settings use NVS. Wi-Fi starts in station mode and starts HTTP OTA after
  obtaining an IP address. There are two OTA app slots; rollback is enabled
  in the defaults.
- On 2026-10-30 the Python simulator suite discovered 33 tests: 31 passed,
  2 skipped. See the simulator guide for commands and dependencies.
- End-to-end controller communication, charging, relay operation, Wi-Fi/OTA
  acceptance, rollback injection and long-duration operation remain unverified
  by this documentation update. A current target build was not rerun for this
  documentation-only change.
