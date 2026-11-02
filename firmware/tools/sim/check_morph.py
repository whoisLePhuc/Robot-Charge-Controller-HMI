#!/usr/bin/env python3
"""Update-vs-fresh-build proof for retained pages. Usage: check_morph.py FRESH_DIR (PNGs from render.py).
1. For every ordered scenario pair (A, B) and every retained page: show A, switch the snapshot to B live, let the 1 s
   refresh update the widgets in place, and require the frame to equal a fresh render of B (and the first frame A).
   Also A -> A+aged (only age texts change) and A -> A+evt (one more event) for the same pages.
   Also A -> A+sim / A+simrdy / A+stale (flags no canned scenario varies).
2. Scroll survival: scroll a list on MANYFAULT, then change the snapshot live (+aged: texts only, +evt: the event list
   changes shape, +nopri: Faults body class, MANYFAULT+sim <-> MANYFAULT: the banner height changes and the page is re-entered) and require the frame to equal a fresh render of that snapshot scrolled the same way.
3. Negative control: the comparison must be able to fail: for every page some pair of scenarios must differ in pixels,
   otherwise a missed update could not be seen."""
import os, subprocess, sys, tempfile
from concurrent.futures import ThreadPoolExecutor
from PIL import Image, ImageChops

BIN = os.environ.get("HMI_SIM", "/tmp/hmi-sim/hmi_sim")
FRESH = sys.argv[1]
SFX = ("+aged", "+evt", "+sim", "+simrdy", "+stale")
SCEN = ["CHARGING", "IDLE", "PRECHECK", "INHIBITED", "FAULT", "OFFLINE", "REMOTE", "MANYFAULT"]
MIGRATED = {"overview": 0, "measure": 1, "faults": 2, "session": 3}   # page name -> registry index (render.py order)
TMP = tempfile.mkdtemp(prefix="hmi-morph-")

def load(p): return Image.open(p).convert("RGB")
def same(a, b): return not ImageChops.difference(load(a), load(b)).getbbox()
def sim(args, out):
    r = subprocess.run([BIN] + args, capture_output=True)
    if r.returncode: raise RuntimeError("simulator failed: " + " ".join(args))
    return out

_fresh = {}
def fresh(scen, page, idx):
    """Fresh render of scen on page: the render.py PNG when it exists, else rendered here."""
    png = os.path.join(FRESH, f"{scen}-{page}.png")
    if os.path.exists(png): return png
    key = (scen, page)
    if key not in _fresh: _fresh[key] = sim([scen, str(idx), os.path.join(TMP, f"fresh-{scen}-{page}.ppm")], os.path.join(TMP, f"fresh-{scen}-{page}.ppm"))
    return _fresh[key]

def one(job):
    page, idx, a, b = job
    out = os.path.join(TMP, f"{page}-{a}-{b}.ppm")
    try:
        sim([a, str(idx), out, "seq=" + b], out)
        bad = []
        if not same(out, fresh(a, page, idx)): bad.append("first frame")
        if not same(out + ".1", fresh(b, page, idx)): bad.append("after update")
    except RuntimeError as e: return f"{page} {a}->{b}: {e}"
    for f in (out, out + ".1"): os.remove(f)
    return f"{page} {a}->{b}: " + ", ".join(bad) if bad else None

DRAG = {"faults": "drag=164,250,180", "session": "drag=200,250,170"}   # scroll the first list / the event list up
def scroll_job(job):
    page, a, b, big = job
    idx = MIGRATED[page]
    drag = DRAG[page].rsplit(",", 1)[0] + ",30" if big else DRAG[page]   # big: drag to the end of the list in both layouts
    tag = f"{page} {a}->{b}"
    out = os.path.join(TMP, f"scroll-{page}-{a}-{b}.ppm"); ref = out + "-ref.ppm"
    try:
        sim([a, str(idx), out, "click=" + drag, "seq=" + b], out)
        sim([b, str(idx), ref, "click=" + drag], ref)
    except RuntimeError as e: return f"scroll {tag}: {e}"
    bad = []
    if same(out, fresh(a, page, idx)): bad.append("the drag did not scroll (test is vacuous)")
    if not same(out + ".1", ref): bad.append("offset lost or content differs after the in-place update")
    return f"scroll {tag}: " + ", ".join(bad) if bad else None

with ThreadPoolExecutor(8) as ex:             # render the fresh references first (the workers then only read them)
    list(ex.map(lambda t: fresh(*t), [(x + sfx, p, i) for p, i in MIGRATED.items() for x in SCEN for sfx in ("",) + SFX]))
jobs = [(p, i, a, b) for p, i in MIGRATED.items() for a in SCEN for b in SCEN if a != b]
jobs += [(p, i, a, a + sfx) for p, i in MIGRATED.items() for a in SCEN for sfx in SFX]
with ThreadPoolExecutor(8) as ex:
    bad = [r for r in ex.map(one, jobs) if r]
    sj = [(p, "MANYFAULT", "MANYFAULT" + sfx, False) for p in DRAG for sfx in ("+aged", "+evt", "+nopri", "+quiet")]
    sj += [(p, "MANYFAULT+sim", "MANYFAULT", True) for p in DRAG]   # banner shrinks: the page is re-entered, the offset must be restored (clamped)
    bad += [r for r in ex.map(scroll_job, sj) if r]
for p, i in MIGRATED.items():                 # negative control
    if not any(not same(fresh(a, p, i), fresh(b, p, i)) for a in SCEN for b in SCEN if a != b):
        bad.append(f"control {p}: no scenario pair changes the pixels, so the update check proves nothing")
for f in os.listdir(TMP): os.remove(os.path.join(TMP, f))
os.rmdir(TMP)
print("MORPH DIFF:", "\n  ".join(bad) if bad else "none (%d update transitions + %d scroll checks identical to a fresh build)" % (len(jobs), len(sj)))
sys.exit(1 if bad else 0)
