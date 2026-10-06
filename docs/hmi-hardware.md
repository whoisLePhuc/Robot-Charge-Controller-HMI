# HMI hardware: ESP32-3248S035R and the link to the controller

| Item | Value |
|---|---|
| Source | Vendor schematics `ESP32-3248S035-MCU-V1.1.jpg` and `ESP32-3248S035-LCM-V1.1.jpg` (2022-08-26), in [`hardware/ESP32-3248S035-3-5INCH-LCD-main/5-Schematic/`](../hardware/ESP32-3248S035-3-5INCH-LCD-main/5-Schematic/); specification PDF in `2-Specification/` |
| Provenance | Copy of the vendor material from upstream `wegi1/ESP32-3248S035-3-5INCH-LCD`. The two schematics and the specification PDF were checked byte-for-byte on 2026-10-06 against that repository at commit `a980b8f` |
| Labels | `confirmed` = read on schematic V1.1; `inferred` = derived, not measured; `needs_verification` = to be checked on the real board (its revision may differ from V1.1) |

## 1. Display and touch pins (`confirmed`, LCM sheet)

| Function | GPIO | Note |
|---|---:|---|
| LCD SCK / touch DCLK | 14 | shared bus |
| LCD SDI (MOSI) / touch DIN | 13 | shared bus |
| LCD SDO / touch DOUT (MISO) | 12 | LCD through R24 (0 Ω). GPIO12 is the **MTDI strapping pin** (flash voltage at boot); XPT2046 DOUT is high-impedance while its CS is high, so it is normally harmless, but touch CS must be held high through reset (`needs_verification`) |
| LCD CS | 15 | MTDO strapping pin |
| LCD RS (D/C) | 2 | strapping pin |
| LCD RESET | — | `TFT_RST` is tied to the ESP32 **EN**: the LCD resets with the chip; no dedicated GPIO |
| Backlight | 27 | drives low-side MOSFET Q2 (AO3402) with R11 3.9 Ω; R10 10 kΩ pulls the gate down |
| Touch CS (XPT2046) | 33 | |
| Touch IRQ (PENIRQ) | 36 | input only, R3 10 kΩ pull-up to 3.3 V |
| Capacitive touch (C variant) | 21, 25, 32, 33 | not used on the R (resistive) variant |

## 2. Other board resources (`confirmed`, MCU/LCM sheets)

| Block | GPIO | Note |
|---|---|---|
| RGB LED | 4, 16, 17 | active low, 1 kΩ to 3.3 V |
| Light sensor (LDR) | 34 | ADC, 1 MΩ divider |
| TF (SD) card | 5 CS, 18 CLK, 19 MISO, 23 MOSI | separate VSPI, not shared with the LCD |
| Audio (SC8002B) | 26 | |
| BOOT / RST buttons | 0 / EN | auto-reset through CH340C DTR/RTS (T1, T2) |
| Header P3 | 21, 22, 35, GND | spare I/O |
| Header CN1 | 21 | IO21 with R18 10 kΩ pull-up |

## 3. UART0, CH340C and P1 (`confirmed`, MCU sheet)

- **P1** (HC-1.25-4P): pin 1 VIN (through P-MOSFET Q1 to the 5 V rail), pin 2 "TXD2" →
  R5 100 Ω → **U0TXD**, pin 3 "RXD2" → R6 100 Ω → **U0RXD**, pin 4 GND. The silkscreen
  "TXD2/RXD2" is misleading: this is **UART0**, not UART2.
- **CH340C** (U6): TXD wired **directly** to U0RXD, RXD to U0TXD. VCC comes from the
  board's 3.3 V rail, so CH340C is powered whether or not USB is connected.
- Open question (`needs_verification`): **without USB**, does CH340C still drive its
  TXD high? If yes, the controller's TX on P1 pin 3 through 100 Ω cannot pull U0RXD low
  and the ESP32 receives nothing. If CH340C leaves TXD undriven without USB, P1 works.
  The acceptance test in [decisions.md](decisions.md#hmi-d03-link-uart) answers it. It
  has **not been run yet**: the controller link has not been connected during bring-up.
- **With USB** (flashing) CH340C does drive U0RXD: the controller must be disconnected
  from P1 then.

## 4. Link to the controller

**Decision HMI-D03 (2026-10-06): UART0 on P1**, no USB in operation, subject to the
acceptance test in [decisions.md](decisions.md#hmi-d03-link-uart).

| P1 pin | Board signal | To the controller (J2, operational UART) |
|---:|---|---|
| 1 | VIN → Q1 → 5 V | +5V from the control board (HMI supply) |
| 2 | "TXD2" → R5 100 Ω → U0TXD | controller RX, **GPIO21** |
| 3 | "RXD2" → R6 100 Ω → U0RXD | controller TX, **GPIO19** |
| 4 | GND | GND |

- HMI firmware: UART0 at 115 200 8N1; ESP-IDF console/log disabled on UART0.
- No USB in operation; disconnect the controller from P1 when flashing over USB.
- Both ends use 3.3 V logic (`confirmed` on both sides).
- Supply: the HMI takes 5 V from the control board on P1 pin 1. Its current (LCD
  backlight) must fit the control board's +5V rail (`needs_verification`).

### 4.1 Fallback: UART1 on header P3

Use this if the P1 acceptance test fails.

| P3 pin | Board signal | Use | To the controller (J2) |
|---:|---|---|---|
| 1 | IO21 | unused | — |
| 2 | **IO22** | **UART1 TX** of the HMI | controller RX, **GPIO21** |
| 3 | **IO35** | **UART1 RX** of the HMI | controller TX, **GPIO19** |
| 4 | GND | common ground | GND |

- The ESP32 GPIO matrix routes UART1 to any pin; IO35 is input-only, which suffices for
  RX.
- IO35 has **no internal pull-up**. With the controller off or the cable unplugged, a
  floating RX produces garbage bytes. The parser must tolerate them anyway; add 10 kΩ to
  3.3 V on the HMI side (`needs_verification`).
- Leaves UART0/CH340C alone: USB flashing, auto-reset and console logging stay usable.
- Check first: the board really is V1.1, P3 really exposes IO22/IO35, and IO22 is not
  used elsewhere on the revision at hand (`needs_verification`).

### 4.2 Other options

| Option | How | Drawback |
|---|---|---|
| UART2 on IO16/IO17 | solder to the RGB LED pins | the LED blinks with the data; remove it or accept it |
| UART0 on P1 with isolation | cut or jumper the CH340C TXD trace | only needed if the acceptance test fails and P3 is not wanted |

## 5. Power and memory

- Two AMS1117-3.3 regulators: one for the ESP32, one for the TFT (`confirmed`).
- Module **ESP32-WROOM-32**; the schematic shows no PSRAM (`confirmed`). The MCU sheet
  has a 25Q32 FLASH block on the SCS/SCK/SDI/SDO bus; its role relative to the module's
  own flash is unclear. Read the real flash size with `esptool flash_id`
  (`needs_verification`).
- A full 480 × 320 × 2-byte framebuffer is 300 KB and does not fit internal SRAM: use
  partial draw buffers.

## 6. Touch: measured behaviour

Measured on the real board on 2026-10-06 (PlatformIO, ESP-IDF 6.1.0, bring-up app in
`firmware/`). These results supersede an earlier conclusion in this repository's history
that the touch controller was faulty; it was not.

| Finding | Evidence | Label |
|---|---|---|
| XPT2046 wiring is correct: VCC, VREF, IOVDD at 3.3 V, and continuity of CS (GPIO33), DCLK (14), DIN (13), DOUT (12) | multimeter | `confirmed` |
| PENIRQ (GPIO36) is about 3.3 V idle and goes low when touched | multimeter, on-screen readout | `confirmed` |
| **The chip returns valid data only at a 100 kHz SPI clock.** At 1 MHz, 2.5 MHz and above every read is zero while PENIRQ is low | on-screen diagnostic, finger held, 100 kHz vs 1 / 2.5 MHz, full- and half-duplex | `confirmed` |
| A software (bit-banged) read at about 100 kHz gives the same values as the SPI peripheral at 100 kHz | same diagnostic | `confirmed` |
| The vendor's factory image `8-Burn operation/Burn files/LVGL-3.5R.bin` (TFT_eSPI, touch at 2.5 MHz) does not react to touch on this board | run on the board | `confirmed` |
| Why the chip fails above 100 kHz (slow DOUT edge from line loading, a clone part, or similar) | not investigated | `needs_verification` |
| GPIO12 reads about 1.5 V with the internal pull-up on: it is a divider against the strapping pin's internal pull-down, not a short or a driven line | multimeter | `inferred` |

Consequences for the firmware:

- The touch SPI clock is **100 kHz** (`TOUCH_CLOCK_HZ` in `hmi_board.c`). One touch read is
  several 24-bit transfers, about 1.5 ms at this clock, acceptable for the 30 ms LVGL
  read period.
- The bus is the shared SPI3 host through the GPIO matrix, as in the vendor demo; the
  LCD runs at 40 MHz on the same bus.
- Raw readings with a finger pressed: Z1 about 970 (12-bit); the driver's pressure
  threshold is set to 50 in `sdkconfig.defaults`.

### Coordinate mapping

The driver scales raw X to 0..480 (`a`) and raw Y to 0..320 (`b`). Pressing the four
corners of the landscape screen (text reading direction) gave:

| Corner | a | b |
|---|---:|---:|
| top-left | 27 | 302 |
| top-right | 49 | 31 |
| bottom-left | 322 | 305 |
| bottom-right | 350 | 18 |

So screen X follows `b` (reversed) and screen Y follows `a`. `hmi_board.c` maps them
linearly and leaves the driver's swap and mirror off. After this mapping, pressing the
top-right and bottom-left corners reads (479, 18) and (0, 304); Y falls about 15 px short
at the edges because the bezel blocks the fingertip. This is a **coarse two-edge fit**
for bring-up. A proper 3-point calibration, with targets inset from the edges, is still
to be done before the UI relies on edge buttons.

Not yet verified after the mapping: top-left, bottom-right and centre points.

