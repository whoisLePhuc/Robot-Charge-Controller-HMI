/*
 * HMI page registry types. A page is one build function in ui/pages/page_*.c plus one row in hmi_pages.c;
 * the nav rail and the page switch iterate the table (see ui/README.md, "Adding a page").
 */
#pragma once

#include <stdbool.h>

#include "hmi_present.h"
#include "lvgl.h"
#include "rcc_link.h"

/* Registry indices (rail order) of pages that other code navigates to; other pages need no name here. */
typedef enum { HMI_PAGE_OVERVIEW, HMI_PAGE_MEASURE, HMI_PAGE_FAULTS, HMI_PAGE_SESSION, HMI_PAGE_DEVICE } hmi_page_id_t;

#define HMI_PAGES_MAX 6   /* the rail pitch shrinks from 63 px (up to 5 tabs) so that 6 tabs still fit in 320 px */

/* Read-only inputs for a page build, supplied by the shell. */
typedef struct {
    int height;                    /* content area height in px (it is 408 px wide) */
    bool link_started;             /* the UART link task came up */
    const hp_bar_spec_t *bar;      /* action bar chosen for this rebuild (bar->start_block: why START is missing) */
} hmi_page_ctx_t;

typedef void (*hmi_page_build_fn)(lv_obj_t *parent, const rcc_snapshot_t *m, const hmi_page_ctx_t *ctx);

typedef struct {
    const char *name;               /* rail label */
    const lv_image_dsc_t *icon;     /* rail icon */
    bool fault_badge;               /* rail shows the fault/inhibit dot on this tab */
    bool main_bar;                  /* the action bar is the full-width START/STOP button (else a status strip) */
    hmi_page_build_fn build;
    void (*set_sub)(int sub);       /* optional: select the page's sub-state (tab) when navigated to with sub >= 0 */
} hmi_page_def_t;

extern const hmi_page_def_t hmi_pages[];
extern const int hmi_page_count;
