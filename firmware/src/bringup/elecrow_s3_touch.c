#include "elecrow_s3_touch.h"

#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hmi_backlight.h"

enum { TOUCH_SDA = 38, TOUCH_SCL = 39, TOUCH_RST = 48, TOUCH_INT = 47 };
static const char *TAG = "elecrow_touch";
static i2c_master_dev_handle_t s_device;
static bool s_integrated;
static lv_point_t s_last;
static uint32_t s_errors;
static bool s_pressed;
static uint8_t s_max_points = 1;

static esp_err_t read_register(uint16_t reg, uint8_t *data, size_t size)
{
    const uint8_t addr[] = {(uint8_t)(reg >> 8), (uint8_t)reg};
    return i2c_master_transmit_receive(s_device, s_integrated ? addr : addr + 1,
                                      s_integrated ? 2 : 1, data, size, 10);
}

static void read_touch(lv_indev_t *indev, lv_indev_data_t *data)
{
    const bool was_pressed = s_pressed;
    s_pressed = false;
    data->state = LV_INDEV_STATE_RELEASED;
    data->point = s_last;
    uint8_t raw[70] = {0};
    /* Reading the final byte of the last supported point acknowledges a TDDI
     * report (Interface Protocol A section 6.6). A point0-only read leaves the
     * report latched, preventing later press/release reports from advancing. */
    esp_err_t err = read_register(s_integrated ? 0x0010 : 0x0002, raw,
                                  s_integrated ? 1 : 5);
    if (err == ESP_OK && s_integrated) {
        if (!(raw[0] & 0x08u)) return;
        err = read_register(0x0014, raw, 7u * s_max_points);
    }
    if (err != ESP_OK) {
        lv_indev_reset(indev, NULL); /* Cancel the gesture; an error is not a tap. */
        if (++s_errors == 1u || s_errors % 100u == 0u)
            ESP_LOGW(TAG, "I2C read errors: %lu (%s)", (unsigned long)s_errors, esp_err_to_name(err));
        return;
    }
    uint16_t x, y;
    if (s_integrated) {
        /* Seven bytes/point; choose the first valid contact for the pointer. */
        const uint8_t *point = NULL;
        for (uint8_t i = 0; i < s_max_points; ++i) {
            if (raw[7u * i] & 0x80u) { point = &raw[7u * i]; break; }
        }
        if (point == NULL) return;
        x = ((uint16_t)(point[0] & 0x3fu) << 8) | point[1];
        y = ((uint16_t)(point[2] & 0x3fu) << 8) | point[3];
    } else {
        /* FT6x36-compatible register layout for the alternate 0x38 module. */
        if ((raw[0] & 0x0fu) == 0u || (raw[0] & 0x0fu) > 2u ||
            (raw[1] >> 6) == 1u || (raw[1] >> 6) == 3u) return;
        x = ((uint16_t)(raw[1] & 0x0fu) << 8) | raw[2];
        y = ((uint16_t)(raw[3] & 0x0fu) << 8) | raw[4];
    }
    if (x >= 320u || y >= 480u) {
        lv_indev_reset(indev, NULL);
        return; /* Reject corrupt coordinates, do not clamp or complete a tap. */
    }
    /* Safe screen wake-up: if display was dimmed, wake up and ignore touch gesture */
    if (hmi_backlight_is_dimmed()) {
        hmi_backlight_notify_activity();
        lv_indev_reset(indev, NULL);
        return;
    }
    hmi_backlight_notify_activity();
    data->point.x = x;
    data->point.y = y;
    data->state = LV_INDEV_STATE_PRESSED;
    s_last = data->point;
    s_pressed = true;
    if (!was_pressed) {
        ESP_LOGI(TAG, "Touch down native=(%u,%u), landscape=(%u,%u)",
                 (unsigned)x, (unsigned)y, 479u - (unsigned)y, (unsigned)x);
    }
}

esp_err_t elecrow_s3_touch_start(lv_display_t *display)
{
    const gpio_config_t reset = {.pin_bit_mask = 1ULL << TOUCH_RST, .mode = GPIO_MODE_OUTPUT};
    const gpio_config_t interrupt = {
        .pin_bit_mask = 1ULL << TOUCH_INT, .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
    };
    ESP_RETURN_ON_ERROR(gpio_config(&reset), TAG, "touch reset GPIO");
    ESP_RETURN_ON_ERROR(gpio_config(&interrupt), TAG, "touch interrupt GPIO");
    ESP_RETURN_ON_ERROR(gpio_set_level(TOUCH_RST, 0), TAG, "touch reset low");
    vTaskDelay(pdMS_TO_TICKS(10));
    ESP_RETURN_ON_ERROR(gpio_set_level(TOUCH_RST, 1), TAG, "touch reset high");
    vTaskDelay(pdMS_TO_TICKS(100));

    const i2c_master_bus_config_t config = {
        .i2c_port = I2C_NUM_0, .sda_io_num = TOUCH_SDA, .scl_io_num = TOUCH_SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT, .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t bus = NULL;
    ESP_RETURN_ON_ERROR(i2c_new_master_bus(&config, &bus), TAG, "I2C bus");
    uint8_t address = 0x55;
    s_integrated = i2c_master_probe(bus, address, 20) == ESP_OK;
    if (!s_integrated) {
        address = 0x38;
        ESP_RETURN_ON_ERROR(i2c_master_probe(bus, address, 20), TAG, "no touch at 0x55 or 0x38");
    }
    const i2c_device_config_t device = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7, .device_address = address,
        .scl_speed_hz = 400000,
    };
    ESP_RETURN_ON_ERROR(i2c_master_bus_add_device(bus, &device, &s_device), TAG, "touch device");
    uint8_t info = 0;
    ESP_RETURN_ON_ERROR(read_register(s_integrated ? 0x0009 : 0x00a6, &info, 1), TAG, "touch identity read");
    if (s_integrated) {
        ESP_RETURN_ON_FALSE(info > 0u && info <= 10u, ESP_ERR_INVALID_RESPONSE,
                            TAG, "invalid maximum touch count: %u", (unsigned)info);
        s_max_points = info;
    }
    ESP_LOGI(TAG, "Touch detected at 0x%02x (%s); info=0x%02x; native 320x480; LVGL rotates once",
             address, s_integrated ? "ST77922 integrated" : "FT6x36-compatible", info);

    lv_indev_t *input = lv_indev_create();
    ESP_RETURN_ON_FALSE(input, ESP_ERR_NO_MEM, TAG, "LVGL pointer");
    lv_indev_set_type(input, LV_INDEV_TYPE_POINTER);
    lv_indev_set_display(input, display);
    lv_indev_set_read_cb(input, read_touch);
    return ESP_OK;
}
