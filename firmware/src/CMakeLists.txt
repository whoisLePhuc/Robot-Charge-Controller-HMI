# Every ui/pages/page_*.c is compiled (registered in ui/hmi_pages.c). A plain glob: after adding a page file,
# touch this file so CMake re-runs (CONFIGURE_DEPENDS is not allowed in IDF's requirements-scan script mode).
file(GLOB PAGE_SRCS RELATIVE ${COMPONENT_DIR} "${COMPONENT_DIR}/ui/pages/page_*.c")

idf_component_register(
    SRCS "bringup/elecrow_s3_main.c" "bringup/elecrow_s3_display.c" "bringup/elecrow_s3_touch.c"
         "rcc_link.c" "rcc_codec.c" "hmi_ui.c" "hmi_icons.c" "ui/hmi_theme.c" "ui/hmi_present.c"
         "ui/hmi_widgets.c" "ui/hmi_nav.c" "ui/hmi_shell.c" "ui/hmi_wifi_ui.c" "ui/hmi_pages.c" ${PAGE_SRCS}
         "hmi_settings.c" "hmi_backlight.c" "hmi_wifi.c" "hmi_ota.c"
         "hmi_fonts/hmi_f10.c" "hmi_fonts/hmi_f12.c" "hmi_fonts/hmi_f12b.c" "hmi_fonts/hmi_f14b.c"
         "hmi_fonts/hmi_f16b.c" "hmi_fonts/hmi_f22b.c" "hmi_fonts/hmi_f28b.c"
         "hmi_fonts/hmi_hero28.c"
    INCLUDE_DIRS "." "ui"
    REQUIRES esp_psram spi_flash esp_system heap log freertos
             esp_lcd esp_driver_spi esp_driver_gpio esp_driver_i2c esp_driver_uart esp_timer
             esp_lvgl_port esp_lcd_st77922 nvs_flash esp_driver_ledc
             esp_wifi esp_netif esp_event app_update esp_http_server
)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra -Werror)
