/* PC renderer: hmi_sim <scenario> <page 0-4> <out.ppm> [tab=link] [cmd=<state>] [click=confirm] [px=<n>] */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lvgl.h"
#include "hmi_ui.h"

#define W 480
#define H 320
int sim_scenario(const char *);
void sim_command_state(const char *);

static uint16_t fb[W * H] __attribute__((aligned(4)));
static uint32_t now_ms;
static lv_point_t pt; static bool down;

static uint32_t tick_cb(void) { return now_ms; }
static void flush_cb(lv_display_t *d, const lv_area_t *a, uint8_t *p) { (void)a; (void)p; lv_display_flush_ready(d); }
static void read_cb(lv_indev_t *i, lv_indev_data_t *d)
{ (void)i; d->point = pt; d->state = down ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED; }

static void run(uint32_t ms) { for (uint32_t t = 0; t < ms; t += 20) { now_ms += 20; lv_timer_handler(); } }
static void click(int x, int y)
{ pt.x = x; pt.y = y; down = true; run(60); down = false; run(60); }

int main(int argc, char **argv)
{
    if (argc < 4) { fprintf(stderr, "usage: hmi_sim SCENARIO PAGE OUT.ppm [tab=link] [cmd=x] [click=confirm]\n"); return 2; }
    if (!sim_scenario(argv[1])) { fprintf(stderr, "unknown scenario\n"); return 2; }
    int page = atoi(argv[2]);
    const char *tab = "", *click_what = "";
    for (int i = 4; i < argc; ++i) {
        if (!strncmp(argv[i], "tab=", 4)) tab = argv[i] + 4;
        else if (!strncmp(argv[i], "cmd=", 4)) sim_command_state(argv[i] + 4);
        else if (!strncmp(argv[i], "click=", 6)) click_what = argv[i] + 6;
    }
    lv_init(); lv_tick_set_cb(tick_cb);
    lv_display_t *d = lv_display_create(W, H);
    lv_display_set_color_format(d, LV_COLOR_FORMAT_RGB565);
    lv_display_set_buffers(d, fb, NULL, sizeof(fb), LV_DISPLAY_RENDER_MODE_DIRECT);
    lv_display_set_flush_cb(d, flush_cb);
    lv_indev_t *in = lv_indev_create(); lv_indev_set_type(in, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(in, read_cb);
    hmi_ui_start(true);
    run(100);
    if (page > 0) { click(26, page * 64 + 32); run(100); }
    if (!strcmp(tab, "link")) { click(60 + 308, 62 + 28); run(1200); }
    if (!strcmp(click_what, "confirm")) { click(260, 290); run(100); }
    run(1100);                                    /* let the 1 s refresh timer settle */
    { lv_mem_monitor_t mon; lv_mem_monitor(&mon);
      fprintf(stderr, "lvgl heap: total %u, free %u, biggest %u, used %u%%, frag %u%%\n",
              (unsigned)mon.total_size, (unsigned)mon.free_size, (unsigned)mon.free_biggest_size,
              (unsigned)mon.used_pct, (unsigned)mon.frag_pct); }
    FILE *f = fopen(argv[3], "wb");
    fprintf(f, "P6\n%d %d\n255\n", W, H);
    for (int i = 0; i < W * H; ++i) {
        uint16_t c = fb[i];
        uint8_t r = (c >> 11) & 31, g = (c >> 5) & 63, b = c & 31;
        fputc((r << 3) | (r >> 2), f); fputc((g << 2) | (g >> 4), f); fputc((b << 3) | (b >> 2), f);
    }
    fclose(f); return 0;
}
