#!/usr/bin/env python3
"""Verify that taps in the rendered HMI reach the expected simulator callbacks."""

import os
from pathlib import Path
import subprocess
import sys
from tempfile import TemporaryDirectory

from PIL import Image, ImageChops


def main():
    executable = os.environ.get("HMI_SIM", "/tmp/hmi-sim/hmi_sim")
    reference = Path(sys.argv[1])
    cases = (
        ("IDLE", "confirm", "0,0"),
        ("IDLE", "send-start", "1,0"),
        ("IDLE", "cancel-start", "0,0"),
        ("CHARGING", "stop", "0,1"),
    )
    with TemporaryDirectory(prefix="hmi-controls-") as work:
        for scenario, action, counts in cases:
            path = Path(work) / "screen.ppm"
            subprocess.run([
                executable, scenario, "0", str(path),
                f"click={action}", f"expect_requests={counts}",
            ], check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            if action == "cancel-start":
                with Image.open(path) as actual, Image.open(reference / "IDLE-overview.png") as expected:
                    if ImageChops.difference(actual.convert("RGB"), expected.convert("RGB")).getbbox():
                        raise RuntimeError("Cancel did not close the START confirmation")
    print(f"COMMAND TAPS: {len(cases)} checks passed")


if __name__ == "__main__":
    main()
