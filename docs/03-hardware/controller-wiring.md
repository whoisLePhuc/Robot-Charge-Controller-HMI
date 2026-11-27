# Controller wiring

How the HMI connects to the Robot Charge Controller, and to the PC controller simulator in its place.

**Not verified.** The pin assignments below are source bindings. Which physical Elecrow connector exposes GPIO43 and GPIO44 has **not** been confirmed, and the wiring has not been exercised with the current image ([verification-status](../08-status/verification-status.md)). Confirm the connector mapping before wiring.

## To the controller

Logic level is **3.3 V UART**, not RS-232. Connect the HMI to the controller's **operational UART** (connector J2). Transmit goes to receive.

| HMI (UART1) | Controller operational UART (J2) |
|---|---|
| GPIO43, TX | GPIO21, RX |
| GPIO44, RX | GPIO19, TX |
| GND | GND |

Parameters: 115200 bit/s, 8N1, no flow control, point-to-point, full duplex. Sources: HMI `hmi_config.h` (`RCC_LINK_*`); controller `rcc_board_binding.h` and `UART.md` §3.1 (see [framing-and-transport §1](../04-controller-protocol/framing-and-transport.md#1-physical-layer-controller-operational-uart)).

The Device page's link tab (COMM LINK) shows the HMI-side binding (`UART1`, `GPIO43 / 44`) read from the same constants.

Wiring the link does not grant control authority. The controller must be configured to treat the operational UART as the active control interface; until then the HMI can monitor but START/STOP are rejected ([commands-and-payloads §1](../04-controller-protocol/commands-and-payloads.md#1-conditions-for-hmi-control)).

## To the PC simulator (instead of the controller)

Use a **separate 3.3 V USB-to-UART adapter**. Disconnect the real controller first.

| HMI | USB-to-UART adapter |
|---|---|
| GPIO43, TX | RX |
| GPIO44, RX | TX |
| GND | GND |

The adapter's port is used by `controller_sim.py`. The HMI's native USB (USB Serial/JTAG) stays the log console and flashing port; it is not this link. Simulator values and relay states are synthetic and operate no charging hardware. Usage: [testing-and-simulation §4](../07-development/testing-and-simulation.md#4-controller-simulator-on-a-real-hmi).

## Checklist before powering

1. Confirm the Elecrow connector that carries GPIO43/GPIO44; record it here once verified.
2. Both sides are 3.3 V logic. Do not connect RS-232 or 5 V signals.
3. TX crosses to RX; grounds are common.
4. Only one peer on the link: real controller **or** simulator, never both.
5. Keep USB flashing/monitoring on the native USB port.
