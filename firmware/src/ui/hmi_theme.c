/*
 * HMI theme: tone and fault-state to colour mapping (see hmi_theme_present.h).
 */
#include "hmi_theme_present.h"

#include "hmi_theme.h"

uint32_t ht_kind_color(hp_kind_t k)
{
    return k == HP_K_WARN ? HC_WARN_COL : k == HP_K_FAULT ? HC_FAULT_COL : k == HP_K_OFF ? HC_OFF_COL :
           k == HP_K_ACCENT ? HC_NAV_CUR : HC_WHITE;
}
uint32_t ht_fault_color(hp_fault_state_t fs)
{
    return fs == HP_FS_FAULT ? HC_FAULT_COL : fs == HP_FS_INHIBIT ? HC_WARN_COL : fs == HP_FS_UNKNOWN ? HC_OFF_COL : HC_OK_COL;
}
uint32_t ht_fault_bg(hp_fault_state_t fs)
{
    return fs == HP_FS_FAULT ? HC_FAULT_BG : fs == HP_FS_INHIBIT ? HC_WARN_BG : fs == HP_FS_UNKNOWN ? HC_OFF_BG : HC_CARD;
}
uint32_t ht_chan_color(hp_chan_state_t st)
{
    return st == HP_CH_STALE ? HC_OFF_COL : st == HP_CH_VALID ? HC_OK_COL : st == HP_CH_FLOOR ? HC_WARN_COL : HC_FAULT_COL;
}
