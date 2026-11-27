/*
 * Device page interface beyond the registry: the sub-tab ids, so other screens can open a given tab.
 */
#pragma once

#include "hmi_pages.h"

typedef enum { HMI_DEV_CONTROLLER, HMI_DEV_LINK, HMI_DEV_SETTINGS } hmi_dev_tab_t;

void page_device_build(lv_obj_t *parent, const rcc_snapshot_t *m, const hmi_page_ctx_t *ctx);
/* Select the visible tab (hmi_dev_tab_t); out-of-range values are ignored. */
void page_device_set_tab(int tab);
