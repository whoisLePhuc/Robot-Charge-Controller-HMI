#include "hmi_board.h"

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_st7796.h"
#include "esp_lcd_touch.h"
#include "esp_lcd_touch_xpt2046.h"
#include "esp_lvgl_port.h"

/* Pins, schematic V1.1 (docs/hmi-hardware.md). The LCD reset is tied to EN. */
#define PIN_SCLK 14
#define PIN_MOSI 13
#define PIN_MISO 12
#define PIN_LCD_CS 15
#define PIN_LCD_DC 2
#define PIN_BACKLIGHT 27
#define PIN_TOUCH_CS 33

/* VSPI through the GPIO matrix, as the vendor's working demo does. */
#define LCD_HOST SPI3_HOST

#define LCD_CLOCK_HZ (40 * 1000 * 1000)
/*
 * Measured on this board (2026-10-06): the XPT2046 returns valid data at 100 kHz and all
 * zeros at 1 MHz and above, with the finger held. The vendor's 2.5 MHz setting fails too.
 */
#define TOUCH_CLOCK_HZ (100 * 1000)

#define BUFFER_LINES 32

/*
 * Touch calibration: coarse two-edge fit, to be replaced by a proper calibration.
 * Input from the XPT2046 driver: a = raw X scaled to 0..480, b = raw Y scaled to 0..320.
 * Measured 2026-10-06 by pressing the four corners of the landscape screen (reading
 * direction of the text):
 *   top-left    a=27  b=302      top-right    a=49  b=31
 *   bottom-left a=322 b=305      bottom-right a=350 b=18
 * Screen X follows b (reversed); screen Y follows a (increasing downwards). Pressing the
 * extreme corners reads about 15 px short in Y because the bezel blocks the fingertip.
 */
#define CAL_B_LEFT 303.5f
#define CAL_B_RIGHT 24.5f
#define CAL_A_TOP 38.0f
#define CAL_A_BOTTOM 336.0f

static uint16_t clamp_coordinate(float value, float maximum)
{
    if (value < 0.0f) {
        return 0;
    }
    if (value > maximum) {
        return (uint16_t)maximum;
    }
    return (uint16_t)value;
}

/* Runs before the driver's own swap/mirror, which stay disabled. */
static void touch_process_coordinates(esp_lcd_touch_handle_t tp, uint16_t *x, uint16_t *y,
                                      uint16_t *strength, uint8_t *point_num,
                                      uint8_t max_point_num)
{
    (void)tp;
    (void)strength;
    (void)max_point_num;
    for (uint8_t i = 0; i < *point_num; ++i) {
        const float a = (float)x[i];
        const float b = (float)y[i];
        const float screen_x =
            (CAL_B_LEFT - b) * (HMI_BOARD_HRES - 1) / (CAL_B_LEFT - CAL_B_RIGHT);
        const float screen_y =
            (a - CAL_A_TOP) * (HMI_BOARD_VRES - 1) / (CAL_A_BOTTOM - CAL_A_TOP);
        x[i] = clamp_coordinate(screen_x, HMI_BOARD_HRES - 1);
        y[i] = clamp_coordinate(screen_y, HMI_BOARD_VRES - 1);
    }
}

hmi_board_status_t hmi_board_init(void)
{
    hmi_board_status_t status = {
        .display = ESP_FAIL,
        .touch = ESP_FAIL,
    };

    /* Backlight off until the first frame is drawn. */
    const gpio_config_t backlight = {
        .pin_bit_mask = 1ULL << PIN_BACKLIGHT,
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&backlight);
    gpio_set_level(PIN_BACKLIGHT, 0);

    const spi_bus_config_t bus = {
        .sclk_io_num = PIN_SCLK,
        .mosi_io_num = PIN_MOSI,
        .miso_io_num = PIN_MISO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = HMI_BOARD_HRES * BUFFER_LINES * sizeof(uint16_t),
    };
    esp_err_t error = spi_bus_initialize(LCD_HOST, &bus, SPI_DMA_CH_AUTO);
    /* GPIO12 (MTDI strapping pin) keeps an internal pull-down after reset; release it. */
    gpio_pulldown_dis(PIN_MISO);

    esp_lcd_panel_io_handle_t lcd_io = NULL;
    esp_lcd_panel_handle_t panel = NULL;
    if (error == ESP_OK) {
        const esp_lcd_panel_io_spi_config_t io_config = {
            .cs_gpio_num = PIN_LCD_CS,
            .dc_gpio_num = PIN_LCD_DC,
            .spi_mode = 0,
            .pclk_hz = LCD_CLOCK_HZ,
            .trans_queue_depth = 10,
            .lcd_cmd_bits = 8,
            .lcd_param_bits = 8,
        };
        error = esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST, &io_config,
                                         &lcd_io);
    }
    if (error == ESP_OK) {
        const esp_lcd_panel_dev_config_t panel_config = {
            .reset_gpio_num = -1,
            .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR,
            .bits_per_pixel = 16,
        };
        error = esp_lcd_new_panel_st7796(lcd_io, &panel_config, &panel);
    }
    if (error == ESP_OK) {
        error = esp_lcd_panel_reset(panel);
    }
    if (error == ESP_OK) {
        error = esp_lcd_panel_init(panel);
    }
    if (error == ESP_OK) {
        error = esp_lcd_panel_disp_on_off(panel, true);
    }

    esp_lcd_touch_handle_t touch = NULL;
    esp_err_t touch_error = ESP_FAIL;
    if (error == ESP_OK) {
        esp_lcd_panel_io_handle_t touch_io = NULL;
        esp_lcd_panel_io_spi_config_t touch_io_config =
            ESP_LCD_TOUCH_IO_SPI_XPT2046_CONFIG(PIN_TOUCH_CS);
        touch_io_config.pclk_hz = TOUCH_CLOCK_HZ;
        touch_error = esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST,
                                               &touch_io_config, &touch_io);
        if (touch_error == ESP_OK) {
            const esp_lcd_touch_config_t touch_config = {
                .x_max = HMI_BOARD_HRES,
                .y_max = HMI_BOARD_VRES,
                .rst_gpio_num = GPIO_NUM_NC,
                .int_gpio_num = GPIO_NUM_NC, /* polling; the IRQ pin is not used */
                .levels = {.reset = 0, .interrupt = 0},
                /* swap/mirror stay off: touch_process_coordinates maps raw to screen. */
                .flags = {.swap_xy = 0, .mirror_x = 0, .mirror_y = 0},
                .process_coordinates = touch_process_coordinates,
            };
            touch_error = esp_lcd_touch_new_spi_xpt2046(touch_io, &touch_config, &touch);
        }
    }

    if (error == ESP_OK) {
        const lvgl_port_cfg_t port_config = ESP_LVGL_PORT_INIT_CONFIG();
        error = lvgl_port_init(&port_config);
    }
    if (error == ESP_OK) {
        const lvgl_port_display_cfg_t display_config = {
            .io_handle = lcd_io,
            .panel_handle = panel,
            .buffer_size = HMI_BOARD_HRES * BUFFER_LINES,
            .double_buffer = true,
            .hres = HMI_BOARD_HRES,
            .vres = HMI_BOARD_VRES,
            .monochrome = false,
            /* Landscape, confirmed upright on the panel (2026-10-06). */
            .rotation = {.swap_xy = true, .mirror_x = false, .mirror_y = false},
            .color_format = LV_COLOR_FORMAT_RGB565,
            .flags = {.buff_dma = true, .swap_bytes = true},
        };
        status.lvgl_display = lvgl_port_add_disp(&display_config);
        if (status.lvgl_display == NULL) {
            error = ESP_FAIL;
        }
    }
    if ((error == ESP_OK) && (touch != NULL)) {
        const lvgl_port_touch_cfg_t touch_port = {.disp = status.lvgl_display, .handle = touch};
        status.lvgl_touch = lvgl_port_add_touch(&touch_port);
        if (status.lvgl_touch == NULL) {
            touch_error = ESP_FAIL;
        }
    }

    status.display = error;
    status.touch = touch_error;
    if (error == ESP_OK) {
        gpio_set_level(PIN_BACKLIGHT, 1);
    }
    return status;
}
