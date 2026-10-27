# Controller simulator

`firmware/tools/controller_sim.py` emulates the Robot Charge Controller on a PC
serial port. It can exercise protocol framing, replies, charging-state
transitions, faults and START/STOP behavior when an HMI controller transport is
available.

## Current HMI limitation

The Elecrow firmware currently binds `rcc_link` to an offline backend. Its USB
Serial/JTAG interface is used for logs and is not the controller link. Therefore
the simulator cannot currently drive the on-device HMI. Do not connect it to a
real charging controller or infer controller/relay verification from simulated
data. This guide applies after the Elecrow transport is integrated.

## Run

From `firmware/`, install the simulator dependency if needed and provide the
serial port assigned to the enabled HMI controller transport:

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
