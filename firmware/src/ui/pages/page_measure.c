/*
 * Measure page: VOUT/IOUT channel cards and the ADC quality matrix.
 */
#include <inttypes.h>
#include <stdio.h>

#include "hmi_icons.h"
#include "hmi_pages.h"
#include "hmi_present.h"
#include "hmi_widgets.h"

static void channel_card(lv_obj_t *p, int x, int y, int w, int h, const char *title, const char *lim, uint32_t hue,
                         bool valid, bool floor_bit, bool stale, int32_t value, int32_t filtered, const char *unit)
{
    lv_obj_t *c = hw_card_box(p, x, y, w, h, HC_CARD, 10, HC_LINE);
    const char *st = stale ? "STALE" : valid ? (floor_bit ? "BELOW FLOOR" : "VALID") : "INVALID";
    const uint32_t sc = stale ? HC_OFF_COL : valid ? (floor_bit ? HC_WARN_COL : HC_OK_COL) : HC_FAULT_COL;
    lv_obj_t *tt = hw_text(c, title, 13, 7 + (16 - hw_lh(HF_10)) / 2, HF_10, HC_LABEL);
    hw_spacing(tt, 1);
    const int tw = hw_width_of(tt);
    hw_text(c, lim, 13 + tw + 6, 7 + (16 - hw_lh(HF_10)) / 2, HF_10, HC_DIM);
    hw_chip(c, w - 13, 7, st, sc, HC_SURFACE2);

    char vb[32], fb[56];
    if (valid) hp_fmt_milli(vb, sizeof(vb), value, floor_bit);
    else snprintf(vb, sizeof(vb), "\xE2\x80\x94");
    lv_obj_t *vl = hw_text(c, vb, 13, 0, HF_28B, valid ? hue : HC_DIM);
    const int vw = hw_width_of(vl);
    lv_obj_set_pos(vl, 13, 23 + (33 - hw_lh(HF_28B)) / 2);
    if (valid) hw_text(c, unit, 13 + vw + 4, 33, HF_14B, HC_LABEL);

    if (valid) {
        char t[24]; hp_fmt_milli(t, sizeof(t), filtered, floor_bit);
        snprintf(fb, sizeof(fb), "Filtered %s %s", t, unit);
    } else {
        snprintf(fb, sizeof(fb), "%s", stale ? "Last value is stale" : "Not valid");
    }
    hw_text_in(c, fb, 13, 56, w - 26, 13, HF_10, HC_TXT2, LV_TEXT_ALIGN_LEFT);
}

void page_measure_build(lv_obj_t *p, const rcc_snapshot_t *m, const hmi_page_ctx_t *ctx)
{
    const int H = ctx->height;
    const bool stale = hp_meas_stale(m);
    channel_card(p, 0, 0, 200, 76, "VOUT", "\xC2\xB7 \xE2\x89\xA4" HP_V_FULL_TXT, HC_VOLT_COL, hp_v_valid(m), hp_v_floor(m), stale,
                 (int32_t)m->vout_mv, (int32_t)m->vout_filtered_mv, "V");
    channel_card(p, 208, 0, 200, 76, "IOUT", "\xC2\xB7 \xC2\xB1" HP_I_FULL_TXT, HC_AMP_COL, hp_i_valid(m), false, stale,
                 m->iout_ma, m->iout_filtered_ma, "A");

    const int ch = H - 82;
    lv_obj_t *mx = hw_card_box(p, 0, 82, 408, ch, HC_CARD, 10, HC_LINE);
    char seq[64], age[24];
    if (m->online && m->measurement_valid) {
        hp_fmt_age(age, sizeof(age), m->measurement_age_ms);
        snprintf(seq, sizeof(seq), "Seq #%" PRIu32 " \xC2\xB7 Age %" PRIu32 " ms \xC2\xB7 Held %s",
                 m->measurement_sequence, m->measurement_sample_age_ms, age);
    } else {
        hp_fmt_age(age, sizeof(age), m->measurement_age_ms);
        snprintf(seq, sizeof(seq), "Seq #%" PRIu32 " \xC2\xB7 Stale (%s)", m->measurement_sequence, age);
    }
    hw_section(mx, 13, 7, 382, "ADC QUALITY MATRIX", seq, HC_TXT2);

    static const char *const hdr[7] = {"PRES", "FRESH", "CAL", "RANGE", "SAT", "PLAUS", "FLOOR"};
    const int my = ch - 68;
    const int c0 = 13 + 46;
    const int cw6 = (382 - 96) * 100 / 6;           /* 1fr in hundredths of a pixel */
    for (int i = 0; i < 7; ++i) {
        const int cx = i < 6 ? c0 + (cw6 * i) / 100 : c0 + (cw6 * 6) / 100 + 3;
        const int w = i < 6 ? cw6 / 100 : 47;
        hw_text_in(mx, hdr[i], cx, my, w, 15, HF_10, HC_LABEL, LV_TEXT_ALIGN_CENTER);
    }
    hw_box(mx, c0 + (cw6 * 6) / 100, my, 1, 61, HC_LINE, 0);

    const bool have = m->online && m->measurement_valid;
    for (int r = 0; r < 2; ++r) {
        const uint16_t bits = r ? m->iout_status : m->vout_status;
        const int ry = my + 18 + r * 23;
        hw_text_in(mx, r ? "IOUT" : "VOUT", 13, ry, 46, 20, HF_12B, r ? HC_AMP_COL : HC_VOLT_COL, LV_TEXT_ALIGN_LEFT);
        for (int i = 0; i < 7; ++i) {
            const int cx = i < 6 ? c0 + (cw6 * i) / 100 : c0 + (cw6 * 6) / 100 + 3;
            const int w = i < 6 ? cw6 / 100 : 47;
            if (!have) { hw_text_in(mx, "\xE2\x80\x94", cx, ry, w, 20, HF_10, HC_DIM, LV_TEXT_ALIGN_CENTER); continue; }
            const bool set = (bits >> i) & 1u;
            if (i == 6) {
                if (set) hw_chip(mx, cx + w / 2 + 19, ry + 1, "LOW", HC_WARN_COL, HC_WARN_BG);
                else hw_text_in(mx, "-", cx, ry, w, 20, HF_10, HC_DIM, LV_TEXT_ALIGN_CENTER);
            } else {
                hw_box(mx, cx + (w - 18) / 2, ry + 2, 18, 16, set ? HC_OK_BG : HC_FAULT_BG, 4);
                hw_icon_c(mx, set ? &hmi_icon_check_11 : &hmi_icon_cross_10, cx + w / 2, ry + 10, set ? HC_OK_COL : HC_FAULT_COL);
            }
        }
    }
}
