# Glossary

Terms used across the HMI documentation. Where a term is defined by the controller repository, that definition is normative and this page only summarises it.

## System and protocol

| Term | Meaning |
|---|---|
| **RCC** | Robot Charge Controller: the separate device (Control Board + Relay Board) that switches and supervises the 60 V charge path. The HMI is its operator panel. |
| **Controller** | Short for the RCC firmware/hardware the HMI talks to. Not part of this repository. |
| **ICD** | Firmware Interface Control Document in the controller repository (`docs/firmware/interfaces/firmware-interface-control-document.md`). Normative for the protocol. |
| **FDD** | Firmware Detailed Design documents (FDD-000 to FDD-011) in the controller repository. Cited for fault, inhibit and state definitions. |
| **Operational UART** | The controller UART the HMI uses (HMI UART1, 115200 8N1, 3.3 V). Distinct from the **service UART**, CAN and RS485. |
| **Service UART** | A separate controller UART for configuration and service tools. The HMI does not use it. |
| **Active control interface** | The one interface the controller currently accepts START/STOP from. Others may read status only. |
| **Authority** | Permission to START/STOP. Reading status does not imply it. The HMI shows a "no authority" state when it is rejected. |
| **Authorization act** | Committing a controller CONFIG that names the peer (`operational_interface`, `operational_node`) over the service UART. This replaced the retired "operational node authorized" build flag (GET_DEVICE_INFO bit 2, now always 0). |
| **COMMAND_RESULT** | The controller's reply container. Every reply starts with a 12-byte header, then a command-specific payload. |
| **GET_STATUS / GET_MEASUREMENTS / GET_FAULTS / GET_DEVICE_INFO / GET_EVENT_LOG** | The read commands the HMI polls. |
| **START_CHARGE / STOP_CHARGE** | The two control commands the HMI can send. |
| **ACCEPTED** | The controller took the command. It does **not** mean charging has started. |
| **Unconfirmed** | The HMI timed out without a result. The outcome is unknown, not failed. |
| **Inhibit mask** | Bits explaining why charging is not allowed right now. Bit positions are *not frozen* by the ICD (see [CR-02](../04-controller-protocol/controller-change-requests.md)). |
| **Fault mask** | Bits for active faults. Fault IDs are confirmed by FDD-004; bit positions are *not frozen*. |
| **Confidence labels** | `confirmed` (in ICD or firmware source), `inferred` (derived, not measured), `not-frozen` (in source but may change). Used in the protocol pages. |
| **CR-nn** | A change request the HMI needs from the controller repository. See [controller-change-requests.md](../04-controller-protocol/controller-change-requests.md). |

## Firmware (ESP-IDF)

| Term | Meaning |
|---|---|
| **Component** | An ESP-IDF component under `firmware/components/<layer>/<module>/` with a public `include/` and private `src/`. |
| **Layer** | `common`, `bsp`, `protocol`, `services` or `ui`. Dependencies point downward only; `REQUIRES`/`PRIV_REQUIRES` enforce this at build time. |
| **Composition root** | `firmware/main/app.c`. The only code that sees every module; it starts them in order and connects them. |
| **BSP** | Board support package: `bsp_elecrow_s3` (LCD, touch, LVGL port, backlight). |
| **`rcc_codec`** | Pure-C frame codec for the controller protocol. Host-tested. |
| **`rcc_link`** | UART1 task that polls the controller, keeps the snapshot and sends START/STOP. |
| **Snapshot (`rcc_snapshot_t`)** | The single struct of controller data the UI may see. Defined in `hmi_common` (`rcc_types.h`). The shell takes one per refresh. |
| **`hmi_present`** | Pure-C logic that turns a snapshot into text, tones and views. No LVGL; host-tested (`test_hmi_present.c`). |
| **Tone** | A semantic colour class (for example ok/warn/fault) returned by `hmi_present` and mapped to a colour by the theme. |
| **`hmi_settings`** | NVS-backed local settings (brightness, dim level, sleep timeout, language). Wi-Fi credentials are stored in NVS by `hmi_wifi` itself. |
| **`hmi_power`** | Brightness, dim/sleep and wake-on-touch policy, over the BSP backlight. |
| **`HMI_WIFI_EVENT_*`** | `esp_event`s posted by `hmi_wifi`; `app.c` uses them to start/stop the OTA server. Services never call each other. |
| **Web OTA** | HTTP firmware update served by the HMI once Wi-Fi has an IP; armed with a PIN. Two app slots, rollback enabled. |
| **Rollback / mark valid** | After an OTA boot the new image must confirm itself (`hmi_ota_mark_valid()`); otherwise the bootloader reverts to the previous slot. |
| **TWDT** | Task watchdog timer; initialised in `app.c`. |
| **NVS** | Non-volatile storage partition used for settings. |

## UI

| Term | Meaning |
|---|---|
| **LVGL** | The embedded graphics library (9.6.0). |
| **LVGL Pro Editor** | The visual editor used to author layouts in `lvgl_ui/`. It exports C (`*_gen.c/.h`). |
| **Layout component** | An XML component in `lvgl_ui/components/hmi/rcc_*` (for example `rcc_overview_hero`). Its generated C is compiled into the firmware. |
| **Screen (preview)** | An XML file in `lvgl_ui/screens/hmi_*`. Used for previews with sample data in the Editor; **not** compiled into the firmware. |
| **Shell** | The persistent chrome: left rail, header, banner, action bar, confirm modal and overlays (`hmi_shell.c`). |
| **Page** | One of Overview, Measure, Faults, Session, Device. Registered in `hmi_pages.c`. |
| **Legacy page** | A page rebuilt on every 1 s refresh (currently Device). |
| **Retained page** | A page whose widgets are created once (`enter`) and then updated in place (`update`, subjects). Overview, Measure, Faults and Session. |
| **Subject / observer** | LVGL's data-binding mechanism. `hmi_model.c` owns one pointer subject per view. |
| **View** | A small zero-padded POD struct built by an `hp_*_view()` function. The model compares views with `memcmp` and notifies only on change. |
| **Overlay** | A full-screen flow (for example Wi-Fi) opened through `hmi_shell_overlay_open()`. |
| **Widget name** | The name given to a widget in the Editor. Firmware finds it with `lv_obj_find_by_name`, so renaming it breaks the binding. |
| **LVGL lock** | The `esp_lvgl_port` mutex. UI work runs under it from the LVGL task. |

## Tooling

| Term | Meaning |
|---|---|
| **Host tests** | C tests (`rcc_codec`, `hmi_present`, `rcc_link`) and the Python `controller_sim` tests, run on a PC via `firmware/test/host/`. |
| **Controller simulator** | `firmware/tools/controller_sim.py`: a Python program that pretends to be the controller over a USB-to-UART adapter. Values are synthetic. |
| **Desktop UI simulator** | `firmware/tools/sim/`: renders the real UI components with LVGL on a PC. |
| **Pixel diff / morph diff** | `check.sh` results. *Pixel diff*: every scenario × page matches a recorded baseline. *Morph diff*: a retained page updated in place equals a fresh build. |
| **Mockup** | `docs/mockups/hmi-ui-landscape.html`: a browser replica of the UI. A reference, not the source of truth. |
