"""Run: python -m unittest discover -s test/host -p test_controller_sim.py -v"""
import argparse
import contextlib
import importlib
import io
import os
from pathlib import Path
import struct
import sys
import time
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "tools"))
sys.path.insert(0, str(Path(__file__).resolve().parent))  # supports staging validation
import controller_sim as sim


class Clock:
    def __init__(self):
        self.now = 100.0

    def __call__(self):
        return self.now

    def advance(self, seconds):
        self.now += seconds


def header(reply):
    return struct.unpack("<BBHBBHI", reply.payload[:12])


class WireTests(unittest.TestCase):
    def test_crc_check_values(self):
        self.assertEqual(sim.crc16(b"123456789"), 0x29B1)
        self.assertEqual(sim.crc32(b"123456789"), 0xCBF43926)

    def test_controller_golden_request(self):
        golden = bytes.fromhex(
            "00 0a 01 01 01 02 78 56 34 12 14 01 01 03 01 01 01 02 20 06 "
            "78 56 34 12 04 0b de ad be ef 74 c9 40 30 26 b1 00")
        message = sim.Message(sim.START, 0x12345678, bytes.fromhex("de ad be ef"))
        self.assertEqual(sim.encode_message(message, destination=1, source=2), golden)
        parser, decoded = sim.Parser(), []
        for byte in golden:
            decoded.extend(parser.feed(bytes((byte,))))
        self.assertEqual(decoded, [message])

    def test_cobs_boundaries(self):
        for data in (b"", b"\0", b"\0" * 256, b"A" * 254, b"A" * 255,
                     bytes(range(256)) * 2):
            self.assertEqual(sim.cobs_decode(sim.cobs_encode(data)), data)

    def test_maximum_object_and_arbitrary_chunks(self):
        message = sim.Message(sim.EVENTS, 55, bytes(range(248)) * 2, 1, 2)
        wire = sim.encode_message(message)
        parser, decoded = sim.Parser(2, 2), []
        for offset in range(0, len(wire), 7):
            decoded.extend(parser.feed(wire[offset:offset + 7]))
        self.assertEqual(decoded, [message])

    def test_frame_crc_rejection_and_recovery(self):
        request = sim.Message(sim.PING, 1)
        parser = sim.Parser()
        self.assertEqual(parser.feed(sim.encode_message(request, 1, 2, corrupt=True)), [])
        self.assertEqual(parser.feed(sim.encode_message(request, 1, 2)), [request])
        self.assertEqual(parser.invalid, 1)

    def test_object_crc_rejected_even_with_good_frame_crc(self):
        wire = sim.encode_message(sim.Message(sim.PING, 1), 1, 2)
        raw = bytearray(sim.cobs_decode(wire[1:-1]))
        raw[16] ^= 1
        raw[-2:] = struct.pack("<H", sim.crc16(raw[:-2]))
        parser = sim.Parser()
        self.assertEqual(parser.feed(b"\0" + sim.cobs_encode(raw) + b"\0"), [])
        self.assertEqual(parser.invalid, 1)

    def test_wrong_destination_source_and_id(self):
        for dst, src, rid in ((255, 2, 1), (3, 2, 1), (1, 1, 1),
                              (1, 255, 1), (1, 2, 0)):
            self.assertEqual(sim.Parser().feed(sim.encode_message(
                sim.Message(sim.PING, rid), dst, src)), [])

    def test_overlong_noise_recovers(self):
        parser = sim.Parser()
        self.assertEqual(parser.feed(b"A" * 10000), [])
        self.assertLessEqual(len(parser.body), 145)
        request = sim.Message(sim.STATUS, 1)
        self.assertEqual(parser.feed(sim.encode_message(request, 1, 2)), [request])

    def test_missing_fragment_and_expiry(self):
        clock = Clock()
        message = sim.Message(sim.EVENTS, 1, b"e" * 300, 1, 2)
        frames = [b"\0" + body + b"\0" for body in sim.encode_message(message).split(b"\0") if body]
        parser = sim.Parser(2, 2, clock)
        self.assertEqual(parser.feed(frames[0] + frames[2]), [])
        self.assertEqual(parser.feed(b"".join(frames)), [message])
        self.assertEqual(parser.feed(frames[0]), [])
        clock.advance(2.01)
        self.assertEqual(parser.feed(b"".join(frames[1:])), [])


class ModelTests(unittest.TestCase):
    def setUp(self):
        self.clock = Clock()
        self.c = sim.Controller(self.clock)
        self.rid = 1

    def ask(self, command, payload=b"", rid=None, source=2):
        if rid is None:
            rid, self.rid = self.rid, self.rid + 1
        self.c.handle(sim.Message(command, rid, payload, source))
        return [reply for _, reply in self.c.drain()]

    def advance(self, seconds):
        self.clock.advance(seconds)
        self.c.tick()
        return [reply for _, reply in self.c.drain()]

    def test_query_schemas_and_ping_echo(self):
        self.assertEqual(self.ask(sim.PING, b"\0test")[0].payload[12:], b"\0test")
        for cmd, size in ((sim.DEVICE, 40), (sim.STATUS, 48), (sim.MEASURE, 32), (sim.FAULTS, 12)):
            reply = self.ask(cmd)[0]
            self.assertEqual(header(reply)[0], sim.COMPLETED)
            self.assertEqual(len(reply.payload), 12 + size)
        device = self.ask(sim.DEVICE)[0].payload[12:]
        self.assertEqual(struct.unpack("<HBB", device[:4]), (1, 1, 7))
        self.assertEqual(device[12:20], b"SIM-RCC1")

    def test_start_lifecycle_and_same_id(self):
        reply = self.ask(sim.START, rid=40)[0]
        self.assertEqual(header(reply)[0], sim.ACCEPTED)
        self.assertFalse(self.c.established)
        self.advance(0.31)
        self.assertEqual((self.c.op, self.c.relay), (4, 1))
        self.advance(0.3)
        self.assertEqual(self.c.op, 5)
        terminal = self.advance(0.7)[0]
        self.assertEqual((terminal.request_id, header(terminal)[0]), (40, sim.COMPLETED))
        self.assertTrue(self.c.established)
        self.assertEqual((self.c.top, self.c.op), (4, 6))
        self.assertEqual(self.advance(2), [])  # no second terminal result

    def test_duplicate_start_does_not_arm_twice(self):
        accepted = self.ask(sim.START, rid=5)[0]
        self.assertEqual(self.ask(sim.START, rid=5), [accepted])
        self.assertEqual(self.c.session_id, 1)
        terminal = self.advance(1.4)[0]
        self.assertEqual(self.ask(sim.START, rid=5), [terminal])
        self.assertEqual(self.c.session_id, 1)

    def test_duplicate_conflict_and_source_key(self):
        self.ask(sim.START, rid=5)
        reply = self.ask(sim.STOP, rid=5)[0]
        self.assertEqual(header(reply)[0:3:2], (sim.REJECTED, 6))
        self.assertFalse(self.c.inhibit)
        self.assertEqual(header(self.ask(sim.STOP, rid=5, source=3)[-1])[0], sim.ACCEPTED)

    def test_stop_off_before_result_and_aborts_start(self):
        self.ask(sim.START, rid=10)
        self.advance(0.7)
        replies = self.ask(sim.STOP, rid=11)
        self.assertEqual(self.c.relay, 0)
        self.assertEqual(self.c.inhibit & 1, 1)
        aborted = next(reply for reply in replies if reply.command == sim.START)
        self.assertEqual((header(aborted)[0], header(aborted)[2]), (sim.FAILED, 0x13))
        terminal = self.advance(0.1)[0]
        self.assertEqual((terminal.command, header(terminal)[0]), (sim.STOP, sim.COMPLETED))
        self.advance(2)
        self.assertFalse(self.c.established)
        self.assertEqual(self.c.relay, 0)

    def test_fault_aborts_start(self):
        self.ask(sim.START)
        self.advance(0.7)
        self.c.command("fault overcurrent")
        terminal = self.c.drain()[0][1]
        self.assertEqual((header(terminal)[0], header(terminal)[2]), (sim.FAILED, 0x14))
        self.assertEqual((self.c.top, self.c.relay, self.c.primary_fault), (6, 0, 0x402))
        self.advance(3)
        self.assertFalse(self.c.established)

    def test_stop_before_relay_still_ends_armed_session(self):
        self.ask(sim.START)
        self.ask(sim.STOP)
        codes = [struct.unpack_from("<H", data)[0] for _, data in self.c.events]
        self.assertIn(0x13, codes)
        self.assertIn(0x16, codes)
        self.assertFalse(self.c.session_armed)

    def test_clear_absent_fault_or_inhibit_preserves_charging(self):
        self.c.command("charging")
        self.c.command("fault clear")
        self.c.command("inhibit clear")
        self.assertEqual((self.c.top, self.c.op, self.c.relay), (4, 6, 1))
        self.assertTrue(self.c.established)

    def test_permission_denied_monitoring_allowed(self):
        self.c.command("authority deny")
        for cmd in (sim.START, sim.STOP):
            reply = self.ask(cmd)[0]
            self.assertEqual((header(reply)[0], header(reply)[2]), (sim.REJECTED, 5))
        self.assertEqual(header(self.ask(sim.STATUS)[0])[0], sim.COMPLETED)

    def test_bad_payload_service_and_unknown_commands(self):
        for cmd, payload, reason in ((sim.STATUS, b"x", 2), (sim.START, b"x", 2),
                                     (sim.PING, b"x" * 33, 2), (sim.EVENTS, b"x", 2),
                                     (0x100, b"", 5), (0x7777, b"", 4)):
            reply = self.ask(cmd, payload)[0]
            self.assertEqual((header(reply)[0], header(reply)[2]), (sim.REJECTED, reason))

    def test_accepted_only_keeps_pending(self):
        self.c.command("control accepted-only")
        self.assertEqual(header(self.ask(sim.START)[0])[0], sim.ACCEPTED)
        self.assertEqual(self.advance(16), [])
        self.assertEqual(self.c.relay, 0)
        self.assertIsNotNone(self.c.pending_start)
        self.ask(sim.STOP)
        self.assertIsNone(self.c.pending_start)

    def test_start_failure(self):
        self.c.command("control start-fail")
        self.ask(sim.START)
        reply = self.advance(1.4)[0]
        self.assertEqual((header(reply)[0], header(reply)[2]), (sim.FAILED, 0x12))
        self.assertEqual((self.c.relay, self.c.top), (0, 5))

    def test_stop_unconfirmed_still_changes_state(self):
        self.c.command("charging")
        self.c.command("control stop-unconfirmed")
        self.assertEqual(self.ask(sim.STOP, rid=1), [])
        self.assertEqual(self.advance(0.1), [])
        self.assertEqual(self.ask(sim.STOP, rid=1), [])
        self.assertEqual((self.c.relay, self.c.top), (0, 5))
        self.assertEqual(header(self.ask(sim.STATUS)[0])[0], sim.COMPLETED)

    def test_stop_failed_and_queue_full_still_off(self):
        for mode, result, reason in (("stop-fail", sim.FAILED, 0x0F),
                                     ("stop-queue-full", sim.REJECTED, 7)):
            self.c.command("charging")
            self.c.command("control " + mode)
            replies = self.ask(sim.STOP)
            replies += self.advance(0.1)
            self.assertEqual((header(replies[-1])[0], header(replies[-1])[2]), (result, reason))
            self.assertEqual(self.c.relay, 0)

    def test_duplicate_stop_and_ledger_expiry(self):
        self.ask(sim.STOP, rid=5)
        terminal = self.advance(0.1)[0]
        self.assertEqual(self.ask(sim.STOP, rid=5), [terminal])
        self.advance(2.01)
        self.assertEqual(header(self.ask(sim.STOP, rid=5)[0])[0], sim.ACCEPTED)

    def test_stop_ledger_full(self):
        for rid in range(1, 5):
            self.ask(sim.STOP, rid=rid)
        reply = self.ask(sim.STOP, rid=5)[0]
        self.assertEqual((header(reply)[0], header(reply)[2]), (sim.REJECTED, 7))
        self.assertEqual(self.c.relay, 0)
        self.assertLessEqual(len(self.c.ledger), 4)

    def test_measurement_invalid_stale_floor_missing_and_signed(self):
        for mode in ("invalid", "stale", "floor", "missing"):
            self.c.command("measurement " + mode)
            reply = self.ask(sim.MEASURE)[0]
            if mode == "missing":
                self.assertEqual((header(reply)[0], header(reply)[2]), (sim.FAILED, 12))
            else:
                data = struct.unpack("<IIIiIiHHI", reply.payload[12:])
                if mode == "stale":
                    self.assertEqual(data[1], 2000)
                    self.assertFalse(data[6] & 2)
                if mode == "invalid":
                    self.assertNotEqual(data[6] & 63, 63)
                if mode == "floor":
                    self.assertEqual((data[2], data[6]), (3380, 0x7F))
            rejected = self.ask(sim.START)[0]
            self.assertEqual(header(rejected)[0], sim.REJECTED)
        self.c.command("measurement valid")
        self.c.command("values 48000 -500")
        self.c.command("charging")
        data = self.ask(sim.MEASURE)[0].payload[12:]
        self.assertEqual(struct.unpack_from("<i", data, 12)[0], -500)

    def test_event_page_three_frames_and_overwrite(self):
        self.c.events.clear()
        self.c.add_event(1, b"\0" * 12)
        for _ in range(3):
            self.c.add_event(0x33, b"\0" * 32)
        reply = self.ask(sim.EVENTS, struct.pack("<IB3x", 0, 4))[0]
        self.assertEqual(len(reply.payload), 252)
        wire = sim.encode_message(reply)
        self.assertEqual(len([b for b in wire.split(b"\0") if b]), 3)
        self.assertEqual(sim.Parser(2, 2).feed(wire), [reply])
        self.c.command("events 40")
        page = self.ask(sim.EVENTS, struct.pack("<IB3x", 1, 4))[0].payload[12:]
        cursor, oldest, missed, count = struct.unpack("<IIIB3x", page[:16])
        self.assertEqual(missed, oldest - 1)
        self.assertEqual((cursor, count), (oldest + 4, 4))

    def test_event_zero_count_means_as_many_as_fit(self):
        self.c.command("events 10")
        page = self.ask(sim.EVENTS, struct.pack("<IB3x", 0, 0))[0].payload[12:]
        self.assertGreater(page[12], 0)
        self.assertLessEqual(len(page), 240)

    def test_reboot_cancels_pending_and_restarts_log(self):
        self.ask(sim.START)
        boot = self.c.boot_id
        self.c.command("reboot")
        self.assertNotEqual(self.c.boot_id, boot)
        self.assertEqual(self.advance(2), [])
        self.assertFalse(self.c.established)
        self.assertEqual(len(self.c.ledger), 0)
        self.assertEqual((len(self.c.events), self.c.events[0][0]), (1, 1))

    def test_link_silence_does_not_stop_model(self):
        self.c.command("charging")
        self.c.command("link silence")
        self.advance(10)
        self.assertTrue(self.c.established)
        self.assertEqual(self.c.relay, 1)

    def test_console_validation(self):
        for command in ("values 60001 1", "values 48000 -10001", "events 1001",
                        "delay -1", "fault xyz", "control xyz"):
            with self.assertRaises(ValueError):
                self.c.command(command)


class ReferenceCodecTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get("RCC_REFERENCE_CODEC_PATH"),
                         "optional independent controller codec path not supplied")
    def test_against_independent_controller_codec(self):
        path = Path(os.environ["RCC_REFERENCE_CODEC_PATH"])
        sys.path.insert(0, str(path.parents[2]))
        reference = importlib.import_module("rcc_calibration_tool.protocol.codec")
        for payload in (b"", b"\0test", bytes(range(248)) * 2):
            reply = sim.Message(sim.EVENTS, 123, payload, 1, 2)
            obj = reference.encode_object(2, sim.EVENTS, 123, payload)
            expected = reference.encode_frames(obj, 123, source=1, destination=2)
            self.assertEqual(sim.encode_message(reply), expected)
            parser = reference.SerialReassembler(host_node=2)
            results = [parser.push_body(body) for body in expected.split(b"\0") if body]
            self.assertEqual(results[-1].payload, payload)


class SerialLoopTests(unittest.TestCase):
    def test_serial_loop_without_hardware(self):
        try:
            import serial
        except ImportError:
            self.skipTest("pyserial not installed; pure protocol/model tests still run")
        class FakePort:
            is_open = False
            def __init__(self, **kwargs):
                self.incoming = bytearray(sim.encode_message(sim.Message(sim.PING, 100, b"hi"), 1, 2))
                self.written = bytearray()
            @property
            def in_waiting(self):
                return len(self.incoming)
            def open(self):
                if self.dtr or self.rts:
                    raise AssertionError("DTR/RTS must be deasserted before open")
                self.is_open = True
            def read(self, count):
                result = bytes(self.incoming[:count])
                del self.incoming[:count]
                time.sleep(0.001)
                return result
            def write(self, wire):
                self.written.extend(wire)
                return len(wire)
            def close(self):
                self.is_open = False
        port = FakePort()
        args = argparse.Namespace(scenario="ready", no_console=True, port="FAKE",
                                  duration=0.03, verbose=False)
        with patch.object(serial, "Serial", return_value=port), contextlib.redirect_stdout(io.StringIO()):
            self.assertEqual(sim.run_serial(args), 0)
        reply = sim.Parser(2, 2).feed(port.written)[0]
        self.assertEqual((reply.request_id, reply.payload[12:]), (100, b"hi"))
        self.assertFalse(port.is_open)


if __name__ == "__main__":
    unittest.main()
