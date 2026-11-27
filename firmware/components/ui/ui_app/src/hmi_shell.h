/*
 * HMI shell: chrome (rail/header/banner), action bar + confirm modal, OTA overlay and the rebuild/refresh timer.
 * Navigation services for pages are in hmi_nav.h; full-screen flows (Wi-Fi) use the overlay service below.
 * LVGL task only.
 */
#pragma once

#include <stdbool.h>

#include "lvgl.h"

/* Build the chrome, start the 1 s refresh timer and draw the first page. */
void hmi_shell_start(bool link_started);
/* Show registry page `page_idx` (sub-tab `sub_tab`, -1 keeps it); out-of-range indices are ignored. */
void hmi_shell_select_page(int page_idx, int sub_tab);

/* Overlay service: opens an empty full-screen layer above everything (closing the confirm modal and freeing the
 * page underneath); the page rebuild is paused while it is open. Returns NULL if one is already open. */
lv_obj_t *hmi_shell_overlay_open(void);
/* Delete the overlay and rebuild the page. */
void hmi_shell_overlay_close(void);
