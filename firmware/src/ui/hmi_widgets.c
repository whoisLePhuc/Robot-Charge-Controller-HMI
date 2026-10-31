/*
 * HMI widget helpers: absolute-positioned LVGL primitives (boxes, labels, icons, chips, rows, scroll areas).
 * Every page and overlay builds from these; colours and fonts come from hmi_theme.h.
 */
#include "hmi_widgets.h"

#include <string.h>

/* ------------------------------------------------------------------ drawing primitives */
/* Cards have a 1 px border and a -1 px padding, so child coordinates are relative to the
 * card's outer top-left corner (the same numbers the HTML concept uses). */
lv_obj_t *hw_obj(lv_obj_t *p, int x, int y, int w, int h)
{
    lv_obj_t *o = lv_obj_create(p);
    lv_obj_remove_style_all(o);
    lv_obj_set_pos(o, x, y);
    lv_obj_set_size(o, w, h);
    lv_obj_set_clickable(o, false);
    lv_obj_set_scrollable(o, false);
    return o;
}
lv_obj_t *hw_box(lv_obj_t *p, int x, int y, int w, int h, uint32_t bg, int radius)
{
    lv_obj_t *o = hw_obj(p, x, y, w, h);
    lv_obj_set_style_bg_color(o, HC(bg), 0);
    lv_obj_set_style_bg_opa(o, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(o, radius, 0);
    return o;
}
lv_obj_t *hw_card_box(lv_obj_t *p, int x, int y, int w, int h, uint32_t bg, int radius, uint32_t border_col)
{
    lv_obj_t *o = hw_box(p, x, y, w, h, bg, radius);
    lv_obj_set_style_border_width(o, 1, 0);
    lv_obj_set_style_border_color(o, HC(border_col), 0);
    lv_obj_set_style_pad_all(o, -1, 0);
    return o;
}

/* Vertical scroll areas: a transparent clip window inside a card. Scroll offsets survive the 1 s
 * rebuild (kept per slot) and are cleared whenever the page changes. */
static int scroll_pos[HW_SCR_SLOTS];

static void scroll_save_cb(lv_event_t *e)
{
    const int slot = (int)(uintptr_t)lv_event_get_user_data(e);
    scroll_pos[slot] = (int)lv_obj_get_scroll_y(lv_event_get_target_obj(e));
}
lv_obj_t *hw_scroll_area(lv_obj_t *p, int x, int y, int w, int h, int slot)
{
    LV_ASSERT(slot >= 0 && slot < HW_SCR_SLOTS);
    lv_obj_t *a = hw_obj(p, x, y, w, h);
    lv_obj_set_scrollable(a, true);
    lv_obj_set_clickable(a, true);
    lv_obj_set_scroll_dir(a, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(a, LV_SCROLLBAR_MODE_AUTO);
    lv_obj_set_style_bg_opa(a, LV_OPA_COVER, LV_PART_SCROLLBAR);
    lv_obj_set_style_bg_color(a, HC(HC_DIM), LV_PART_SCROLLBAR);
    lv_obj_set_style_width(a, 3, LV_PART_SCROLLBAR);
    lv_obj_set_style_radius(a, 2, LV_PART_SCROLLBAR);
    lv_obj_set_style_pad_right(a, 1, LV_PART_SCROLLBAR);
    lv_obj_add_event_cb(a, scroll_save_cb, LV_EVENT_SCROLL, (void *)(uintptr_t)slot);
    return a;
}
void hw_scroll_restore(lv_obj_t *a, int slot)
{
    LV_ASSERT(slot >= 0 && slot < HW_SCR_SLOTS);
    lv_obj_update_layout(a);
    lv_obj_scroll_to_y(a, scroll_pos[slot], LV_ANIM_OFF);
}
void hw_scroll_reset(int slot) { if (slot >= 0 && slot < HW_SCR_SLOTS) scroll_pos[slot] = 0; }
void hw_scroll_reset_all(void) { memset(scroll_pos, 0, sizeof(scroll_pos)); }
int hw_lh(const lv_font_t *f) { return (int)lv_font_get_line_height(f); }
void hw_spacing(lv_obj_t *o, int px) { lv_obj_set_style_text_letter_space(o, px, 0); }
/* Auto-sized single-line label at (x, y). */
lv_obj_t *hw_text(lv_obj_t *p, const char *s, int x, int y, const lv_font_t *f, uint32_t c)
{
    lv_obj_t *o = lv_label_create(p);
    lv_label_set_text(o, s);
    lv_obj_set_pos(o, x, y);
    lv_obj_set_style_text_font(o, f, 0);
    lv_obj_set_style_text_color(o, HC(c), 0);
    return o;
}
/* Fixed-width single-line label, vertically centred in a row of height h; long text ends in "...". */
lv_obj_t *hw_text_in(lv_obj_t *p, const char *s, int x, int y, int w, int h,
                     const lv_font_t *f, uint32_t c, lv_text_align_t align)
{
    lv_obj_t *o = lv_label_create(p);
    lv_label_set_long_mode(o, LV_LABEL_LONG_MODE_DOTS);
    lv_label_set_text(o, s);
    lv_obj_set_size(o, w, hw_lh(f));
    lv_obj_set_style_text_align(o, align, 0);
    lv_obj_set_style_text_font(o, f, 0);
    lv_obj_set_style_text_color(o, HC(c), 0);
    lv_obj_set_pos(o, x, y + (h - hw_lh(f)) / 2);
    return o;
}
int hw_width_of(lv_obj_t *label) { lv_obj_update_layout(label); return lv_obj_get_width(label); }
lv_obj_t *hw_icon(lv_obj_t *p, const lv_image_dsc_t *d, int x, int y, uint32_t color)
{
    lv_obj_t *i = lv_image_create(p);
    lv_image_set_src(i, d);
    lv_obj_set_pos(i, x, y);
    lv_obj_set_style_image_recolor(i, HC(color), 0);
    lv_obj_set_style_image_recolor_opa(i, LV_OPA_COVER, 0);
    lv_obj_set_clickable(i, false);
    return i;
}
/* Icon centred on a row of height h. */
lv_obj_t *hw_icon_row(lv_obj_t *p, const lv_image_dsc_t *d, int x, int y, int h, uint32_t color)
{
    return hw_icon(p, d, x, y + (h - (int)d->header.h) / 2, color);
}
lv_obj_t *hw_icon_c(lv_obj_t *p, const lv_image_dsc_t *d, int cx, int cy, uint32_t color)
{
    return hw_icon(p, d, cx - (int)d->header.w / 2, cy - (int)d->header.h / 2, color);
}
void hw_spin_anim(void *o, int32_t v) { lv_image_set_rotation(o, v); }
void hw_hline(lv_obj_t *p, int x, int y, int w, uint32_t c) { hw_box(p, x, y, w, 1, c, 0); }

lv_obj_t *hw_button(lv_obj_t *p, int x, int y, int w, int h, uint32_t bg, int radius,
                    lv_event_cb_t cb, void *user)
{
    lv_obj_t *o = lv_button_create(p);
    lv_obj_remove_style_all(o);
    lv_obj_set_pos(o, x, y);
    lv_obj_set_size(o, w, h);
    lv_obj_set_style_bg_color(o, HC(bg), 0);
    lv_obj_set_style_bg_opa(o, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(o, radius, 0);
    lv_obj_set_scrollable(o, false);
    lv_obj_set_style_bg_opa(o, LV_OPA_70, LV_STATE_PRESSED);
    if (cb) lv_obj_add_event_cb(o, cb, LV_EVENT_CLICKED, user);
    return o;
}
/* Pill label (17 px high). right_anchor: x is the right edge, otherwise the left edge. Returns width. */
int hw_chip_at(lv_obj_t *p, int x, int y, bool right_anchor, const char *s, uint32_t fg, uint32_t bg)
{
    lv_obj_t *l = hw_text(p, s, 0, 0, HF_10, fg);
    const int w = hw_width_of(l) + 14;
    const int x0 = right_anchor ? x - w : x;
    hw_box(p, x0, y, w, 17, bg, 6);
    lv_obj_move_foreground(l);
    lv_obj_set_pos(l, x0 + 7, y + (17 - hw_lh(HF_10)) / 2);
    return w;
}
void hw_chip(lv_obj_t *p, int right_x, int y, const char *s, uint32_t fg, uint32_t bg)
{
    (void)hw_chip_at(p, right_x, y, true, s, fg, bg);
}
/* "label ............ value" row of height h. */
void hw_kv(lv_obj_t *p, int x, int y, int w, int h, const char *k, const char *v, uint32_t vc)
{
    lv_obj_t *vl = hw_text(p, v, 0, 0, HF_12B, vc);
    int vw = hw_width_of(vl);
    lv_obj_delete(vl);
    lv_obj_t *kl = hw_text_in(p, k, x, y, w, h, HF_12, HC_LABEL, LV_TEXT_ALIGN_LEFT);
    (void)kl;
    if (vw > w - 4) vw = w - 4;
    hw_text_in(p, v, x + w - vw - 1, y, vw + 2, h, HF_12B, vc, LV_TEXT_ALIGN_RIGHT);
}
/* Section title row (16 px) with an optional right-aligned value. */
void hw_section(lv_obj_t *p, int x, int y, int w, const char *left, const char *right, uint32_t rc)
{
    lv_obj_t *l = hw_text(p, left, x, y + (16 - hw_lh(HF_10)) / 2, HF_10, HC_LABEL);
    hw_spacing(l, 1);
    if (right) {
        const int lw = hw_width_of(l) + 8;
        hw_text_in(p, right, x + lw, y, w - lw, 16, HF_10, rc, LV_TEXT_ALIGN_RIGHT);
    }
}
