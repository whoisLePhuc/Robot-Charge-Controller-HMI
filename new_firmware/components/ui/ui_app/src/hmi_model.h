/*
 * UI model: LVGL pointer subjects over small POD views built by hmi_present. The shell feeds it once per refresh;
 * retained pages observe the subjects (lv_subject_add_observer_obj) and draw the view they are handed.
 */
#pragma once

#include "lvgl.h"
#include "hmi_present.h"
#include "rcc_link.h"

/* Create the subjects (once, before any page is entered). */
void hmi_model_init(void);
/* Build the views from `m`; a subject notifies its observers only when its view bytes changed. */
void hmi_model_update(const rcc_snapshot_t *m);

/* Measure page: hp_chan_view_t (VOUT, IOUT), hp_adc_view_t. Use lv_subject_get_pointer() in the observer. */
extern lv_subject_t *hm_chan_v, *hm_chan_i, *hm_adc;
/* Overview: hp_hero_view_t, hp_sys_view_t, hp_tel_view_t (volt, current, power). */
extern lv_subject_t *hm_hero, *hm_sys, *hm_tel[HP_TEL_COUNT];
/* Faults: hp_fault_sum_view_t (summary card), hp_fault_pri_view_t (primary/inhibit card). Session: hp_sess_view_t (four values + event caption). */
extern lv_subject_t *hm_fault_sum, *hm_fault_pri, *hm_sess;
