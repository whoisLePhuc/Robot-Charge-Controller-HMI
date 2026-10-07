#!/usr/bin/env python3
"""RCC operational-UART peer for testing the real HMI. No physical outputs.

Wire contract: docs/controller-interface.md, controller rcc_router_query.c,
rcc_event_log.c and the HMI rcc_codec.c. Python 3.9+, pyserial for serial mode.
The model is a UI/protocol test double, not a replacement for RCC safety logic.
"""
import argparse
import binascii
from collections import deque
from dataclasses import dataclass
import heapq
import queue
import secrets
import shlex
import struct
import sys
import threading
import time
from typing import Optional

PING, DEVICE, STATUS, MEASURE, FAULTS, EVENTS = 1, 2, 0x10, 0x11, 0x12, 0x13
START, STOP = 0x20, 0x21
ACCEPTED, REJECTED, COMPLETED, FAILED = range(4)
COMMANDS = {PING: "PING", DEVICE: "DEVICE", STATUS: "STATUS", MEASURE: "MEASURE",
            FAULTS: "FAULTS", EVENTS: "EVENTS", START: "START", STOP: "STOP"}
FAULT_NAMES = {"internal": (0, 0x0101), "nvs": (1, 0x0102),
               "adc-stale": (5, 0x0302), "voltage": (10, 0x0401),
               "overcurrent": (11, 0x0402), "reverse": (12, 0x0403),
               "not-established": (13, 0x0501)}
MODES = ("normal", "accepted-only", "start-fail", "stop-unconfirmed",
         "stop-fail", "stop-queue-full")


def crc16(data):
    return binascii.crc_hqx(data, 0xFFFF)


def crc32(data):
    return binascii.crc32(data) & 0xFFFFFFFF


def cobs_encode(data):
    out = bytearray(b"\0")
    index, code = 0, 1
    for byte in data:
        if byte:
            out.append(byte)
            code += 1
            if code != 255:
                continue
        out[index] = code
        index = len(out)
        out.append(0)
        code = 1
    out[index] = code
    return bytes(out)


def cobs_decode(data):
    out, index = bytearray(), 0
    while index < len(data):
        code = data[index]
        index += 1
        end = index + code - 1
        if not code or end > len(data):
            raise ValueError("invalid COBS block")
        chunk = data[index:end]
        if 0 in chunk:
            raise ValueError("zero in COBS body")
        out.extend(chunk)
        index = end
        if code != 255 and index < len(data):
            out.append(0)
    return bytes(out)


@dataclass(frozen=True)
class Message:
    command: int
    request_id: int
    payload: bytes = b""
    source: int = 2
    message_type: int = 1


def encode_message(message, destination=2, source=1, corrupt=False):
    """Serialize one object; fragment at 128 bytes, including CRCs at both layers."""
    if len(message.payload) > 496:
        raise ValueError("object exceeds 512 bytes")
    obj = struct.pack("<BBHHIH", 1, message.message_type, 0, message.command,
                      message.request_id, len(message.payload)) + message.payload
    obj += struct.pack("<I", crc32(obj))
    wire = bytearray()
    for index, offset in enumerate(range(0, len(obj), 128)):
        frame_type = 1 if len(obj) <= 128 else (2 if index == 0 else 3)
        raw = struct.pack("<BBBBIHBB", 1, frame_type, destination, source,
                          message.request_id, len(obj), index, 0) + obj[offset:offset + 128]
        check = crc16(raw)
        if corrupt and index == 0:
            check ^= 1
        wire.extend(b"\0" + cobs_encode(raw + struct.pack("<H", check)) + b"\0")
    return bytes(wire)


class Parser:
    """Bounded UART stream parser. Rejects corrupt/address-invalid/out-of-order traffic."""
    def __init__(self, destination=1, message_type=1, clock=time.monotonic):
        self.destination, self.message_type, self.clock = destination, message_type, clock
        self.body = bytearray()
        self.partial = bytearray()
        self.key = None
        self.next_index = 0
        self.dropping = False
        self.last_fragment = 0.0
        self.invalid = 0

    def feed(self, data):
        messages = []
        if self.partial and self.clock() - self.last_fragment > 2.0:
            self.partial.clear()
            self.key = None
        for byte in data:
            if byte == 0:
                if self.body and not self.dropping:
                    try:
                        message = self._frame(bytes(self.body))
                        if message:
                            messages.append(message)
                    except (ValueError, struct.error):
                        self.invalid += 1
                        self.partial.clear()
                        self.key = None
                self.body.clear()
                self.dropping = False
            elif not self.dropping:
                if len(self.body) >= 145:
                    self.body.clear()
                    self.dropping = True
                    self.invalid += 1
                    self.partial.clear()
                    self.key = None
                else:
                    self.body.append(byte)
        return messages

    def _frame(self, body):
        raw = cobs_decode(body)
        if not 15 <= len(raw) <= 142 or crc16(raw[:-2]) != struct.unpack("<H", raw[-2:])[0]:
            raise ValueError("frame CRC/length")
        version, kind, dst, src, rid, size, index, reserved = struct.unpack("<BBBBIHBB", raw[:12])
        if (version != 1 or dst != self.destination or src in (dst, 255) or
                reserved or not 16 <= size <= 512 or index >= (size + 127) // 128):
            raise ValueError("frame header")
        fragment = raw[12:-2]
        if len(fragment) != min(128, size - index * 128):
            raise ValueError("fragment length")
        key = src, dst, rid, size
        if kind == 1:
            if index or size > 128:
                raise ValueError("single frame")
            self.partial.clear()
            self.key = None
            obj = fragment
        elif kind == 2:
            if index or size <= 128:
                raise ValueError("first frame")
            self.partial = bytearray(fragment)
            self.key, self.next_index, self.last_fragment = key, 1, self.clock()
            return None
        elif kind == 3:
            if self.key != key or index != self.next_index or not index:
                raise ValueError("fragment order")
            self.partial.extend(fragment)
            self.next_index += 1
            self.last_fragment = self.clock()
            if len(self.partial) != size:
                return None
            obj = bytes(self.partial)
            self.partial.clear()
            self.key = None
        else:
            raise ValueError("frame kind")
        if crc32(obj[:-4]) != struct.unpack("<I", obj[-4:])[0]:
            raise ValueError("object CRC")
        version, kind, flags, cmd, object_id, plen = struct.unpack("<BBHHIH", obj[:12])
        if (version != 1 or kind != self.message_type or flags or object_id != rid or
                not rid or plen + 16 != size):
            raise ValueError("object header")
        return Message(cmd, rid, obj[12:-4], src, kind)


@dataclass
class LedgerEntry:
    request: Message
    reply: Optional[Message] = None
    terminal_at: Optional[float] = None


class Controller:
    """Deterministic state machine with injectable time, no serial dependency."""
    def __init__(self, clock=time.monotonic):
        self.clock = clock
        self.authority = True
        self.mode = "normal"
        self.measurement = "valid"
        self.link_silent = False
        self.bad_crc = False
        self.drop_next = False
        self.delay_ms = 0
        self.voltage, self.current = 48000, 2000
        self.sequence = 0
        self.boot_id = secrets.randbits(64) or 1
        self.top, self.op, self.relay, self.established = 4, 1, 0, False
        self.fault_mask = self.primary_fault = self.inhibit = self.session_id = 0
        self.start_time = self.clock()
        self.session_started = self.clock()
        self.session_armed = False
        self.event_sequence = 0
        self.events = deque(maxlen=32)
        self.ledger = {}
        self.scheduled = []
        self.schedule_sequence = 0
        self.outbox = []
        self.pending_start = None
        self.boot_event()

    def uptime_us(self):
        return max(0, int((self.clock() - self.start_time) * 1_000_000))

    def add_event(self, code, data=b"", origin=0):
        self.event_sequence += 1
        header = struct.pack("<HBBIQQI", code, 0, 0, self.event_sequence,
                             self.boot_id, self.uptime_us(), origin)
        self.events.append((self.event_sequence, header + data))

    def boot_event(self):
        self.add_event(1, struct.pack("<BBHII", 1, self.top, 0, 1, 1))

    def set_state(self, top, op, origin=0):
        old = self.top, self.op
        self.top, self.op = top, op
        if old != (top, op):
            self.add_event(0x10, bytes((*old, top, op)), origin)

    def set_inhibit(self, value, origin=0):
        old, self.inhibit = self.inhibit, value
        if old != value:
            self.add_event(0x11, struct.pack("<II", old, value), origin)

    def result(self, request, result=COMPLETED, reason=0, data=b""):
        payload = struct.pack("<BBHBBHI", result, 0, reason, self.top, self.op,
                              self.primary_fault, self.inhibit) + data
        return Message(request.command, request.request_id, payload, 1, 2)

    def emit(self, reply, destination, suppress=False):
        if not suppress:
            self.outbox.append((destination, reply))

    def drain(self):
        replies, self.outbox = self.outbox, []
        return replies

    def schedule(self, seconds, callback):
        if len(self.scheduled) >= 64:
            raise RuntimeError("simulator scheduler full")
        self.schedule_sequence += 1
        heapq.heappush(self.scheduled, (self.clock() + seconds, self.schedule_sequence, callback))

    def tick(self):
        while self.scheduled and self.scheduled[0][0] <= self.clock():
            _, _, callback = heapq.heappop(self.scheduled)
            callback()
        for key, entry in list(self.ledger.items()):
            if entry.terminal_at is not None and self.clock() - entry.terminal_at > 2.0:
                del self.ledger[key]

    def finish(self, entry, result=COMPLETED, reason=0):
        key = entry.request.source, entry.request.request_id
        if self.ledger.get(key) is not entry or entry.terminal_at is not None:
            return
        entry.reply = self.result(entry.request, result, reason)
        entry.terminal_at = self.clock()
        suppress = entry.request.command == STOP and self.mode == "stop-unconfirmed"
        self.emit(entry.reply, entry.request.source, suppress)
        if self.pending_start is entry:
            self.pending_start = None

    def end_session(self, reason, origin=0):
        if self.session_armed:
            duration = max(0, min(0xFFFFFFFF, int((self.clock() - self.session_started) * 1000)))
            self.add_event(0x15 if reason == 1 else 0x16,
                           struct.pack("<IB3xIIi", self.session_id, reason, duration,
                                       self.voltage, self.current if self.established else 0),
                           origin)
        self.relay, self.established = 0, False
        self.session_armed = False

    def abort_start(self, reason):
        if self.pending_start:
            self.finish(self.pending_start, FAILED, reason)

    def query(self, request):
        cmd, data = request.command, request.payload
        if cmd == PING:
            if len(data) > 32:
                return self.result(request, REJECTED, 2)
            return self.result(request, data=data)
        if cmd == EVENTS:
            if len(data) != 8 or any(data[5:]):
                return self.result(request, REJECTED, 2)
            cursor = struct.unpack("<I", data[:4])[0]
            oldest = self.events[0][0] if self.events else 0
            missed = max(0, oldest - cursor) if cursor else 0
            seq, count, encoded = max(cursor, oldest), 0, bytearray()
            for event_seq, payload in self.events:
                if event_seq < seq:
                    continue
                if (data[4] and count == data[4]) or len(encoded) + len(payload) + 1 > 224:
                    break
                encoded.extend(bytes((len(payload),)) + payload)
                count += 1
                seq = event_seq + 1
            page = struct.pack("<IIIB3x", seq if oldest else cursor, oldest, missed, count)
            return self.result(request, data=page + encoded)
        if data:
            return self.result(request, REJECTED, 2)
        self.sequence = (self.sequence + 1) & 0xFFFFFFFF
        if cmd == DEVICE:
            data = struct.pack("<HBBII8sQI4H", 1, 1, 7, 1, 1, b"SIM-RCC1",
                               self.boot_id, 4, 1, 1, 1, 1)
        elif cmd == STATUS:
            data = struct.pack("<IQIBBBBIIIII4H", self.sequence, self.boot_id,
                               self.fault_mask, self.relay, 0, int(self.established), 0,
                               self.session_id, 1, 1, 1, 1, 1, 1, 1, 1)
        elif cmd == MEASURE:
            if self.measurement == "missing":
                return self.result(request, FAILED, 0x0C)
            age = 2000 if self.measurement == "stale" else 5
            status = 1 if self.measurement == "invalid" else (0x3D if age == 2000 else 0x3F)
            v = 3380 if self.measurement == "floor" else self.voltage
            i = self.current if self.established or self.op == 5 else 0
            data = struct.pack("<IIIiIiHHI", self.sequence, age, v, i, v, i,
                               status | (0x40 if self.measurement == "floor" else 0),
                               status, 0)
        elif cmd == FAULTS:
            data = struct.pack("<IIHH", self.fault_mask, self.inhibit, self.primary_fault, 0)
        else:
            return self.result(request, REJECTED, 4)
        return self.result(request, data=data)

    def handle(self, request):
        self.tick()
        if request.command in (PING, DEVICE, STATUS, MEASURE, FAULTS, EVENTS):
            self.emit(self.query(request), request.source)
            return
        if request.command not in (START, STOP):
            service = (0x100 <= request.command <= 0x105 or
                       0x200 <= request.command <= 0x208 or request.command == 0x300)
            reason = 5 if service or (request.command == 0x22 and not self.authority) else 4
            self.emit(self.result(request, REJECTED, reason), request.source)
            return
        key = request.source, request.request_id
        if key in self.ledger:
            entry = self.ledger[key]
            if entry.request.command != request.command or entry.request.payload != request.payload:
                self.emit(self.result(request, REJECTED, 6), request.source)
            elif entry.reply:
                self.emit(entry.reply, request.source,
                          request.command == STOP and self.mode == "stop-unconfirmed")
            return
        if not self.authority or request.payload:
            self.emit(self.result(request, REJECTED, 5 if not self.authority else 2),
                      request.source)
            return
        count = sum(e.request.command == request.command for e in self.ledger.values())
        if count >= (4 if request.command == STOP else 8):
            if request.command == STOP:
                self.apply_stop(request.request_id)
            self.emit(self.result(request, REJECTED, 7), request.source)
            return
        if request.command == START:
            reason = (0x0A if self.fault_mask else 8 if self.pending_start or
                      not (self.top == 5 or (self.top == 4 and self.op in (1, 2, 3))) else
                      0x0C if self.measurement in ("invalid", "stale", "missing") else
                      0x10 if self.measurement == "floor" else 0)
            if reason:
                entry = LedgerEntry(request, self.result(request, REJECTED, reason), self.clock())
                self.ledger[key] = entry
                self.emit(entry.reply, request.source)
                return
        entry = LedgerEntry(request)
        self.ledger[key] = entry
        entry.reply = self.result(request, ACCEPTED)
        if request.command == STOP:
            self.apply_stop(request.request_id)       # model OFF before any STOP result
            if self.mode == "stop-queue-full":
                self.finish(entry, REJECTED, 7)
                return
            entry.reply = self.result(request, ACCEPTED)
            self.emit(entry.reply, request.source, self.mode == "stop-unconfirmed")
            mode = self.mode
            self.schedule(0.08, lambda: self.finish(entry, FAILED if mode == "stop-fail"
                                                  else COMPLETED, 0x0F if mode == "stop-fail" else 0))
            return
        self.pending_start = entry
        self.set_inhibit(0, request.request_id)
        self.set_state(4, 3, request.request_id)
        self.emit(entry.reply, request.source)
        if self.mode == "accepted-only":
            return
        self.session_id += 1
        self.session_armed = True
        self.session_started = self.clock()
        self.add_event(0x13, struct.pack("<IBBH", self.session_id, 0, 3, 0), request.request_id)
        mode = self.mode
        def advance(op):
            if self.pending_start is entry:
                self.set_state(4, op, request.request_id)
                self.relay = int(op >= 4)
        def terminal():
            if self.pending_start is not entry:
                return
            if mode == "start-fail":
                self.end_session(5, request.request_id)
                self.set_inhibit(4, request.request_id)
                self.set_state(5, 0, request.request_id)
                self.finish(entry, FAILED, 0x12)
            else:
                self.relay, self.established = 1, True
                self.set_state(4, 6, request.request_id)
                self.add_event(0x14, struct.pack("<IIi", self.session_id, self.voltage,
                                               self.current), request.request_id)
                self.finish(entry)
        self.schedule(0.3, lambda: advance(4))
        self.schedule(0.6, lambda: advance(5))
        self.schedule(1.3, terminal)

    def apply_stop(self, origin=0):
        self.end_session(2, origin)
        self.set_inhibit(self.inhibit | 1, origin)
        self.set_state(6 if self.fault_mask else 5, 0, origin)
        self.abort_start(0x13)

    def reboot(self):
        self.end_session(7)
        self.ledger.clear()
        self.scheduled.clear()
        self.outbox.clear()
        self.pending_start = None
        self.boot_id = (self.boot_id + 1) & 0xFFFFFFFFFFFFFFFF or 1
        self.start_time = self.clock()
        self.event_sequence = self.sequence = 0
        self.events.clear()
        self.fault_mask = self.primary_fault = 0
        self.top, self.op = (5, 0) if self.inhibit else (4, 1)
        self.boot_event()

    def command(self, line):
        """Console mutations run only on the serial loop thread."""
        args = shlex.split(line)
        if not args:
            return ""
        name = args[0].lower()
        if name in ("quit", "exit"):
            return "quit"
        if name == "help":
            return HELP
        if name == "status":
            return (f"SIM boot={self.boot_id:016X} state={self.top}/{self.op} "
                    f"relay={self.relay} session={self.session_id} "
                    f"fault=0x{self.fault_mask:X} inhibit=0x{self.inhibit:X} "
                    f"authority={self.authority} mode={self.mode} measure={self.measurement} "
                    f"silent={self.link_silent} crc_bad={self.bad_crc} delay={self.delay_ms}ms")
        if name == "reboot" and len(args) == 1:
            self.reboot()
        elif name in ("ready", "charging", "complete") and len(args) == 1:
            self.abort_start(8)
            self.end_session(1 if name == "complete" else 8)
            self.fault_mask = self.primary_fault = 0
            self.set_inhibit(0)
            self.set_state(4, {"ready": 1, "charging": 6, "complete": 7}[name])
            if name == "charging":
                self.session_id += 1
                self.session_armed = True
                self.session_started = self.clock()
                self.relay, self.established = 1, True
                self.add_event(0x13, struct.pack("<IBBH", self.session_id, 0, 3, 0))
                self.add_event(0x14, struct.pack("<IIi", self.session_id, self.voltage, self.current))
        elif name == "fault" and len(args) == 2 and args[1] in (*FAULT_NAMES, "clear"):
            old = self.fault_mask
            if args[1] == "clear":
                self.fault_mask = self.primary_fault = 0
                if old:
                    self.set_state(5 if self.inhibit else 4, 0 if self.inhibit else 1)
            else:
                bit, self.primary_fault = FAULT_NAMES[args[1]]
                self.fault_mask = 1 << bit
                self.end_session(4)
                self.set_state(6, 0)
                self.abort_start(0x14)
            self.add_event(0x12, struct.pack("<IIHH", old, self.fault_mask, self.primary_fault, 0))
        elif name == "inhibit" and len(args) == 2 and args[1] in ("remote", "reset", "recovery", "clear"):
            value = {"remote": 1, "reset": 2, "recovery": 4, "clear": 0}[args[1]]
            old = self.inhibit
            if value:
                self.end_session(2 if value == 1 else 3)
                self.abort_start(0x13)
            self.set_inhibit(value)
            if value or old:
                self.set_state(6 if self.fault_mask else 5 if value else 4,
                               0 if value or self.fault_mask else 1)
        elif name == "authority" and len(args) == 2 and args[1] in ("allow", "deny"):
            self.authority = args[1] == "allow"
        elif name == "link" and len(args) == 2 and args[1] in ("normal", "silence"):
            self.link_silent = args[1] == "silence"
        elif name == "measurement" and len(args) == 2 and args[1] in ("valid", "invalid", "stale", "floor", "missing"):
            self.measurement = args[1]
        elif name == "control" and len(args) == 2 and args[1] in MODES:
            self.mode = args[1]
        elif name == "wire" and len(args) == 2 and args[1] in ("normal", "bad-crc", "drop-next"):
            self.bad_crc = args[1] == "bad-crc"
            self.drop_next = args[1] == "drop-next"
        elif name == "delay" and len(args) == 2 and 0 <= int(args[1]) <= 20000:
            self.delay_ms = int(args[1])
        elif name == "values" and len(args) == 3 and 0 <= int(args[1]) <= 60000 and -10000 <= int(args[2]) <= 10000:
            self.voltage, self.current = int(args[1]), int(args[2])
        elif name == "events" and len(args) == 2 and 1 <= int(args[1]) <= 1000:
            for i in range(int(args[1])):
                self.add_event(0x32, struct.pack("<III", i, i, i))
        else:
            raise ValueError("Invalid command. Type help.")
        return self.command("status")


HELP = """Commands (simulated state only):
  ready | charging | complete | reboot | status | quit
  fault overcurrent|voltage|reverse|adc-stale|internal|nvs|not-established|clear
  inhibit remote|reset|recovery|clear
  authority allow|deny                 (active-interface permission; monitoring stays allowed)
  link normal|silence                  (silence does not stop a session)
  measurement valid|invalid|stale|floor|missing
  control normal|accepted-only|start-fail|stop-unconfirmed|stop-fail|stop-queue-full
  wire normal|bad-crc|drop-next
  delay 0..20000                       (reply delay, milliseconds)
  values VOUT_mV IOUT_mA               (0..60000 and -10000..10000)
  events 1..1000                       (exercise pagination/overwritten events)
Tip: enter control normal, ready, then measurement valid on separate lines to restore START.
"""


def run_serial(args):
    try:
        import serial
    except ImportError:
        print("Missing pyserial. Install: python -m pip install -r tools/controller_sim_requirements.txt",
              file=sys.stderr)
        return 2
    controller = Controller()
    if args.scenario != "ready":
        controller.command({"charging": "charging", "fault": "fault overcurrent",
                            "inhibited": "inhibit remote", "unauthorized": "authority deny"}[args.scenario])
    parser = Parser()
    console = queue.Queue(maxsize=64)
    def read_console():
        for line in sys.stdin:
            try:
                console.put(line.rstrip(), timeout=1)
            except queue.Full:
                print("Console queue full; command dropped.", file=sys.stderr)
    if not args.no_console:
        threading.Thread(target=read_console, daemon=True).start()
    # Set control lines before opening: avoid intentional ESP32 auto-reset/BOOT.
    port = serial.Serial(port=None, baudrate=115200, timeout=0.01, write_timeout=0.5,
                         rtscts=False, dsrdtr=False)
    port.dtr = False
    port.rts = False
    port.port = args.port
    pending_tx, tx_sequence = [], 0
    received = transmitted = dropped = 0
    deadline = time.monotonic() + args.duration if args.duration else None
    try:
        port.open()
        print(f"SIMULATED RCC on {args.port}, 115200 8N1. No physical relay outputs.")
        print("Keep controller P1 disconnected; close Serial Monitor/upload tools.")
        print(HELP if not args.no_console else controller.command("status"))
        while deadline is None or time.monotonic() < deadline:
            while not console.empty():
                try:
                    previous_boot = controller.boot_id
                    answer = controller.command(console.get_nowait())
                    if controller.boot_id != previous_boot:
                        pending_tx.clear()
                        parser = Parser()
                    if answer == "quit":
                        return 0
                    if answer:
                        print(answer)
                except ValueError as error:
                    print(error, file=sys.stderr)
            for message in parser.feed(port.read(min(port.in_waiting or 1, 4096))):
                received += 1
                if args.verbose or message.command in (START, STOP):
                    print(f"RX {COMMANDS.get(message.command, hex(message.command))} "
                          f"id={message.request_id:08X} source={message.source:02X}")
                controller.handle(message)
            controller.tick()
            if controller.link_silent:
                pending_tx.clear()
            for destination, reply in controller.drain():
                if controller.link_silent or controller.drop_next or len(pending_tx) >= 256:
                    controller.drop_next = False
                    dropped += 1
                    continue
                wire = encode_message(reply, destination=destination, corrupt=controller.bad_crc)
                tx_sequence += 1
                heapq.heappush(pending_tx, (time.monotonic() + controller.delay_ms / 1000,
                                          tx_sequence, wire, reply))
            now = time.monotonic()
            while pending_tx and pending_tx[0][0] <= now:
                _, _, wire, reply = heapq.heappop(pending_tx)
                if port.write(wire) != len(wire):
                    raise serial.SerialTimeoutException("partial serial write")
                transmitted += 1
                if args.verbose or reply.command in (START, STOP):
                    result, _, reason = struct.unpack("<BBH", reply.payload[:4])
                    print(f"TX {COMMANDS.get(reply.command, hex(reply.command))} "
                          f"id={reply.request_id:08X} result={result} reason=0x{reason:04X}")
        return 0
    except (serial.SerialException, OSError) as error:
        print(f"Serial error: {error}. Close Monitor/upload tools and check the port.", file=sys.stderr)
        return 2
    except KeyboardInterrupt:
        return 0
    finally:
        if port.is_open:
            port.close()
        print(f"SIM closed: RX={received} TX={transmitted} invalid_frames={parser.invalid} "
              f"dropped_replies={dropped}")


def main():
    cli = argparse.ArgumentParser(description=__doc__)
    cli.add_argument("--port", default="COM4", help="HMI USB serial port (default COM4)")
    cli.add_argument("--scenario", choices=("ready", "charging", "fault", "inhibited", "unauthorized"),
                     default="ready")
    cli.add_argument("--verbose", action="store_true", help="log all polling traffic")
    cli.add_argument("--no-console", action="store_true", help="disable interactive stdin")
    cli.add_argument("--duration", type=float, default=0, help="exit after N seconds; 0 runs until quit")
    args = cli.parse_args()
    if args.duration < 0:
        cli.error("--duration must be nonnegative")
    return run_serial(args)


if __name__ == "__main__":
    sys.exit(main())
