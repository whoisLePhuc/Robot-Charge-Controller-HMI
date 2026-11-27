Generated with `lv_font_conv@1.5.3` (4 bpp, no compression) from Montserrat (the weights embedded in the
Bento v3.2 concept HTML) plus DejaVu Sans for U+2264 (≤) and U+2192 (→), which Montserrat lacks.
Range: `0x20-0x7E,0xB0,0xB1,0xB7,0x2014,0x2022,0x2026,0x2212`.

| Font | Size | Weight | Used for |
|---|---|---|---|
| hmi_f10 | 10 | 600 | labels, chips, rail text, event details |
| hmi_f12 / hmi_f12b | 12 | 500 / 700 | body rows / bold values, header, bar titles |
| hmi_f14b | 14 | 700 | page titles, split START/STOP, units |
| hmi_f16b | 16 | 800 | full-width STOP/START, modal title |
| hmi_f22b / hmi_f28b | 22 / 28 | 800 | telemetry and Measure values (digits . - — ≤ only) |
| hmi_hero28 | 28 | 900 italic | state name (A-Z _ - space) |

Example: `lv_font_conv --bpp 4 --size 10 --no-compress --format lvgl --lv-include lvgl.h
--lv-font-name hmi_f10 -o hmi_f10.c --font Montserrat-SemiBold.ttf -r 0x20-0x7E,0xB0,0xB1,0xB7,0x2014,0x2022,0x2026,0x2212
--font DejaVuSans.ttf -r 0x2264,0x2192`
