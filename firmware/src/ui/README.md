# HMI UI layers

LVGL 9.6 UI for the 480x320 landscape HMI. `src/hmi_ui.c` only implements the public API in `hmi_ui.h`;
everything else lives here.

```
hmi_ui.c               public API (start, select page, open Wi-Fi, sim inject) -> thin wrappers
ui/hmi_shell.c         chrome (rail/header/banner), action bar + confirm modal, OTA progress overlay,
                       1 s rebuild/refresh timer, overlay service (hmi_shell_overlay_open/close)
ui/hmi_nav.c           visible page, page switch (hmi_nav_goto), tap-to-navigate, redraw request
ui/hmi_wifi_ui.c       Wi-Fi flow + on-screen keyboard, drawn on a shell overlay
ui/hmi_pages.c         page registry: {name, rail icon, fault badge, main bar, build, set_sub, ops}
ui/hmi_model.c         retained-mode model: LVGL subjects over small POD views built by hmi_present (Measure, Overview, Faults, Session)
ui/pages/page_*.c      one build function per page (page_device.h: Device tab ids)
ui/hmi_widgets.c       absolute-positioned LVGL primitives (box, card, text, icon, chip, kv, scroll area...)
ui/hmi_theme.c         palette (hmi_theme.h, HC_*/HF_*) and tone -> colour mapping (hmi_theme_present.h)
ui/hmi_present.c       pure logic: text, tones, derived state. No LVGL, host-tested
```

Include edges (arrows point at what is included):

- `hmi_shell` -> `hmi_pages`, `hmi_nav`, `page_device.h` (tab ids for the header taps), `hmi_widgets`, `hmi_theme_present`
- `hmi_nav` -> `hmi_pages`, `hmi_widgets`
- pages -> `hmi_nav` (goto, redraw), `hmi_model` (retained pages: subjects), `hmi_widgets`, `hmi_present`, `hmi_theme_present`
- `page_device` -> also `hmi_wifi_ui` (opens the Wi-Fi flow) and the settings/backlight/Wi-Fi/OTA services (see rules)
- `hmi_wifi_ui` -> `hmi_shell` (overlay service only), `hmi_widgets`; it includes no page and no `hmi_nav`
- `hmi_widgets` -> `hmi_theme` -> `lvgl`; `hmi_theme.c` -> `hmi_theme_present` -> `hmi_present` -> `rcc_link.h`

`hmi_theme.h` and `hmi_widgets.h` are LVGL-only: they do not pull in `hmi_present.h` or `rcc_link.h`.

## Rules

- Pages never call `rcc_link_*`. The shell takes one snapshot per refresh and passes it in (`const rcc_snapshot_t *`,
  plus `hmi_page_ctx_t`: height, link state, the action-bar spec). Controller data reaches a page only that way.
  The Device page is the one exception for non-controller data: it also reads and writes `hmi_settings` and
  reads `hmi_wifi` and `hmi_ota`, and its tab is state kept in `page_device.c`. A legacy page is a pure function
  of the snapshot and ctx; a retained page is a function of the model subjects plus the snapshot and ctx, and keeps
  its widget pointers in file-static state (cleared in `leave`).
- Per-page state stays in the page file (exported setter, wired through the registry's `set_sub`), not in the
  shell, nav or ctx. Per-page shell behaviour is a field of the registry row (`main_bar`, `fault_badge`).
- Logic goes into `hmi_present` (formatting, thresholds, state classification, gauge scaling), with a test in
  `test/host/test_hmi_present.c`. Pages only lay out what `hmi_present` returns and map a tone to a colour with
  `hmi_theme_present.h`.
- The shared palette is defined only in `hmi_theme.h` (`HC_CARD`, `HC_NAV_CUR`, one-off tints such as
  `HC_STOPPING_BG`, `HC_OTA_EDGE`). No hex literals in other UI files.
- Scroll areas use a per-page slot number (`enum` in the page file, below `HW_SCR_SLOTS`) via `hw_scroll_area`; offsets are
  cleared on every `hmi_nav_goto`, so pages number their slots from 0.
- Overlays (full-screen flows) go through `hmi_shell_overlay_open()`; they never touch the shell's page container.
- Work happens under the LVGL port lock, from the LVGL task.

## Retained mode (enter / update / leave)

A page is either legacy or retained. A legacy page has `build` and no `ops`: the shell cleans the content area and
calls `build` with a fresh snapshot on every 1 s refresh. A retained page has `ops` (`hmi_page_ops_t`, `build` is NULL)
and its widgets are created once; later refreshes only change values on the existing widgets. **Overview, Measure, Faults and
Session** are retained; **Device** is still legacy (its tabs and settings read other services; migrate it only if it pays off).

| Hook | When it runs | What it does |
|---|---|---|
| `enter(root, m, ctx)` | page shown for the first time, or the shell key changed (page, content height/top, any `hmi_nav_goto`, overlay closed) | create all widgets in `root`, store pointers in file-static state, bind observers to `root` |
| `update(m, ctx)` (optional) | every refresh after `enter`, after `hmi_model_update` | only for regions no subject drives (variable-length lists): compare a signature, rebuild that region only |
| `leave()` (optional) | just before `root` is deleted | clear the file-static widget pointers |

The shell calls `hmi_model_update(m)` on every refresh whose visible page is retained (the shell is the only caller of
`rcc_link_snapshot`), then enter/update; the legacy Device page does not observe the model, so the views are not built
for it (they are rebuilt before the next `enter`). The update costs about ten `hp_*_view` builds plus a `memcmp`
each, on a ~1 KB stack buffer, once per second. Touch, modal and overlay still pause the whole refresh as before.

Model: `hmi_model.c` owns one LVGL **pointer** subject per UI view (`hm_chan_v/i`, `hm_adc` for Measure; `hm_hero`, `hm_sys`,
`hm_tel[]` for Overview; `hm_fault_sum`, `hm_fault_pri` for Faults; `hm_sess` for Session). The view is
a zero-padded POD struct built by a pure `hp_*_view()` in `hmi_present` (text, enums, ints; no colours, no LVGL types).
A pointer subject notifies on every set, so the model `memcmp`s the new view with the current one and only calls
`lv_subject_set_pointer` when the bytes differ. Views are static (no heap); each subject is ~1 small heap block, created
once with `lv_subject_create` (the `lv_subject_init_*` API is deprecated). The observer module must be on:
`CONFIG_LV_USE_OBSERVER=y` (the Kconfig default, set in `sdkconfig.elecrow_s3`) and `LV_USE_OBSERVER 1` in `tools/sim/lv_conf.h`.

Rules for a retained page:
- Create objects in the same order as the old builder (z-order is part of the pixels). A region whose shape depends on
  the data (the Measure matrix cells) is a child container that is cleaned and refilled only when its small signature
  changes (`have`, status bits); keeping every alternative as a hidden widget cost ~8 KB of the 64 KB heap.
- Anything positioned from a label's width (chip, unit next to a value) is re-measured in the observer after
  `lv_label_set_text` (`hw_width_of`), never stored from an earlier text.
- An observer callback only changes widgets: never delete `root`, never `lv_obj_clean(root)`, never navigate.
  Structural changes (e.g. a list changing shape) belong in `update`.
- Bind with `lv_subject_add_observer_obj(subj, cb, obj, &state)` where `obj` lives as long as the widgets it draws:
  the observer is removed with it, and the callback runs once on add, so "fresh build" and "update" are the same code.
- Scroll offsets live in the page's scroll object, which survives every in-place update. The shell also re-enters a
  retained page when the banner changes the geometry (`top`/`height`) and a Faults body class change recreates the lists, so
  create a list's scroll box with `hw_scroll_area(.., slot)` (it records the offset in its slot) and, when filling a box that
  was just created, finish with `hw_scroll_restore(box, slot)` instead of `hw_scroll_to`.
  Variable-length lists keep the card, caption and scroll box for the page's life and refill only the rows: compare a
  signature in `update` (a pure `hp_*` function: `hp_events_sig` for the Session events, the mask itself for the Faults bit
  lists), then `y = lv_obj_get_scroll_y(box); lv_obj_clean(box); ...rows...; hw_scroll_to(box, y)`. States of one region that
  exclude each other (offline text / "no events" / list) are all created once and toggled with `lv_obj_set_hidden`.
- A structure that depends on data (Overview: the hero is 74 px high with a stage strip and 72 without, which moves every
  card below; Faults: which cards exist under the summary, `hp_fault_body`) is checked in `update`, which rebuilds that part
  (`lv_obj_clean`, build again). Never from an observer. Parts that change shape often but are small (the Overview stage strip,
  the Measure matrix) are a child container refilled when their own small key changes.
- Optional, rarely shown widgets (the Overview start-block strip, driven by `ctx->bar` rather than a subject) can be created
  and deleted in `update`: that costs less of the 64 KB heap than keeping them hidden.

Per page: **Measure** binds everything (matrix cells refilled on `have`/bits). **Overview** binds hero, system rows and the
three gauge cards, strip refilled on `stage`, whole page rebuilt when `stage >= 0` flips. **Faults** binds the summary; body
rebuilt on a class change, primary card on its text, each list's rows on its mask. **Session** binds the four values and the
caption; the event rows are rebuilt when `hp_events_sig` changes.

Subject or `update()`? Use a **subject** when the page shows a fixed set of widgets whose text/colour/visibility is a pure
function of one small view (the common case: add the field to a view, bind it). Use **`update(m)`** only for structure
that depends on data (variable-length rows, which cards exist, an optional strip): compare a pure signature from
`hmi_present` (`hp_events_sig`, `hp_events_region`, `hp_fault_body`, the mask) and rebuild just that region. The decision
(which region, which caption) belongs in `hmi_present` with a host test; the page only switches on the returned enum.

How to add a bound widget (a new field on Measure): add the field to the view struct in `hmi_present.h`, fill it in
its `hp_*_view()` (with a host test), create the widget in `enter`, and set it in the existing `apply_*` observer with
`hw_label_set` / a style setter. A new view needs a subject: declare it in `hmi_model.h`, `lv_subject_create` it in
`hmi_model.c` (a field in `views_t`, a line in `fill()`, a row in `tbl[]`), and observe it from the page with
`lv_subject_add_observer_obj`. That is the whole recipe: no create/publish code per view.

## Adding a page

Both kinds need one row in `ui/hmi_pages.c` (`{name, icon, fault_badge, main_bar, build, set_sub, ops}`; `NULL` for unused hooks)
and an extern next to the others. Add an id to `hmi_page_id_t` in `hmi_pages.h` only if other code navigates to the page;
otherwise append a plain row after the designated ones. `parent`/`root` is a 408 px wide container, `ctx->height` px high.

**Legacy page** (rebuilt every refresh): create `ui/pages/page_foo.c` with
`void page_foo_build(lv_obj_t *parent, const rcc_snapshot_t *m, const hmi_page_ctx_t *ctx)` and declare it in a header
(see `page_device.h`); registry row `{"Foo", &icon, false, false, page_foo_build, NULL, NULL}`.

**Retained page** (widgets created once): create `ui/pages/page_foo.c` exporting `const hmi_page_ops_t page_foo_ops`;
the registry row is `{"Foo", &icon, false, false, NULL, NULL, &page_foo_ops}` plus
`extern const hmi_page_ops_t page_foo_ops;`. Skeleton:

```c
static struct { lv_obj_t *val; } fo;                              /* file-static widget pointers */
static void apply_chan(lv_observer_t *o, lv_subject_t *s)         /* runs once on add, then on each change */
{ (void)o; const hp_chan_view_t *v = lv_subject_get_pointer(s); hw_label_set(fo.val, v->value); }
static void enter(lv_obj_t *root, const rcc_snapshot_t *m, const hmi_page_ctx_t *ctx)
{
    (void)m; (void)ctx; memset(&fo, 0, sizeof(fo));
    fo.val = hw_text_in(root, "", 13, 7, 100, 18, HF_14B, HC_WHITE, LV_TEXT_ALIGN_LEFT);
    lv_subject_add_observer_obj(hm_chan_v, apply_chan, fo.val, NULL);
}
static void leave(void) { memset(&fo, 0, sizeof(fo)); }
const hmi_page_ops_t page_foo_ops = {.enter = enter, .leave = leave};   /* .update only for data-shaped regions */
```

That is all for the build systems: `src/CMakeLists.txt` globs `ui/pages/page_*.c` (touch that file after adding
a page so CMake re-runs) and the simulator Makefile globs `ui/pages/*.c`. The rail and the page switch iterate the table; the rail pitch shrinks above five pages
(`HMI_PAGES_MAX` is 6; a seventh tab needs a scrolling or "More" rail). Files outside `ui/pages/` must still be
listed in `src/CMakeLists.txt` and in `APPSRC` in `tools/sim/Makefile`.

## Checks

```sh
sh tools/sim/check.sh                    # build the PC simulator, render 80 PNGs, diff against the baseline
CC=gcc sh test/host/run.sh               # host unit tests (rcc_codec, hmi_present)
```

After the diff, `check.sh` runs `tools/sim/check_morph.py`: for every ordered pair of scenarios (A, B) and every retained
page it shows A, switches the snapshot to B live (`hmi_sim A page out seq=B`), lets the 1 s refresh update the widgets in
place and requires the frame to equal the fresh render of B (`MORPH DIFF: none`). It also covers `A -> A+aged` / `A -> A+evt`
and a scroll-survival check on Faults and Session. First-paint equality alone does not prove the update path. Add a page to
`MIGRATED` in that script when it becomes retained. Scroll survival is checked for in-place updates (`+aged`, `+evt`,
`+nopri`, `+quiet`) and for a banner height change that re-enters the page (`MANYFAULT+sim -> MANYFAULT`, both dragged to the
end of the list so the clamped offsets agree). `+sim`, `+simrdy` and `+stale` cover the flags no canned scenario varies, and a
control fails the run if no scenario pair changes a page's pixels. Mutation check (done by hand, not in CI): removing the
`hw_scroll_restore` call in `page_session.c` or `page_faults.c`, or skipping one `lv_subject_set_pointer` in
`hmi_model_update`, makes `MORPH DIFF` fail; a removed `apply_*` call in an observer shows in the first-paint `PIXEL DIFF`.

`check.sh` must print `PIXEL DIFF: none` for a pure refactor. The baseline must be recorded from the original
(pre-change) commit with `sh tools/sim/check.sh baseline`; `/tmp` is volatile, so redo it after a reboot. Re-record
it only after an intentional visual change. The 80 images cover every scenario x page, the Device tabs, the
command/confirm overlays, the Wi-Fi flow (list, scroll, keyboard, connecting, info, failure) and the OTA overlay.
See `tools/sim/README.md`.

## LVGL Pro / XML

LVGL 9.5+ removed the XML runtime, so LVGL Pro (Editor) output cannot be loaded at run time. If it is used,
export it as C and place the result in `ui/gen/`. Never hand-edit generated files; regenerate them. Bind the
generated widgets through the same layers: colours and fonts from `hmi_theme`, text and state from
`hmi_present`, navigation through `hmi_nav.h`, and data via the snapshot handed to the page.
