/*
 * Measure page (retained): VOUT/IOUT channel cards and the ADC quality matrix. Widgets are created once in enter();
 * the model subjects (hm_chan_v/i, hm_adc) redraw them when the data changes.
 */
#include <string.h>

#include "hmi_data_widgets.h"
#include "hmi_icons.h"
#include "hmi_model.h"
#include "hmi_pages.h"
#include "hmi_present.h"
#include "hmi_theme_present.h"
#include "hmi_widgets.h"

#include "hmi_lvgl_headers.h"
#include "rcc_measure_channel_gen.h"
#include "rcc_measure_floor_badge_gen.h"
#include "rcc_measure_matrix_gen.h"
#include "rcc_measure_quality_badge_gen.h"
#include "rcc_measure_row_heading_gen.h"
#include "rcc_measure_text_cell_gen.h"

typedef struct {
    hw_chip_t chip;
    lv_obj_t *val, *unit, *filt;
    int chip_edge, value_x, unit_gap;
    lv_color_t hue;
} chan_ui_t;

/* The matrix cells are rebuilt (inside `cells` only) when the bits change: keeping every alternative as a hidden
 * widget would cost ~8 KB of the LVGL heap. */
static struct {
    lv_obj_t *seq, *cells;
    int header_y, column_x[7], column_width[7];
    hp_adc_view_t built;
    bool have_built;
} adc;
static chan_ui_t chan_v, chan_i;

static void apply_chan(lv_observer_t *o, lv_subject_t *s)
{
    chan_ui_t *u = lv_observer_get_user_data(o);
    const hp_chan_view_t *v = lv_subject_get_pointer(s);
    hw_chip_set(&u->chip, u->chip_edge, hp_chan_state_text(v->st), ht_chan_color(v->st), HC_SURFACE2);
    hw_label_set(u->val, v->value);
    lv_obj_set_style_text_color(u->val, v->valid ? u->hue : HC(HC_DIM), 0);
    const int vw = hw_width_of(u->val);
    lv_obj_set_hidden(u->unit, !v->valid);
    lv_obj_set_x(u->unit, u->value_x + vw + u->unit_gap);
    hw_label_set(u->filt, v->filtered);
}

static void channel_card(chan_ui_t *u, lv_obj_t *parent, lv_subject_t *subject, int x, const char *title,
                         const char *limit, uint32_t hue, const char *unit, int limit_x, int unit_x,
                         const char *sample)
{
    lv_obj_t *card = rcc_measure_channel_create(parent, x, title, limit, unit, HC(hue), limit_x,
                                               sample, "", unit_x, HMI_DATA_FONT(HF_10),
                                               HMI_DATA_FONT(HF_28B), HMI_DATA_FONT(HF_14B));
    u->chip.box = hmi_data_widget(card, "channel_badge");
    u->chip.label = hmi_data_widget(card, "channel_status");
    u->val = hmi_data_widget(card, "channel_value");
    u->unit = hmi_data_widget(card, "channel_unit");
    u->filt = hmi_data_widget(card, "channel_filtered");
    const int sample_width = hw_width_of(u->val);
    u->chip.y = lv_obj_get_y_aligned(u->chip.box);
    u->chip_edge = lv_obj_get_x_aligned(u->chip.box) + lv_obj_get_width(u->chip.box);
    u->value_x = lv_obj_get_x_aligned(u->val);
    u->unit_gap = lv_obj_get_x_aligned(u->unit) - u->value_x - sample_width;
    u->hue = lv_obj_get_style_text_color(u->val, 0);
    lv_subject_add_observer_obj(subject, apply_chan, card, u);
}

static void build_cells(lv_obj_t *matrix, const hp_adc_view_t *v)
{
    for (int row = 0; row < 2; ++row) {
        const uint16_t bits = row ? v->ibits : v->vbits;
        const int y = adc.header_y + 17 + row * 23;
        rcc_measure_row_heading_create(matrix, y + 3, row ? "IOUT" : "VOUT", HMI_DATA_FONT(HF_12B),
                                       HC(row ? HC_AMP_COL : HC_VOLT_COL));
        for (int i = 0; i < 7; ++i) {
            const int x = adc.column_x[i], width = adc.column_width[i];
            const bool set = (bits >> i) & 1u;
            if (!v->have || (i == 6 && !set)) {
                rcc_measure_text_cell_create(matrix, x, y + 3, width, v->have ? "-" : "—",
                                             HMI_DATA_FONT(HF_10), HC(HC_DIM));
            } else if (i == 6) {
                const int chip_width = hmi_data_ascii_width("LOW", HF_10, 0) + 14;
                rcc_measure_floor_badge_create(matrix, x + width / 2 + 19 - chip_width, y + 1,
                                               chip_width, HMI_DATA_FONT(HF_10));
            } else {
                const lv_image_dsc_t *mark = set ? &hmi_icon_check_11 : &hmi_icon_cross_10;
                const int badge_x = x + (width - 18) / 2;
                rcc_measure_quality_badge_create(matrix, badge_x, y + 2,
                                                 x + width / 2 - mark->header.w / 2 - badge_x,
                                                 8 - mark->header.h / 2, mark,
                                                 HC(set ? HC_OK_COL : HC_FAULT_COL),
                                                 HC(set ? HC_OK_BG : HC_FAULT_BG));
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
    channel_card(&chan_v, p, hm_chan_v, 0, "VOUT", "· ≤" HP_V_FULL_TXT, HC_VOLT_COL, "V", 51, 81, "48.2");
    channel_card(&chan_i, p, hm_chan_i, 208, "IOUT", "· ±" HP_I_FULL_TXT, HC_AMP_COL, "A", 47, 63, "8.0");

    lv_obj_t *matrix = rcc_measure_matrix_create(p, ctx->height - 82, HMI_DATA_FONT(HF_10), 154);
    static const char *const column_names[] = {
        "matrix_header_0", "matrix_header_1", "matrix_header_2", "matrix_header_3",
        "matrix_header_4", "matrix_header_5", "matrix_header_6",
    };
    adc.seq = hmi_data_widget(matrix, "matrix_seq");
    adc.cells = hmi_data_widget(matrix, "matrix_cells");
    adc.have_built = false;
    lv_obj_update_layout(matrix);
    for (int i = 0; i < 7; ++i) {
        lv_obj_t *header = hmi_data_widget(matrix, column_names[i]);
        adc.column_x[i] = lv_obj_get_x_aligned(header);
        adc.column_width[i] = lv_obj_get_width(header);
        if (i == 0) adc.header_y = lv_obj_get_y_aligned(header);
    }
    lv_subject_add_observer_obj(hm_adc, apply_adc, matrix, NULL);
}

static void leave(void) { memset(&adc, 0, sizeof(adc)); memset(&chan_v, 0, sizeof(chan_v)); memset(&chan_i, 0, sizeof(chan_i)); }

const hmi_page_ops_t page_measure_ops = {.enter = enter, .update = NULL, .leave = leave};
