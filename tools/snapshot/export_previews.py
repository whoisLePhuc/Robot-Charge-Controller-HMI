"""Upscale the hmi_render BMPs to the 960x640 README previews in assets/ui/.

Usage (from the repository root, after building tools/snapshot):
    hmi_render <bmp-dir>
    python tools/snapshot/export_previews.py <bmp-dir>
"""
import pathlib
import sys

from PIL import Image

NAMES = ["overview", "measure", "faults", "session", "device", "settings", "wifi"]

src = pathlib.Path(sys.argv[1] if len(sys.argv) > 1 else ".")
dst = pathlib.Path(__file__).resolve().parents[2] / "assets" / "ui"
dst.mkdir(parents=True, exist_ok=True)
for name in NAMES:
    img = Image.open(src / f"{name}.bmp").convert("RGB")
    # Nearest-neighbour keeps the panel's pixels sharp at 2x.
    img.resize((img.width * 2, img.height * 2), Image.NEAREST).save(dst / f"{name}.png", optimize=True)
    print(dst / f"{name}.png")
