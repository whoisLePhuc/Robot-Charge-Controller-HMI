/*
 * HMI bring-up app: proves the pinned toolchain and board support build and run.
 *
 * The screen shows the LVGL version, the display and touch status, three colour bars
 * (left to right must read red, green, blue) and the touch position, so the display,
 * orientation, colour order and touch can be checked by eye. Not the HMI application.
 *
 * No logging: the ESP-IDF console is disabled (UART0 carries the controller link,
 * HMI-D03). Problems are shown on the screen.
 */

#include "esp_lvgl_port.h"
#include "hmi_board.h"
#include "lvgl.h"

static lv_obj_t *s_touch_label;

static void touch_cb(lv_event_t *event)
{
    (void)event;
    lv_indev_t *indev = lv_indev_active();
    if (indev == NULL) {
        return;
    }
    lv_point_t point;
    lv_indev_get_point(indev, &point);
    lv_label_set_text_fmt(s_touch_label, "touch x=%d y=%d", (int)point.x, (int)point.y);
}

static void build_screen(const hmi_board_status_t *status)
{
    lv_obj_t *screen = lv_screen_active();
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);

    lv_obj_t *title = lv_label_create(screen);
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    lv_label_set_text_fmt(title, "RCC HMI bring-up\nLVGL %d.%d.%d  %dx%d", LVGL_VERSION_MAJOR,
                          LVGL_VERSION_MINOR, LVGL_VERSION_PATCH, HMI_BOARD_HRES,
                          HMI_BOARD_VRES);
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 8, 8);

    lv_obj_t *info = lv_label_create(screen);
    lv_obj_set_style_text_color(info, lv_color_white(), 0);
    lv_label_set_text_fmt(info, "display %s   touch %s",
                          (status->display == ESP_OK) ? "OK" : "FAIL",
                          (status->touch == ESP_OK) ? "OK" : "FAIL");
    lv_obj_align(info, LV_ALIGN_TOP_LEFT, 8, 56);

    static const uint32_t colours[3] = {0xFF0000, 0x00FF00, 0x0000FF};
    for (int index = 0; index < 3; ++index) {
        lv_obj_t *bar = lv_obj_create(screen);
        lv_obj_set_size(bar, 100, 60);
        lv_obj_set_style_bg_color(bar, lv_color_hex(colours[index]), 0);
        lv_obj_set_style_border_width(bar, 0, 0);
        lv_obj_align(bar, LV_ALIGN_BOTTOM_LEFT, 8 + (index * 108), -8);
    }

    s_touch_label = lv_label_create(screen);
    lv_obj_set_style_text_color(s_touch_label, lv_color_hex(0xFFD400), 0);
    lv_label_set_text(s_touch_label, "touch: none");
    lv_obj_align(s_touch_label, LV_ALIGN_BOTTOM_RIGHT, -8, -8);

    /* The whole screen reports the touch position, so the corners can be checked. */
    lv_obj_add_event_cb(screen, touch_cb, LV_EVENT_PRESSING, NULL);
}

void app_main(void)
{
    const hmi_board_status_t status = hmi_board_init();
    if (status.display != ESP_OK) {
        return; /* nothing can be shown */
    }
    if (lvgl_port_lock(1000)) {
        build_screen(&status);
        lvgl_port_unlock();
    }
}
