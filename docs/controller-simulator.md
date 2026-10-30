# Controller simulator

`firmware/tools/controller_sim.py` emulates the Robot Charge Controller on a PC
serial port. It can exercise protocol framing, replies, charging-state
transitions, faults and START/STOP behavior over the HMI UART1 transport.

## Connection and verification status

The Elecrow target compiles `rcc_link.c` and `rcc_codec.c` and attempts to start
UART1 during display initialization. Connect a separate **3.3 V logic**
USB-to-UART adapter: adapter TX to HMI GPIO44 (RX), adapter RX to GPIO43 (TX),
and common GND. Confirm the Elecrow connector mapping before wiring. Use the
adapter's COM port at 115200 8N1, with no flow control. Native USB Serial/JTAG
is used for logs and is not the controller link.

Disconnect the real controller from this UART while using the simulator.
The physical simulator-to-HMI workflow has not been verified in this repository;
protocol tests on a PC do not verify that connection or a physical relay.

## Run

From `firmware/`, install the simulator dependency if needed and provide the
serial port assigned to the external USB-to-UART adapter:

~~~powershell
python -m pip install -r tools/controller_sim_requirements.txt
python tools/controller_sim.py --port COMx
~~~

Use the actual port in place of `COMx`. Close other applications that own it.
The simulator holds DTR/RTS deasserted to avoid intentionally toggling
auto-reset/BOOT lines.

The default scenario is synthetic OPERATIONAL/IDLE with valid sample
measurements and no faults. Interactive commands include:

~~~text
help
status
charging
fault overcurrent
ready
authority deny
link silence
link normal
reboot
quit
~~~

Optional scenarios include `ready`, `charging`, `fault` and `inhibited`:

~~~powershell
python tools/controller_sim.py --port COMx --scenario charging
python tools/controller_sim.py --port COMx --scenario fault
python tools/controller_sim.py --port COMx --verbose --no-console --duration 30
~~~

All readings and state transitions are test data. They are not measurements,
timing guarantees, relay feedback or evidence of a real charging session.

## Host tests

From `firmware/`:

~~~powershell
python -B -m unittest discover -s test/host -p test_controller_sim.py -v
~~~

On 2026-10-30 this command discovered 33 tests: 31 passed and 2 were skipped.
The skipped tests require pyserial and an optional independent codec path,
respectively. Install `tools/controller_sim_requirements.txt` for the serial
test. To enable the independent comparison, set `RCC_REFERENCE_CODEC_PATH` to
the controller repository's
`tools/calibration/src/rcc_calibration_tool/protocol/codec.py` before running
the command. This Python comparison is separate from the C codec test file
`firmware/test/host/test_rcc_codec.c`.
