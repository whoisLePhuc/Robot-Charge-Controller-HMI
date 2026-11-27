# HMI documentation

Documentation for the Robot Charge Controller HMI (Elecrow ESP32-S3 touchscreen, ESP-IDF 6.1 + LVGL 9.6).

The set is being reorganised into numbered sections. Pages marked **new** are in the final structure. Pages marked **legacy** are the previous documents, still valid as sources but stamped with an older revision and waiting to be split or re-checked. Pages marked *planned* do not exist yet.

> **Status warning:** implementation statements do not establish hardware acceptance. The restructured firmware has not been flashed to a board. See [verification-status](08-status/verification-status.md).

## Start here

| If you are... | Read |
|---|---|
| New to the project | [project-overview](01-overview/project-overview.md), then [repository-map](02-architecture/repository-map.md), then [glossary](01-overview/glossary.md) |
| Changing how screens look | [lvgl-pro-workflow](05-ui/lvgl-pro-workflow.md) |
| Changing firmware logic | [firmware-architecture](02-architecture/firmware-architecture.md), [data-flow](02-architecture/data-flow.md), [firmware/components/README.md](../firmware/components/README.md), [firmware/components/ui/README.md](../firmware/components/ui/README.md) |
| Changing START/STOP, banner or action bar | [operator-safety-behavior](05-ui/operator-safety-behavior.md) first |
| Working on the controller protocol | [framing-and-transport](04-controller-protocol/framing-and-transport.md), [commands-and-payloads](04-controller-protocol/commands-and-payloads.md), [code-tables](04-controller-protocol/code-tables.md) |
| Wiring or flashing a board | [controller-wiring](03-hardware/controller-wiring.md), [build-and-flash](07-development/build-and-flash.md), [elecrow-s3-board](03-hardware/elecrow-s3-board.md) |
| Checking what is proven | [verification-status](08-status/verification-status.md) |
| Testing on the board | [hardware-test-plan](08-status/hardware-test-plan.md) |

## Index

### 01 Overview
| Page | State |
|---|---|
| [project-overview](01-overview/project-overview.md) | new |
| [getting-started](01-overview/getting-started.md) | new (steps 1–3 run on 2026-11-27; 4–7 not run) |
| [glossary](01-overview/glossary.md) | new |

### 02 Architecture
| Page | State |
|---|---|
| [repository-map](02-architecture/repository-map.md) | new |
| [firmware-architecture](02-architecture/firmware-architecture.md) | new (layers, start order, tasks, locks; built from source 2026-11-27) |
| [data-flow](02-architecture/data-flow.md) | new (controller to screen and back) |

### 03 Hardware
| Page | State |
|---|---|
| [elecrow-s3-board](03-hardware/elecrow-s3-board.md) | new |
| [controller-wiring](03-hardware/controller-wiring.md) | new |

### 04 Controller protocol
| Page | State |
|---|---|
| [framing-and-transport](04-controller-protocol/framing-and-transport.md) | new (split from `controller-interface.md`) |
| [commands-and-payloads](04-controller-protocol/commands-and-payloads.md) | new (split; GET_STATUS byte 19 / bit5 corrected) |
| [code-tables](04-controller-protocol/code-tables.md) | new (split; not re-checked against current controller headers) |
| [controller-change-requests](04-controller-protocol/controller-change-requests.md) | moved, content unchanged |

### 05 UI
| Page | State |
|---|---|
| [lvgl-pro-workflow](05-ui/lvgl-pro-workflow.md) | new |
| [ui-pages-and-states](05-ui/ui-pages-and-states.md) | new (replaces `ui-pages-and-information.md`, rebuilt from source) |
| [operator-safety-behavior](05-ui/operator-safety-behavior.md) | new (includes known gaps G1–G6 for human review) |
| [mockups/hmi-ui-landscape.html](mockups/hmi-ui-landscape.html) | reference only; the firmware is authoritative |

### 06 Services
| Page | State |
|---|---|
| [settings-wifi-ota](06-services/settings-wifi-ota.md) | moved; header re-stamped, partly spot-checked |

### 07 Development
| Page | State |
|---|---|
| [build-and-flash](07-development/build-and-flash.md) | new |
| [testing-and-simulation](07-development/testing-and-simulation.md) | new (replaces `controller-simulator.md`; commands re-run 2026-11-27) |
| [how-to-recipes](07-development/how-to-recipes.md) | new (not executed end to end) |
| [troubleshooting](07-development/troubleshooting.md) | new (causes from source; nothing reproduced on a board) |
| Tool references | [firmware/README](../firmware/README.md), [desktop UI simulator](../firmware/tools/sim/README.md) |

### 08 Status
| Page | State |
|---|---|
| [verification-status](08-status/verification-status.md) | new |
| [hardware-test-plan](08-status/hardware-test-plan.md) | new (everything that needs the board; nothing run yet) |

### 09 Decisions
| Page | State |
|---|---|
| [ADR index](09-decisions/README.md), [0001 component layout](09-decisions/0001-esp-idf-component-layout.md) | new |
| [0002 LVGL Pro UI source](09-decisions/0002-lvgl-pro-ui-source.md) | new (accepted; open questions remain) |
| [archive/firmware-restructure-design](09-decisions/archive/firmware-restructure-design.md) | moved, translated to English, historical |

## Conventions for this set

- English only.
- Diagrams are written in Mermaid (fenced ` ```mermaid ` blocks) so they render on GitHub and diff as text. Directory trees and byte layouts stay as plain text.
- Each page states what revision it was checked against, or says it was not.
- Status and test results live in [verification-status](08-status/verification-status.md); other pages link to it.
- Protocol facts defer to the controller repository (`../Robot_Charge_Controller/docs/firmware/`, the ICD in particular). Where this set and the ICD differ, the ICD is right.
- Use the confidence labels `confirmed`, `inferred`, `not-frozen` for protocol facts.
