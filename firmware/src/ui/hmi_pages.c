/*
 * The page registry: rail order, label, icon and build function of every page. Add a page here.
 */
#include "hmi_pages.h"

#include "hmi_icons.h"
#include "pages/page_device.h"

extern const hmi_page_ops_t page_overview_ops;
extern const hmi_page_ops_t page_measure_ops;
extern const hmi_page_ops_t page_faults_ops;
extern const hmi_page_ops_t page_session_ops;

/* {name, rail icon, fault badge, main bar, build, set_sub, ops}. Rows named by hmi_page_id_t use designated
 * initializers; a page nothing navigates to is a plain row appended after the last one. */
const hmi_page_def_t hmi_pages[] = {
    [HMI_PAGE_OVERVIEW] = {"Overview", &hmi_icon_home_19,  false, true,  NULL,                NULL, &page_overview_ops},
    [HMI_PAGE_MEASURE]  = {"Measure",  &hmi_icon_gauge_19, false, false, NULL,                NULL, &page_measure_ops},
    [HMI_PAGE_FAULTS]   = {"Faults",   &hmi_icon_warn_19,  true,  false, NULL,                NULL, &page_faults_ops},
    [HMI_PAGE_SESSION]  = {"Session",  &hmi_icon_list_19,  false, false, NULL,                NULL, &page_session_ops},
    [HMI_PAGE_DEVICE]   = {"Device",   &hmi_icon_chip_19,  false, false, page_device_build,   page_device_set_tab, NULL},
};
const int hmi_page_count = (int)(sizeof(hmi_pages) / sizeof(hmi_pages[0]));

_Static_assert(sizeof(hmi_pages) / sizeof(hmi_pages[0]) <= HMI_PAGES_MAX, "nav rail has room for HMI_PAGES_MAX tabs");
