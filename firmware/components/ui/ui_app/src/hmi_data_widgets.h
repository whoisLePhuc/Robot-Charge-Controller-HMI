#pragma once

#include "lvgl.h"

/* The Editor accepts a non-const font pointer, but generated creation only passes it to LVGL style setters. */
#define HMI_DATA_FONT(font) ((lv_font_t *)(font))

static inline lv_obj_t *hmi_data_widget(lv_obj_t *parent, const char *name)
{
    lv_obj_t *widget = lv_obj_find_by_name(parent, name);
    LV_ASSERT_MSG(widget != NULL, name);
    return widget;
}

/* These fixed template captions are ASCII. Use the real font's kerning and letter spacing. */
static inline int hmi_data_ascii_width(const char *text, const lv_font_t *font, int spacing)
{
    int width = 0;
    while (*text) {
        width += lv_font_get_glyph_width(font, (unsigned char)text[0], (unsigned char)text[1]);
        if (text[1]) width += spacing;
        ++text;
    }
    return width;
}
