#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_compat.h"
#include "lvgl.h"
#include "../../firmware/src/hmi_ui.h"

#define SCREEN_W 480
#define SCREEN_H 320

static uint8_t g_framebuffer[SCREEN_W * SCREEN_H * 4];

static bool save_bmp(const char *filename, uint32_t width, uint32_t height, const uint8_t *argb_data)
{
    FILE *f = fopen(filename, "wb");
    if (!f) {
        printf("Failed to open %s for writing\n", filename);
        return false;
    }

    uint32_t image_size = width * height * 4;
    uint32_t file_size = 54 + image_size;

    uint8_t file_header[14] = {
        'B', 'M',
        (uint8_t)file_size, (uint8_t)(file_size >> 8), (uint8_t)(file_size >> 16), (uint8_t)(file_size >> 24),
        0, 0, 0, 0,
        54, 0, 0, 0
    };
    fwrite(file_header, 1, 14, f);

    int32_t neg_height = -(int32_t)height; // top-down
    uint8_t info_header[40] = {
        40, 0, 0, 0,
        (uint8_t)width, (uint8_t)(width >> 8), (uint8_t)(width >> 16), (uint8_t)(width >> 24),
        (uint8_t)neg_height, (uint8_t)(neg_height >> 8), (uint8_t)(neg_height >> 16), (uint8_t)(neg_height >> 24),
        1, 0,
        32, 0,
        0, 0, 0, 0,
        (uint8_t)image_size, (uint8_t)(image_size >> 8), (uint8_t)(image_size >> 16), (uint8_t)(image_size >> 24),
        0x13, 0x0B, 0, 0,
        0x13, 0x0B, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0
    };
    fwrite(info_header, 1, 40, f);

    fwrite(argb_data, 1, image_size, f);
    fclose(f);
    printf("Successfully exported snapshot: %s (%ux%u)\n", filename, width, height);
    return true;
}

static void my_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    int32_t w = area->x2 - area->x1 + 1;
    int32_t h = area->y2 - area->y1 + 1;
    for (int32_t y = 0; y < h; ++y) {
        int32_t dst_y = area->y1 + y;
        if (dst_y < 0 || dst_y >= SCREEN_H) continue;
        int32_t dst_x = area->x1;
        if (dst_x < 0 || dst_x >= SCREEN_W) continue;
        uint32_t *dst = (uint32_t *)g_framebuffer + dst_y * SCREEN_W + dst_x;
        const uint32_t *src = (const uint32_t *)px_map + y * w;
        memcpy(dst, src, w * 4);
    }
    lv_display_flush_ready(disp);
}

static uint32_t tick_cb(void)
{
    return (uint32_t)(clock() * 1000 / CLOCKS_PER_SEC);
}

void mock_set_scenario(int scenario);   /* mock_runtime.c */

static void settle(lv_display_t *disp)
{
    for (int i = 0; i < 4; ++i) lv_timer_handler();   /* page switches are async calls */
    lv_obj_invalidate(lv_screen_active());
    lv_refr_now(disp);
}

static void render_and_save(lv_display_t *disp, const char *dir, int scenario, int page, int tab, const char *name)
{
    char path[512];
    mock_set_scenario(scenario);
    hmi_ui_select_page(page, tab);
    settle(disp);
    snprintf(path, sizeof(path), "%s/%s.bmp", dir, name);
    save_bmp(path, SCREEN_W, SCREEN_H, g_framebuffer);
}

/* Renders the README previews (480x320 BMP) into argv[1], default ".".
 * tools/snapshot/export_previews.py upscales them to assets/ui/*.png. */
int main(int argc, char **argv)
{
    const char *dir = argc > 1 ? argv[1] : ".";
    lv_init();
    lv_tick_set_cb(tick_cb);

    static uint8_t render_buf[SCREEN_W * 40 * 4];
    lv_display_t *disp = lv_display_create(SCREEN_W, SCREEN_H);
    lv_display_set_buffers(disp, render_buf, NULL, sizeof(render_buf), LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_ARGB8888);
    lv_display_set_flush_cb(disp, my_flush_cb);

    mock_set_scenario(0);
    hmi_ui_start(true);

    render_and_save(disp, dir, 0, 0, 0, "overview");
    render_and_save(disp, dir, 0, 1, 0, "measure");
    render_and_save(disp, dir, 1, 2, 0, "faults");
    render_and_save(disp, dir, 0, 3, 0, "session");
    render_and_save(disp, dir, 0, 4, 0, "device");
    render_and_save(disp, dir, 0, 4, 2, "settings");

    char path[512];
    hmi_ui_open_wifi();
    settle(disp);
    snprintf(path, sizeof(path), "%s/wifi.bmp", dir);
    save_bmp(path, SCREEN_W, SCREEN_H, g_framebuffer);
    return 0;
}
