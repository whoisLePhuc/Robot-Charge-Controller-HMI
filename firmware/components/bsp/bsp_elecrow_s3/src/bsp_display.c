/* ST77922 QSPI panel and LVGL port. Starts no application module: app.c starts the link, UI and touch. */
#include "bsp_elecrow_s3.h"

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_check.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_st77922.h"
#include "esp_lvgl_port.h"
#include "lvgl.h"
#include "bsp_pins.h"
#include "elecrow_s3_panel_init.h"

enum {
    NATIVE_WIDTH = 320, NATIVE_HEIGHT = 480,
    LANDSCAPE_WIDTH = 480, LANDSCAPE_HEIGHT = 320,
    BUFFER_PIXELS = LANDSCAPE_WIDTH * 32,
};

static const char *TAG = "elecrow_lcd";

/* Round both axes: with software rotation, logical Y becomes panel X.
 * ST77922 requires native X start and exclusive end aligned to four pixels.
 * Both display dimensions are divisible by four, so this stays within bounds. */
static void round_area(lv_area_t *area)
{
    area->x1 &= ~3;
    area->y1 &= ~3;
    area->x2 |= 3;
    area->y2 |= 3;
}

esp_err_t bsp_display_start(lv_display_t **out_display)
{
    /* Initialize PWM backlight at 0% while configuring ST77922 display */
    ESP_RETURN_ON_ERROR(bsp_backlight_init(0), TAG, "backlight init");

    const spi_bus_config_t bus = ST77922_PANEL_BUS_QSPI_CONFIG(
        BSP_LCD_CLK, BSP_LCD_D0, BSP_LCD_D1, BSP_LCD_D2, BSP_LCD_D3, BUFFER_PIXELS * sizeof(uint16_t));
    ESP_RETURN_ON_ERROR(spi_bus_initialize(SPI2_HOST, &bus, SPI_DMA_CH_AUTO), TAG, "QSPI bus");
    esp_lcd_panel_io_handle_t io = NULL;
    esp_lcd_panel_io_spi_config_t io_config = ST77922_PANEL_IO_QSPI_CONFIG(BSP_LCD_CS, NULL, NULL);
    io_config.pclk_hz = 20 * 1000 * 1000;
    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI2_HOST,
                                               &io_config, &io), TAG, "panel IO");
    st77922_vendor_config_t vendor = {
        .init_cmds = elecrow_lcd_init_cmds,
        .init_cmds_size = sizeof(elecrow_lcd_init_cmds) / sizeof(elecrow_lcd_init_cmds[0]),
        .flags.use_qspi_interface = 1,
    };
    const esp_lcd_panel_dev_config_t config = {
        .reset_gpio_num = -1, /* LCD reset shares MCU EN. */
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
        .vendor_config = &vendor,
    };
    esp_lcd_panel_handle_t panel = NULL;
    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_st77922(io, &config, &panel), TAG, "ST77922 driver");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_reset(panel), TAG, "panel reset");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_init(panel), TAG, "panel init");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_disp_on_off(panel, true), TAG, "display on");

    const lvgl_port_cfg_t port = ESP_LVGL_PORT_INIT_CONFIG();
    ESP_RETURN_ON_ERROR(lvgl_port_init(&port), TAG, "LVGL port");
    const lvgl_port_display_cfg_t display_config = {
        .io_handle = io,
        .panel_handle = panel,
        .buffer_size = BUFFER_PIXELS,
        .double_buffer = true,
        .hres = NATIVE_WIDTH,
        .vres = NATIVE_HEIGHT,
        .rounder_cb = round_area,
        .color_format = LV_COLOR_FORMAT_RGB565,
        .flags = {.buff_dma = true, .sw_rotate = true, .swap_bytes = true},
    };
    lv_display_t *display = lvgl_port_add_disp(&display_config);
    ESP_RETURN_ON_FALSE(display, ESP_ERR_NO_MEM, TAG, "LVGL display");
    ESP_RETURN_ON_FALSE(lvgl_port_lock(1000), ESP_ERR_TIMEOUT, TAG, "LVGL lock");
    lv_display_set_rotation(display, LV_DISPLAY_ROTATION_90);
    lvgl_port_unlock();
    *out_display = display;
    ESP_LOGI(TAG, "ST77922 initialized: QSPI 20 MHz, RGB565, logical 480x320; visual check pending");
    return ESP_OK;
}
