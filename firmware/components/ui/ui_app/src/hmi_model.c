/*
 * UI model: owns the subjects and the current view of each (static, no heap). See hmi_model.h.
 * One table row per view: adding a view = a field in views_t, a line in fill(), a row in tbl[], an extern in hmi_model.h.
 */
#include "hmi_model.h"

#include <stddef.h>
#include <string.h>

#include "hmi_present.h"

lv_subject_t *hm_chan_v, *hm_chan_i, *hm_adc, *hm_hero, *hm_sys, *hm_tel[HP_TEL_COUNT], *hm_fault_sum, *hm_fault_pri, *hm_sess;

typedef struct {
    hp_chan_view_t v, i;
    hp_adc_view_t adc;
    hp_hero_view_t hero;
    hp_sys_view_t sys;
    hp_tel_view_t tel[HP_TEL_COUNT];
    hp_fault_sum_view_t fsum;
    hp_fault_pri_view_t fpri;
    hp_sess_view_t sess;
} views_t;

static views_t cur;

static const struct { lv_subject_t **s; size_t off, n; } tbl[] = {
#define ROW(subj, f) {&(subj), offsetof(views_t, f), sizeof(((views_t *)0)->f)}
    ROW(hm_chan_v, v), ROW(hm_chan_i, i), ROW(hm_adc, adc), ROW(hm_hero, hero), ROW(hm_sys, sys),
    ROW(hm_tel[0], tel[0]), ROW(hm_tel[1], tel[1]), ROW(hm_tel[2], tel[2]),
    ROW(hm_fault_sum, fsum), ROW(hm_fault_pri, fpri), ROW(hm_sess, sess),
#undef ROW
};
_Static_assert(HP_TEL_COUNT == 3, "one tbl[] row per hm_tel[] subject");

static void fill(views_t *w, const rcc_snapshot_t *m)
{
    const bool stale = hp_meas_stale(m);
    memset(w, 0, sizeof(*w));   /* views are compared bytewise: padding must be stable */
    hp_chan_view(&w->v, hp_v_valid(m), hp_v_floor(m), stale, (int32_t)m->vout_mv, (int32_t)m->vout_filtered_mv, "V");
    hp_chan_view(&w->i, hp_i_valid(m), false, stale, m->iout_ma, m->iout_filtered_ma, "A");
    hp_adc_view(&w->adc, m);
    hp_hero_view(&w->hero, m); hp_sys_view(&w->sys, m); hp_tel_views(w->tel, m);
    hp_fault_sum_view(&w->fsum, m); hp_fault_pri_view(&w->fpri, m); hp_sess_view(&w->sess, m);
}

void hmi_model_init(void)
{
    if (hm_chan_v) return;
    fill(&cur, &(rcc_snapshot_t){0});                      /* placeholders until the first update */
    for (size_t k = 0; k < sizeof(tbl) / sizeof(tbl[0]); ++k) {
        *tbl[k].s = lv_subject_create(LV_SUBJECT_TYPE_POINTER);
        lv_subject_set_pointer(*tbl[k].s, (char *)&cur + tbl[k].off);
    }
}

/* A pointer subject always notifies on set, so the bytewise compare is the gate. The subject points into cur. */
void hmi_model_update(const rcc_snapshot_t *m)
{
    views_t nv;
    fill(&nv, m);
    for (size_t k = 0; k < sizeof(tbl) / sizeof(tbl[0]); ++k) {
        char *c = (char *)&cur + tbl[k].off;
        const char *n = (const char *)&nv + tbl[k].off;
        if (!memcmp(c, n, tbl[k].n)) continue;
        memcpy(c, n, tbl[k].n);
        lv_subject_set_pointer(*tbl[k].s, c);
    }
}
