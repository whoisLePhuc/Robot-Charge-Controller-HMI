/*
 * The page registry: rail order, label, icon and build function of every page. Add a page here.
 */
#include "hmi_pages.h"

#include "hmi_icons.h"
#include "pages/page_device.h"

void page_overview_build(lv_obj_t *parent, const rcc_snapshot_t *m, const hmi_page_ctx_t *ctx);
void page_measure_build(lv_obj_t *parent, const rcc_snapshot_t *m, const hmi_page_ctx_t *ctx);
void page_faults_build(lv_obj_t *parent, const rcc_snapshot_t *m, const hmi_page_ctx_t *ctx);
void page_session_build(lv_obj_t *parent, const rcc_snapshot_t *m, const hmi_page_ctx_t *ctx);

/* {name, rail icon, fault badge, main bar, build, set_sub}. Rows named by hmi_page_id_t use designated
 * initializers; a page nothing navigates to is a plain row appended after the last one. */
const hmi_page_def_t hmi_pages[] = {
    [HMI_PAGE_OVERVIEW] = {"Overview", &hmi_icon_home_19,  false, true,  page_overview_build, NULL},
    [HMI_PAGE_MEASURE]  = {"Measure",  &hmi_icon_gauge_19, false, false, page_measure_build,  NULL},
    [HMI_PAGE_FAULTS]   = {"Faults",   &hmi_icon_warn_19,  true,  false, page_faults_build,   NULL},
    [HMI_PAGE_SESSION]  = {"Session",  &hmi_icon_list_19,  false, false, page_session_build,  NULL},
    [HMI_PAGE_DEVICE]   = {"Device",   &hmi_icon_chip_19,  false, false, page_device_build,   page_device_set_tab},
};
const int hmi_page_count = (int)(sizeof(hmi_pages) / sizeof(hmi_pages[0]));

_Static_assert(sizeof(hmi_pages) / sizeof(hmi_pages[0]) <= HMI_PAGES_MAX, "nav rail has room for HMI_PAGES_MAX tabs");
