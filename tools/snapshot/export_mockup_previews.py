"""High-resolution README previews from docs/mockups/hmi-ui-landscape.html.

The mockup is a browser port of firmware/src/hmi_ui.c (checked against the LVGL host renders of
tools/snapshot), so it can be captured at any pixel density with sharp text. Writes assets/ui/*.png.

Usage (from the repository root):
    python -m pip install playwright
    python tools/snapshot/export_mockup_previews.py [--scale 3] [--browser msedge|chrome|chromium]
"""
import argparse
import pathlib

from playwright.sync_api import sync_playwright

ROOT = pathlib.Path(__file__).resolve().parents[2]
MOCKUP = ROOT / "docs" / "mockups" / "hmi-ui-landscape.html"
OUT = ROOT / "assets" / "ui"

# name -> mockup query string (scenario, page, device tab, overlay)
SHOTS = {
    "overview": "s=CHARGING&p=0",
    "measure": "s=CHARGING&p=1",
    "faults": "s=INHIBITED%20(bench)&p=2",
    "session": "s=CHARGING&p=3",
    "device": "s=CHARGING&p=4&t=0",
    "settings": "s=CHARGING&p=4&t=2",
    "wifi": "s=IDLE&o=wifi-list",
}

parser = argparse.ArgumentParser()
parser.add_argument("--scale", type=float, default=3, help="pixels per UI pixel (3 -> 1440x960)")
parser.add_argument("--browser", default="msedge", help="Playwright channel; 'chromium' uses the bundled browser")
args = parser.parse_args()

OUT.mkdir(parents=True, exist_ok=True)
with sync_playwright() as p:
    launch = {} if args.browser == "chromium" else {"channel": args.browser}
    browser = p.chromium.launch(**launch)
    page = browser.new_page(device_scale_factor=args.scale)
    errors = []
    page.on("pageerror", lambda e: errors.append(str(e)))
    for name, query in SHOTS.items():
        page.goto(f"{MOCKUP.as_uri()}?z=1&{query}")
        page.wait_for_function('document.body.dataset.ready === "1"')
        # Square the 6 px rounded bezel corners of the preview frame.
        page.add_style_tag(content=".scr{border-radius:0!important;box-shadow:none!important}")
        page.locator("#scr").screenshot(path=str(OUT / f"{name}.png"))
        print(OUT / f"{name}.png")
    browser.close()
if errors:
    raise SystemExit("JavaScript errors: " + "; ".join(errors))
