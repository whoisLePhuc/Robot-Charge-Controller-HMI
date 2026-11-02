/*
 * Measure page (retained): VOUT/IOUT channel cards and the ADC quality matrix. Widgets are created once in enter();
 * the model subjects (hm_chan_v/i, hm_adc) redraw them when the data changes.
 */
#include <string.h>

#include "hmi_icons.h"
#include "hmi_model.h"
#include "hmi_pages.h"
#include "hmi_present.h"
#include "hmi_theme_present.h"
#include "hmi_widgets.h"

typedef struct {
    hw_chip_t chip;
    lv_obj_t *val, *unit, *filt;
    int w;                                  /* card width (chip right edge is w - 13) */
    uint32_t hue;
} chan_ui_t;

/* The matrix cells are rebuilt (inside `cells` only) when the bits change: keeping every alternative as a hidden
 * widget would cost ~8 KB of the 64 KB LVGL heap. */
static struct { lv_obj_t *seq, *cells; int my; hp_adc_view_t built; bool have_built; } adc;
static chan_ui_t chan_v, chan_i;

static void apply_chan(lv_observer_t *o, lv_subject_t *s)
{
    chan_ui_t *u = lv_observer_get_user_data(o);
    const hp_chan_view_t *v = lv_subject_get_pointer(s);
    hw_chip_set(&u->chip, u->w - 13, hp_chan_state_text(v->st), ht_chan_color(v->st), HC_SURFACE2);
    hw_label_set(u->val, v->value);
    lv_obj_set_style_text_color(u->val, HC(v->valid ? u->hue : HC_DIM), 0);
    const int vw = hw_width_of(u->val);
    lv_obj_set_pos(u->val, 13, 23 + (33 - hw_lh(HF_28B)) / 2);
    lv_obj_set_hidden(u->unit, !v->valid);
    lv_obj_set_x(u->unit, 13 + vw + 4);
    hw_label_set(u->filt, v->filtered);
}

static void channel_card(chan_ui_t *u, lv_obj_t *p, lv_subject_t *subj, int x, int y, int w, int h, const char *title,
                         const char *lim, uint32_t hue, const char *unit)
{
    lv_obj_t *c = hw_card_box(p, x, y, w, h, HC_CARD, 10, HC_LINE);
    lv_obj_t *tt = hw_text(c, title, 13, 7 + (16 - hw_lh(HF_10)) / 2, HF_10, HC_LABEL);
    hw_spacing(tt, 1);
    const int tw = hw_width_of(tt);
    hw_text(c, lim, 13 + tw + 6, 7 + (16 - hw_lh(HF_10)) / 2, HF_10, HC_DIM);
    u->w = w; u->hue = hue;
    hw_chip_create(&u->chip, c, 7);
    u->val = hw_text(c, "", 13, 0, HF_28B, HC_DIM);
    u->unit = hw_text(c, unit, 13, 33, HF_14B, HC_LABEL);
    u->filt = hw_text_in(c, "", 13, 56, w - 26, 13, HF_10, HC_TXT2, LV_TEXT_ALIGN_LEFT);
    lv_subject_add_observer_obj(subj, apply_chan, c, u);
}

static void build_cells(lv_obj_t *mx, const hp_adc_view_t *v)
{
    const int c0 = 13 + 46;
    const int cw6 = (382 - 96) * 100 / 6;           /* 1fr in hundredths of a pixel */
    for (int r = 0; r < 2; ++r) {
        const uint16_t bits = r ? v->ibits : v->vbits;
        const int ry = adc.my + 18 + r * 23;
        hw_text_in(mx, r ? "IOUT" : "VOUT", 13, ry, 46, 20, HF_12B, r ? HC_AMP_COL : HC_VOLT_COL, LV_TEXT_ALIGN_LEFT);
        for (int i = 0; i < 7; ++i) {
            const int cx = i < 6 ? c0 + (cw6 * i) / 100 : c0 + (cw6 * 6) / 100 + 3;
            const int w = i < 6 ? cw6 / 100 : 47;
            if (!v->have) { hw_text_in(mx, "\xE2\x80\x94", cx, ry, w, 20, HF_10, HC_DIM, LV_TEXT_ALIGN_CENTER); continue; }
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

static void apply_adc(lv_observer_t *o, lv_subject_t *s)
{
    (void)o;
    const hp_adc_view_t *v = lv_subject_get_pointer(s);
    hw_label_set(adc.seq, v->seq);
    if (adc.have_built && adc.built.have == v->have && adc.built.vbits == v->vbits && adc.built.ibits == v->ibits) return;
    lv_obj_clean(adc.cells);
    build_cells(adc.cells, v);
    adc.built = *v; adc.have_built = true;
}

static void enter(lv_obj_t *p, const rcc_snapshot_t *m, const hmi_page_ctx_t *ctx)
{
    (void)m;
    const int H = ctx->height;
    channel_card(&chan_v, p, hm_chan_v, 0, 0, 200, 76, "VOUT", "\xC2\xB7 \xE2\x89\xA4" HP_V_FULL_TXT, HC_VOLT_COL, "V");
    channel_card(&chan_i, p, hm_chan_i, 208, 0, 200, 76, "IOUT", "\xC2\xB7 \xC2\xB1" HP_I_FULL_TXT, HC_AMP_COL, "A");

    const int ch = H - 82;
    lv_obj_t *mx = hw_card_box(p, 0, 82, 408, ch, HC_CARD, 10, HC_LINE);
    lv_obj_t *l = hw_text(mx, "ADC QUALITY MATRIX", 13, 7 + (16 - hw_lh(HF_10)) / 2, HF_10, HC_LABEL);
    hw_spacing(l, 1);
    const int lw = hw_width_of(l) + 8;
    adc.seq = hw_text_in(mx, "", 13 + lw, 7, 382 - lw, 16, HF_10, HC_TXT2, LV_TEXT_ALIGN_RIGHT);

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

    adc.my = my; adc.have_built = false;
    adc.cells = hw_obj(mx, 0, 0, 408, ch);          /* children use the card's outer-corner coordinates, as in mx */
    lv_subject_add_observer_obj(hm_adc, apply_adc, mx, NULL);
}

static void leave(void) { memset(&adc, 0, sizeof(adc)); memset(&chan_v, 0, sizeof(chan_v)); memset(&chan_i, 0, sizeof(chan_i)); }

const hmi_page_ops_t page_measure_ops = {.enter = enter, .update = NULL, .leave = leave};
