#!/bin/sh
# Build the simulator, render every scenario/page/overlay and compare pixel-for-pixel with a baseline.
#   tools/sim/check.sh baseline   # record /tmp/hmi-baseline from the CURRENT code (run it on the pre-change commit)
#   tools/sim/check.sh            # render /tmp/hmi-after and diff against the baseline
# /tmp is volatile: after a reboot, check out the pre-change commit and record the baseline again.
# Works on Linux and on Windows Git Bash + WinLibs (mingw32-make, gcc).
set -e
cd "$(dirname "$0")"
T=$(cygpath -m /tmp 2>/dev/null || echo /tmp)
if [ "$1" != baseline ] && [ ! -d "$T/hmi-baseline" ]; then
    echo "check.sh: no baseline in $T/hmi-baseline." >&2
    echo "  Check out the commit to compare against (the original UI), run 'sh check.sh baseline', then come back." >&2
    exit 2
fi
MAKE=$(command -v make || command -v mingw32-make)
FIND=/usr/bin/find
L=../../managed_components/lvgl__lvgl
LVSRC=$($FIND $L/src -name '*.c' -not -path '*/drivers/*' -not -path '*/libs/*' -not -path '*/others/vg_lite_tvg/*' | tr '\n' ' ')
"$MAKE" -j8 CC="${CC:-gcc}" OUT=/tmp/hmi-sim LVSRC="$LVSRC" >/tmp/hmi-sim-build.log 2>&1 || { echo "check.sh: simulator build failed:" >&2; tail -30 /tmp/hmi-sim-build.log >&2; exit 1; }
BIN=/tmp/hmi-sim/hmi_sim; [ -f $BIN.exe ] && BIN=$BIN.exe
PY=python; "$PY" -c "import PIL" 2>/dev/null || PY=python3
"$PY" -c "import PIL" 2>/dev/null || { echo "check.sh: python with Pillow is required (pip install pillow)" >&2; exit 1; }
render() {   # render <outdir>: all PNGs; show the log if the simulator or render.py fails
    rm -rf "$1"
    HMI_SIM=$BIN "$PY" render.py "$1" --scale 1 >/tmp/hmi-sim-render.log 2>&1 || { echo "check.sh: render failed:" >&2; tail -20 /tmp/hmi-sim-render.log >&2; exit 1; }
}
if [ "$1" = baseline ]; then
    render $T/hmi-baseline; echo "baseline recorded ($(ls $T/hmi-baseline | grep -c '\.png$') images)"; exit 0
fi
render $T/hmi-after
T="$T" "$PY" - <<'EOF'
import os, sys
from PIL import Image, ImageChops
T = os.environ["T"]; a, b = T + "/hmi-baseline", T + "/hmi-after"
bad = []
base = [f for f in sorted(os.listdir(a)) if f.endswith(".png")]
for f in base:
    if not os.path.exists(os.path.join(b, f)): bad.append(f + " (missing)"); continue
    if ImageChops.difference(Image.open(os.path.join(a, f)).convert("RGB"), Image.open(os.path.join(b, f)).convert("RGB")).getbbox():
        bad.append(f)
extra = [f for f in os.listdir(b) if f.endswith(".png") and f not in base]
print("PIXEL DIFF:", ", ".join(bad) if bad else "none (%d images identical)" % len(base))
if extra: print("note: %d rendered images have no baseline and were not compared (e.g. %s); re-record the baseline from the original commit to cover them" % (len(extra), extra[0]))
sys.exit(1 if bad else 0)
EOF
