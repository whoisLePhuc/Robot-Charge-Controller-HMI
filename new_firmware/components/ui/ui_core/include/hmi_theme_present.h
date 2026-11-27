/*
 * Maps hmi_present.h tones and fault states to theme colours, so pages only pick a tone and never a hex value.
 */
#pragma once

#include <stdint.h>

#include "hmi_present.h"

/* Text colour of a presentation tone (normal = white, accent = nav cyan). */
uint32_t ht_kind_color(hp_kind_t k);
/* Measure channel chip colour: stale grey, valid green, below floor amber, invalid red. */
uint32_t ht_chan_color(hp_chan_state_t st);
/* Fault-state colour: ok green, fault red, inhibit amber, unknown grey. */
uint32_t ht_fault_color(hp_fault_state_t fs);
/* Fault-state card tint: fault red, inhibit amber, unknown grey, ok = plain card. */
uint32_t ht_fault_bg(hp_fault_state_t fs);
