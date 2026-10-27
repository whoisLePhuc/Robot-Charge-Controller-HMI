idf_component_register(
    SRCS "bringup/elecrow_s3_main.c" "bringup/elecrow_s3_display.c" "bringup/elecrow_s3_touch.c"
         "rcc_link.c" "rcc_codec.c" "hmi_ui.c" "hmi_icons.c"
         "hmi_settings.c" "hmi_backlight.c" "hmi_wifi.c" "hmi_ota.c"
         "hmi_fonts/hmi_f10.c" "hmi_fonts/hmi_f12.c" "hmi_fonts/hmi_f12b.c" "hmi_fonts/hmi_f14b.c"
         "hmi_fonts/hmi_f16b.c" "hmi_fonts/hmi_f22b.c" "hmi_fonts/hmi_f28b.c"
         "hmi_fonts/hmi_hero28.c"
    REQUIRES esp_psram spi_flash esp_system heap log freertos
             esp_lcd esp_driver_spi esp_driver_gpio esp_driver_i2c esp_driver_uart esp_timer
             esp_lvgl_port esp_lcd_st77922 nvs_flash esp_driver_ledc
             esp_wifi esp_netif esp_event app_update esp_http_server
)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra -Werror)
