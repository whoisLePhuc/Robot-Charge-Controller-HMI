/*
 * Navigation and redraw services the shell offers to pages (the only shared mutable UI state a page may touch).
 * Implemented in hmi_nav.c; call from the LVGL task only.
 */
#pragma once

#include "lvgl.h"

/* Shell wiring: `redraw` rebuilds the screen (run asynchronously), `on_leave` runs before a page switch. */
void hmi_nav_bind(lv_async_cb_t redraw, void (*on_leave)(void));
/* Registry index of the visible page. */
int hmi_nav_page(void);
/* Switch to registry page `page`: clears scroll offsets, rebuilds on the next LVGL tick. `sub` >= 0 is passed to the
 * page's set_sub hook (e.g. Device tab), -1 leaves the page's own state alone. Out-of-range pages are ignored. */
void hmi_nav_goto(int page, int sub);
/* Make `o` navigate on tap, as hmi_nav_goto(page, sub). */
void hmi_nav_make_tappable(lv_obj_t *o, int page, int sub);
/* Rebuild the current page on the next LVGL tick (after a setting changed). */
void hmi_nav_request_redraw(void);
