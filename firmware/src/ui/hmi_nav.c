/*
 * Navigation state (visible page) and the page-switch / redraw services of hmi_nav.h.
 */
#include "hmi_nav.h"

#include <stdint.h>

#include "hmi_pages.h"
#include "hmi_widgets.h"

static int cur_page;
static lv_async_cb_t redraw_cb;
static void (*leave_cb)(void);

void hmi_nav_bind(lv_async_cb_t redraw, void (*on_leave)(void)) { redraw_cb = redraw; leave_cb = on_leave; }
int hmi_nav_page(void) { return cur_page; }
void hmi_nav_request_redraw(void) { if (redraw_cb) (void)lv_async_call(redraw_cb, NULL); }

void hmi_nav_goto(int page, int sub)
{
    if (page < 0 || page >= hmi_page_count) return;
    if (leave_cb) leave_cb();
    hw_scroll_reset_all();
    if (sub >= 0 && hmi_pages[page].set_sub) hmi_pages[page].set_sub(sub);
    cur_page = page;
    hmi_nav_request_redraw();
}

/* Tap target: user data is the page, plus (sub + 1) << 8 when the tap also selects a sub-state. */
static void nav_to_cb(lv_event_t *e)
{
    const intptr_t v = (intptr_t)lv_event_get_user_data(e);
    hmi_nav_goto((int)(v & 0xff), (int)(v >> 8) - 1);
}
void hmi_nav_make_tappable(lv_obj_t *o, int page, int sub)
{
    lv_obj_set_clickable(o, true);
    lv_obj_add_event_cb(o, nav_to_cb, LV_EVENT_CLICKED, (void *)(intptr_t)(page | ((sub + 1) << 8)));
}
