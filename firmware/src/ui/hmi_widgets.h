/*
 * HMI widget helpers: absolute-positioned LVGL primitives shared by the pages, shell and Wi-Fi flow.
 * Cards have a 1 px border and -1 px padding, so child coordinates are relative to the card's outer corner.
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "hmi_theme.h"
#include "lvgl.h"

lv_obj_t *hw_obj(lv_obj_t *p, int x, int y, int w, int h);                       /* transparent, not clickable */
lv_obj_t *hw_box(lv_obj_t *p, int x, int y, int w, int h, uint32_t bg, int radius);
lv_obj_t *hw_card_box(lv_obj_t *p, int x, int y, int w, int h, uint32_t bg, int radius, uint32_t border_col);
lv_obj_t *hw_button(lv_obj_t *p, int x, int y, int w, int h, uint32_t bg, int radius,
                    lv_event_cb_t cb, void *user);
void hw_hline(lv_obj_t *p, int x, int y, int w, uint32_t c);

int hw_lh(const lv_font_t *f);                                                    /* line height */
void hw_spacing(lv_obj_t *o, int px);                                             /* letter spacing */
int hw_width_of(lv_obj_t *label);                                                 /* laid-out width */
lv_obj_t *hw_text(lv_obj_t *p, const char *s, int x, int y, const lv_font_t *f, uint32_t c);
lv_obj_t *hw_text_in(lv_obj_t *p, const char *s, int x, int y, int w, int h,
                     const lv_font_t *f, uint32_t c, lv_text_align_t align);

lv_obj_t *hw_icon(lv_obj_t *p, const lv_image_dsc_t *d, int x, int y, uint32_t color);
lv_obj_t *hw_icon_row(lv_obj_t *p, const lv_image_dsc_t *d, int x, int y, int h, uint32_t color);
lv_obj_t *hw_icon_c(lv_obj_t *p, const lv_image_dsc_t *d, int cx, int cy, uint32_t color);
void hw_spin_anim(void *o, int32_t v);                                            /* lv_anim exec cb: rotate an image */

int hw_chip_at(lv_obj_t *p, int x, int y, bool right_anchor, const char *s, uint32_t fg, uint32_t bg);
void hw_chip(lv_obj_t *p, int right_x, int y, const char *s, uint32_t fg, uint32_t bg);
void hw_kv(lv_obj_t *p, int x, int y, int w, int h, const char *k, const char *v, uint32_t vc);
void hw_section(lv_obj_t *p, int x, int y, int w, const char *left, const char *right, uint32_t rc);

/* Vertical scroll areas keep their offset per slot across the 1 s rebuild. Call hw_scroll_restore() after
 * filling the area. A slot is a small index (< HW_SCR_SLOTS) the caller picks; the offsets are cleared on every
 * page change (hmi_nav_goto), so pages can number their slots from 0 independently. */
#define HW_SCR_SLOTS 4
lv_obj_t *hw_scroll_area(lv_obj_t *p, int x, int y, int w, int h, int slot);
void hw_scroll_restore(lv_obj_t *a, int slot);
void hw_scroll_reset(int slot);
void hw_scroll_reset_all(void);
