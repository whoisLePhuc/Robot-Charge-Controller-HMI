#!/usr/bin/env python3
"""Render the real hmi_ui.c on the PC. Usage: render.py [OUT_DIR] [--scale N]
Writes one PNG per (scenario, page) plus overlays, and sheet-<scenario>.png contact sheets."""
import os, subprocess, sys
from PIL import Image, ImageDraw

HERE = os.path.dirname(os.path.abspath(__file__))
BIN = os.environ.get("HMI_SIM", "/tmp/hmi-sim/hmi_sim")
OUT = sys.argv[1] if len(sys.argv) > 1 and not sys.argv[1].startswith("--") else os.path.join(HERE, "out")
SCALE = int(sys.argv[sys.argv.index("--scale") + 1]) if "--scale" in sys.argv else 2
os.makedirs(OUT, exist_ok=True)
SCEN = ["CHARGING", "IDLE", "PRECHECK", "INHIBITED", "FAULT", "OFFLINE", "REMOTE"]
PAGES = ["overview", "measure", "faults", "session", "device"]
OVL = [("IDLE", "confirm", ["click=confirm"]), ("CHARGING", "stopping", ["cmd=stopping"]),
       ("REMOTE", "stopped", ["cmd=stopped"]), ("CHARGING", "queuefull", ["cmd=queuefull"]), ("IDLE", "noauth", ["cmd=noauth"]),
       ("CHARGING", "timeout", ["cmd=timeout"]), ("PRECHECK", "accepted", ["cmd=accepted"])]

def render(name, scen, page, extra):
    ppm = os.path.join(OUT, name + ".ppm")
    subprocess.run([BIN, scen, str(page), ppm] + extra, check=True)
    im = Image.open(ppm).convert("RGB"); os.remove(ppm)
    if SCALE != 1: im = im.resize((480 * SCALE, 320 * SCALE), Image.NEAREST)
    im.save(os.path.join(OUT, name + ".png")); return im

sheets = {}
for s in SCEN:
    ims = [render(f"{s}-{p}", s, i, []) for i, p in enumerate(PAGES)]
    ims.append(render(f"{s}-device-link", s, 4, ["tab=link"]))
    sheets[s] = ims
for s, o, extra in OVL:
    render(f"ovl-{o}", s, 0, extra)
for s, ims in sheets.items():                      # contact sheet: 3 columns x 2 rows
    w, h = ims[0].size; sheet = Image.new("RGB", (w * 3 + 40, h * 2 + 30), (42, 42, 48))
    for i, im in enumerate(ims): sheet.paste(im, (10 + (i % 3) * (w + 10), 10 + (i // 3) * (h + 10)))
    sheet.save(os.path.join(OUT, f"sheet-{s}.png"))
print("done", OUT)
