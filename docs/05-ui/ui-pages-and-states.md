# UI pages and states

What each screen shows, where each value comes from, and how the UI refreshes. For START/STOP and authority rules see [operator-safety-behavior](operator-safety-behavior.md). For how layouts are authored see [lvgl-pro-workflow](lvgl-pro-workflow.md).

Replaces `docs/ui-pages-and-information.md` (stamped `28d2253`, 2026-10-30). Rebuilt on 2026-11-27 from the source at commit `f96b6bb` (`hmi_pages.c`, `hmi_shell.c`, `hmi_present.c`, `page_*.c`). Nothing here was observed on a display; the HTML mockup and the screenshots in `assets/ui/` were **not** compared against the current layouts ([verification-status](../08-status/verification-status.md)). Where a mockup and the firmware differ, the firmware is authoritative.

## 1. Screen layout

- Display: landscape **480 × 320**.
- **Left rail** (about 63 px per tab): Overview, Measure, Faults, Session, Device. The Faults tab carries a dot for an active fault (red) or inhibit (amber).
- **Header**: node chip, a fixed label `NODE RCC-01` taken from the layout, **not** read from the controller (tap → Device › Controller), UART pill `ONLINE · <age>` / `OFFLINE · <age>` / `UART FAILED` (tap → Device › Comm link), and a fault chip (tap → Faults).
- **Banner** (one line, 20 px; two lines, 34 px) when something needs attention: link lost, no authority, latched fault, simulated V/I, UART init failure. A banner that has a target is tappable.
- **Content area**: 408 px wide, starting at x = 64. A banner reduces its height.
- **Action bar** at the bottom (264 px down), or absent on pages that do not need it. Its content depends on page, controller state and the last command.

Page registry: `firmware/components/ui/ui_app/src/hmi_pages.c`. The shell is `hmi_shell.c`.

## 2. Refresh model

- A 1 s LVGL timer (`hmi_shell.c: refresh`) takes **one** snapshot and updates the chrome and the visible page.
- The refresh **does nothing** while a touch is held, a START dialog is open, or an overlay (Wi-Fi) is open. OTA progress still updates. See [operator-safety-behavior §7](operator-safety-behavior.md#7-behaviour-that-can-surprise-an-operator).
- **Retained pages** (Overview, Measure, Faults, Session) create their widgets once (`enter`), then redraw only changed views through LVGL subjects. They are rebuilt when the page, content height or top, or navigation epoch changes (any navigation, or an overlay closing).
- **Legacy page** (Device) is rebuilt from the snapshot on every refresh.
- Polling and freshness: [data-flow §4](../02-architecture/data-flow.md#4-polling-and-freshness). Poll rate does not depend on which page is open.

## 3. Pages

### 3.1 Overview (retained; full-width action bar)

| Region | Content | Source |
|---|---|---|
| State hero | State name and sub-line, e.g. `OPERATIONAL` / `OPERATIONAL · READY`; `UNKNOWN` / `STATE NOT CONFIRMED` when status is not fresh; `NOT_CONFIRMED` when charging without `charging_established` | `hp_state_info` |
| Stage strip | `IDLE · PRE · RELAY · VERIFY · CHG`, one highlighted | `hp_state_info.stage` |
| System rows | **UART** (reply age), **Fault** (short name / `Inhibited` / `Unreadable` / `No faults`), **Relay** (`OPEN` / `CLOSED` command, `—` when unknown), **Sess** (session line; `Session (SIM V/I)` when simulated) | `hp_sys_view` |
| Telemetry cards | **VOLT**, **CURR.**, **POWER** with a gauge and sub-text `NN%/60V`, `/10A`, `/600W`, or `STALE DATA` | `hp_tel_views`, `hp_gauge` |
| Action bar | START or STOP, or command feedback | `hp_bar_spec` |

Gauge full scales (60 V, 10 A, 600 W) are the **operating envelope** of the controller SRS. They are display scales, not limits the HMI enforces; 80 V / 20 A is a non-operating design boundary.

### 3.2 Measure (retained; status strip)

| Region | Content |
|---|---|
| VOUT card (`≤ 60V`) | Value, chip (`VALID` / `BELOW FLOOR` / `INVALID` / `STALE`; stale wins), "Filtered x V" or "Last value is stale" / "Not valid" |
| IOUT card (`± 10A`) | Same; current is **signed** |
| ADC quality matrix | Rows VOUT and IOUT, bits 0–5 plus bit6 (below floor); dashes when no valid read; caption `Seq #n · Age ms · Held` or `Stale (age)` |

`Age` is the controller's sample age; `Held` is how long the HMI has held the reading. They are shown separately and not added. Values are usable only when all six status bits are set. A VOUT below the measurement floor is shown as an upper bound (`<=`).

### 3.3 Faults (retained; status strip; rail badge)

| Region | Content |
|---|---|
| Summary card | `All Systems Nominal — No Faults` / `Active Fault Condition` / `Charging Inhibited` / `Fault State Unreadable`, plus `Read <age> ago` or `Not read` |
| Primary card | `PRIMARY · <fault name>` with id `0x....` and an action hint; for an inhibit: `INHIBIT · <names>` and "START may clear it; the controller decides" |
| Registers | `FAULT REGISTER` and `INHIBIT REGISTER`: hex mask, active-bit count, text |
| Lists | `FAULTS` (19 named bits) and `INHIBITS` (3: REMOTE, RESET, RECOVERY); any other bit shows `Unrecognized (bit N)` |

Names and bit positions follow [code-tables §5–§6](../04-controller-protocol/code-tables.md#5-inhibit-mask). They are `not-frozen` (CR-02): if the controller registry moves a bit, this page will name faults wrongly until updated. The hard-coded table is `hp_fault_names` / `fault_ids` in `hmi_present.c`; the 19 entries match the protocol extract as of 2026-11-27.

### 3.4 Session (retained; status strip)

| Region | Content |
|---|---|
| Four cards | **SESSION** (`#id · state`, or `SIMULATED V/I`), **DURATION** (completed-session duration, `In progress`, or `—`), **LAST CMD** (`START · OK`, `STOP · REJECTED`, `NO REPLY`, `WAIT`), **END REASON** (`COMPLETE`, `REMOTE_STOP`, … `CHARGE_LOST`) |
| Event ring | `EVENT RING BUFFER (n held · m rotated)` or `(offline)`; scrollable rows with relative time and detail; **SYNC** re-reads the snapshot now |

- Duration and end reason come from `SESSION_COMPLETED` / `SESSION_ABORTED` events, shown only when they match the current session id.
- Event times are relative to controller boot. The HMI does not send `SET_TIME`. SYNC does not synchronise a clock.
- The HMI keeps the last **8** events (`RCC_EVENT_SLOTS`). Each event query asks for up to 4.

### 3.5 Device (legacy page, three tabs)

A banner at the top says `PRODUCT BUILD` or `BENCH IMAGE — NOT A PRODUCT BUILD` (from build-flag bit0 only; see gap G5), plus `Relay locked` when build-flag bit1 is clear, or `CONTROLLER INFO NOT READ` / `Link lost`.

| Tab | Content |
|---|---|
| **Controller** | Compatibility chip (`UNREAD` / `COMPATIBLE` / `INCOMPAT.`, product `0x0001` and protocol 1); product/protocol, hardware/firmware revision, image id (8 bytes of SHA-256, abbreviated), boot id; chips for provisional and relay-enable flags (`ON`/`OFF`); **control authority chip** (`THIS HMI`, `NONE`, `CAN`, `RS485`, `UART`, `SVC`); running links CAN / RS485 / UART / SVC |
| **Comm link** | `ONLINE`/`OFFLINE`, reply age, timeout, bad-frame and missed-event counters, status / measurement / fault ages, `UART1`, `115200 8N1`, `GPIO43 / 44` |
| **Settings** | Backlight presets 25/50/75/100 %, sleep timeout Off/1/3/5 min, idle dim level (read-only), language English/Tiếng Việt (stored only), watchdog `5.0 s`, running firmware version, Wi-Fi entry |

Settings and Wi-Fi configure the HMI only. Details: [settings-wifi-ota](../06-services/settings-wifi-ota.md).

### 3.5.1 Wi-Fi flow and overlays

The Wi-Fi screens (scan, password keyboard, progress, failure, network info, disconnect, forget) are a full-screen overlay opened from Device › Settings or the header (`hmi_wifi_ui.c`, `hmi_shell_overlay_open`). While it is open the page below is freed and the refresh is paused. The OTA progress overlay appears while an upload is receiving, verifying or has succeeded.

## 4. Action bar

| Bar | Shown when |
|---|---|
| Full-width **START** | Overview, START allowed |
| Full-width **STOP** | Overview, START not allowed (with `Start blocked: …` if the reason is a stale link or a latched fault) |
| Split: status text + button | A recent command (START/STOP feedback); or, on a non-Overview page, whenever START is not available: the state name, a relay-command line (`Relay commanded open/closed`, with session id) and a STOP button |
| Message only | Command feedback with no button |
| **No-authority** | A recent `REJECTED/UNAUTHORIZED_SOURCE` |
| None | Non-Overview page with START available (no bar, more content height) |

Wording and rules: [operator-safety-behavior §3–§4](operator-safety-behavior.md#3-start).

## 5. State semantics shown to the operator

| Controller state | Hero text (sub-line) |
|---|---|
| `BOOT_SAFE`, `SELF_TEST` | `STARTING UP` |
| `SERVICE_LOCK` | `CONFIG / CALIBRATION NEEDED` (warn) |
| `CONFIG_MODE` | `SERVICE CONFIGURATION` |
| `INHIBITED` | inhibit names, plus `· FAULT <name>` if a primary fault exists (warn) |
| `LATCHED_FAULT` | `FAULT 0x....` (fault) |
| `OPERATIONAL` / `IDLE` | `OPERATIONAL · READY` |
| `VREQ_VALIDATE`, `PRECHECK` | `VALIDATING REQUEST`, `PRE-CHECKING` |
| `RELAY_CLOSING`, `CHARGE_VERIFY` | `CLOSING RELAY`, `VERIFYING CURRENT` |
| `CHARGING` | `OPERATIONAL · SESSION #n` (or `SESSION ACTIVE`) |
| `COMPLETE`, `WAIT_REARM` | `CHARGE COMPLETE`, `WAITING REARM` |

Codes: [code-tables §3](../04-controller-protocol/code-tables.md#3-two-level-state).

## 6. Visual references

- `assets/ui/*.png` and `docs/mockups/hmi-ui-landscape.html` are captures and a browser replica with **illustrative data**; they are not photographs of the display or recorded sessions.
- `lvgl_ui/screens/hmi_*` are Editor previews with sample data (state suffixes such as `_idle`, `_precheck`, `_inhibited`, `_fault`, `_offline`, `_remote`) and are not compiled into the firmware.
- The only pixel-level regression check is the desktop simulator ([firmware/tools/sim/README.md](../../firmware/tools/sim/README.md)); its baseline predates the LVGL Pro migration.

## 7. Differences from the previous page that were resolved or are still open

| Item | Now |
|---|---|
| Control authority "not reported in advance" | **Resolved by CR-01**: Device › Controller shows it. Still not used for gating (G1) |
| STOP `FAILED` wording | Implemented ("Stop took effect, not tracked"). `REJECTED/QUEUE_FULL` on STOP now shows the same text (G2, 2026-11-27) |
| Link start | Now started by `app.c`, not by display start-up |
| Acceptance of navigation, touch, modal, persistence, Wi-Fi/OTA | Still pending on hardware |
