/*
 * Faults page: fault state banner, primary fault card and the scrollable fault/inhibit bit lists.
 */
#include <inttypes.h>
#include <stdio.h>

#include "hmi_icons.h"
#include "hmi_pages.h"
#include "hmi_present.h"
#include "hmi_theme_present.h"
#include "hmi_widgets.h"

enum { SLOT_FAULTS, SLOT_INHIBITS };   /* scroll slots of the two bit lists */

static void mask_list(lv_obj_t *p, int x, int y, int w, int h, const char *title, uint32_t mask,
                      const char *const *names, int count, int slot)
{
    lv_obj_t *c = hw_card_box(p, x, y, w, h, HC_CARD, 10, HC_LINE);
    char hs[16]; snprintf(hs, sizeof(hs), "0x%04" PRIX32, mask);
    hw_section(c, 13, 7, w - 26, title, hs, HC_LABEL);
    int n = 0;
    for (int b = 0; b < 32; ++b) if (mask & (1u << b)) ++n;
    if (!n) { hw_text_in(c, "None (0x0)", 13, 23, w - 26, 18, HF_12, HC_LABEL, LV_TEXT_ALIGN_LEFT); return; }
    lv_obj_t *a = hw_scroll_area(c, 0, 23, w, h - 23 - 3, slot);
    int r = 0;
    for (int b = 0; b < 32; ++b) {
        if (!(mask & (1u << b))) continue;
        const int ry = r * 18;
        char nm[40], bl[8];
        if (b < count) snprintf(nm, sizeof(nm), "%s", names[b]); else snprintf(nm, sizeof(nm), "Unrecognized (bit %d)", b);
        snprintf(bl, sizeof(bl), "b%d", b);
        hw_text_in(a, nm, 13, ry, w - 26 - 24, 18, HF_10, HC_WHITE, LV_TEXT_ALIGN_LEFT);
        hw_text_in(a, bl, w - 13 - 24, ry, 24, 18, HF_10, HC_LABEL, LV_TEXT_ALIGN_RIGHT);
        ++r;
    }
    hw_scroll_restore(a, slot);
}

void page_faults_build(lv_obj_t *p, const rcc_snapshot_t *m, const hmi_page_ctx_t *ctx)
{
    const int H = ctx->height;
    const hp_fault_state_t fs = hp_fault_state(m);
    const uint32_t tc = ht_fault_color(fs), bg = ht_fault_bg(fs);
    const uint32_t bd = fs == HP_FS_OK ? HC_LINE : tc;
    lv_obj_t *sum = hw_card_box(p, 0, 0, 408, 40, bg, 10, bd);
    hw_icon_row(sum, fs == HP_FS_OK ? &hmi_icon_checkc_18 : &hmi_icon_warn_18, 13, 0, 40, tc);
    hw_text_in(sum, hp_fault_title(fs), 41, 0, 250, 40, HF_14B, fs == HP_FS_OK ? HC_WHITE : tc, LV_TEXT_ALIGN_LEFT);
    char rs[40], at[24];
    if (m->online && m->faults_valid) { hp_fmt_age(at, sizeof(at), m->faults_age_ms); snprintf(rs, sizeof(rs), "Read %s ago", at); }
    else snprintf(rs, sizeof(rs), "Not read");
    hw_text_in(sum, rs, 290, 0, 105, 40, HF_10, HC_LABEL, LV_TEXT_ALIGN_RIGHT);

    if (fs == HP_FS_OK || fs == HP_FS_UNKNOWN) {
        const int ch = H - 46, ih = ch - 14;
        const int gap = (ih - 16 - 17 - 13) / 2;
        for (int k = 0; k < 2; ++k) {
            lv_obj_t *c = hw_card_box(p, k * 208, 46, 200, ch, HC_CARD, 10, HC_LINE);
            hw_section(c, 13, 7, 174, k ? "INHIBIT REGISTER" : "FAULT REGISTER", fs == HP_FS_UNKNOWN ? "\xE2\x80\x94" : "0x0000", HC_LABEL);
            hw_text_in(c, fs == HP_FS_UNKNOWN ? "Unreadable" : "0 active bits", 13, 7 + 16 + gap, 174, 17, HF_14B,
                       fs == HP_FS_UNKNOWN ? HC_DIM : HC_OK_COL, LV_TEXT_ALIGN_LEFT);
            hw_text_in(c, fs == HP_FS_UNKNOWN ? "Link offline" : (k ? "No inhibit bits set" : "No fault bits set"),
                       13, ch - 7 - 13, 174, 13, HF_10, HC_LABEL, LV_TEXT_ALIGN_LEFT);
        }
        return;
    }
    int y = 46;
    const bool pri = m->primary_fault || m->inhibit_mask;
    if (pri) {
        const uint32_t pc = m->primary_fault ? HC_FAULT_COL : HC_WARN_COL;
        lv_obj_t *c = hw_card_box(p, 0, y, 408, 44, m->primary_fault ? HC_FAULT_BG : HC_WARN_BG, 10, pc);
        char nm[64], id[12], act[80], im[40];
        if (m->primary_fault) {
            snprintf(nm, sizeof(nm), "PRIMARY \xC2\xB7 %s", hp_fault_name(m->primary_fault));
            snprintf(id, sizeof(id), "0x%04X", (unsigned)m->primary_fault);
            snprintf(act, sizeof(act), "Action: %s", hp_fault_hint(m->primary_fault));
        } else {
            hp_mask_text(im, sizeof(im), m->inhibit_mask, hp_inhibit_names, 3);
            snprintf(nm, sizeof(nm), "INHIBIT \xC2\xB7 %s", im);
            id[0] = '\0';
            snprintf(act, sizeof(act), "START may clear it; the controller decides");
        }
        hw_text_in(c, nm, 13, 6, 300, 18, HF_12B, pc, LV_TEXT_ALIGN_LEFT);
        hw_text_in(c, id, 320, 6, 75, 18, HF_12, HC_LABEL, LV_TEXT_ALIGN_RIGHT);
        hw_text_in(c, act, 13, 23, 382, 14, HF_10, HC_TXT2, LV_TEXT_ALIGN_LEFT);
        y += 50;
    }
    mask_list(p, 0, y, 200, H - y, "FAULTS", m->active_fault_mask, hp_fault_names, 19, SLOT_FAULTS);
    mask_list(p, 208, y, 200, H - y, "INHIBITS", m->inhibit_mask, hp_inhibit_names, 3, SLOT_INHIBITS);
}
