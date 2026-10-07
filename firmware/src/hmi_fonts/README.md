Generated with `lv_font_conv@1.5.3` (4 bpp, no compression) from Montserrat (the weights used by
docs/mockups/hmi-ui-landscape.html) plus DejaVu Sans for U+2264 (≤) and U+2192 (→), which
Montserrat lacks. Range: `0x20-0x7E,0xB0,0xB7,0x2014,0x2022,0x2026,0x2212`.

| Font | Size | Weight | Used for |
|---|---|---|---|
| hmi_f10 | 10 | 600 | rail labels |
| hmi_f12 / hmi_f12b | 12 | 500 / 700 | body, bold labels (+ ≤ →) |
| hmi_f14b | 14 | 700 | titles, messages |
| hmi_f16b | 16 | 800 | modal title, split STOP |
| hmi_f20b | 20 | 800 | full-width STOP/START, last-reply age |
| hmi_f22b / hmi_f28b | 22 / 28 | 800 | ring values, Measure values (digits . - — ≤ only) |
| hmi_hero28/40/48 | 28/40/48 | 900 italic | state name (A-Z _ - space) |

Example: `lv_font_conv --bpp 4 --size 12 --no-compress --format lvgl --lv-include lvgl.h
--lv-font-name hmi_f12 -o hmi_f12.c --font Montserrat-Medium.ttf -r 0x20-0x7E,0xB0,0xB7,0x2014,0x2022,0x2026,0x2212
--font DejaVuSans.ttf -r 0x2264,0x2192`
