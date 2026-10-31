/*
 * HMI theme: palette (HC_*), fonts (HF_*) and the colour helper HC(). LVGL only; the tone/fault-state to colour
 * mapping is in hmi_theme_present.h. Change colours here, not in the pages.
 */
#pragma once

#include <stdint.h>

#include "lvgl.h"

LV_FONT_DECLARE(hmi_f10);
LV_FONT_DECLARE(hmi_f12);
LV_FONT_DECLARE(hmi_f12b);
LV_FONT_DECLARE(hmi_f14b);
LV_FONT_DECLARE(hmi_f16b);
LV_FONT_DECLARE(hmi_f22b);
LV_FONT_DECLARE(hmi_f28b);
LV_FONT_DECLARE(hmi_hero28);

#define HF_10 (&hmi_f10)
#define HF_12 (&hmi_f12)
#define HF_12B (&hmi_f12b)
#define HF_14B (&hmi_f14b)
#define HF_16B (&hmi_f16b)
#define HF_22B (&hmi_f22b)
#define HF_28B (&hmi_f28b)
#define HF_28H (&hmi_hero28)

/* ===== Modern Industrial Dark Slate Palette ===== */
#define HC_BG          0x090D14   /* Deep slate-black canvas */
#define HC_CARD        0x111722   /* Elevated Bento card surface */
#define HC_SURFACE2    0x172030   /* Secondary inner pill / track surface */
#define HC_LINE        0x1E293B   /* Clean 1px structural border */
#define HC_WHITE       0xF8FAFC   /* Crisp white primary text */
#define HC_TXT2        0xCBD5E1   /* Soft slate secondary text */
#define HC_LABEL       0x94A3B8   /* Muted label */
#define HC_DIM         0x64748B   /* Inactive icon / disabled label */

#define HC_NAV_CUR     0x38BDF8   /* Calm Sky Cyan (Navigation / Active) */
#define HC_NAV_BOX     0x0C2538   /* Muted Cyan background */

#define HC_VOLT_COL    0xFBBF24   /* Telemetry: Volt */
#define VOLT_T      0x2A220B
#define HC_AMP_COL     0xFB923C   /* Telemetry: Current */
#define AMP_T       0x2C1A0D
#define HC_WATT_COL    0x38BDF8   /* Telemetry: Power */
#define WATT_T      0x0C2538

#define HC_OK_COL      0x22C55E   /* Status OK */
#define HC_OK_BG       0x0A2617
#define HC_WARN_COL    0xFACC15   /* Status Warning */
#define HC_WARN_BG     0x2B2309
#define HC_FAULT_COL   0xF43F5E   /* Status Fault */
#define HC_FAULT_BG    0x35101B

#define HC_STOP_COL    0xDC2626   /* E-Stop */
#define HC_START_COL   0x0EA5E9   /* Charge Start */
#define HC_OFF_COL     0x94A3B8
#define HC_OFF_BG      0x161E2B
#define HC_HERO_COL    0xFFFFFF
#define HC_INK         0x06090E   /* Darkest ink: nav rail, text on bright buttons, modal scrim */
#define HC_NAV_EDGE    0x1E4963   /* Border of HC_NAV_BOX chips and buttons */
#define HC_TRACK       0x2B3648   /* Inactive bar / ring track */

/* One-off tints of the status colours (same hue family, darker). */
#define HC_STOPPING_BG 0x4A1818   /* STOPPING button fill */
#define HC_STOPPING_FG 0xFCA5A5   /* STOPPING button text and spinner */
#define HC_AMBER       0xF59E0B   /* Warning text on the OTA overlay */
#define HC_OTA_BG      0x070D19   /* OTA overlay fill */
#define HC_OTA_EDGE    0x0284C7   /* OTA overlay border */
#define HC_OK_DEEP     0x0F3A22   /* Icon disc / chip on an OK_BG card */
#define HC_FAULT_DEEP  0x4A1624   /* Icon disc on a FAULT_BG card */
#define HC_PRESS_BORDER 0x334155  /* Border of a tappable card while pressed */

#define HC(x) lv_color_hex(x)
