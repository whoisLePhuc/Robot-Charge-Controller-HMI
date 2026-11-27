# Layer: bsp. Elecrow DLE06235B / ES3C35P drivers: ST77922 QSPI LCD, LVGL port, touch, LEDC backlight.
# Pins are private (src/bsp_pins.h). Depends on no project component.
idf_component_register(
    SRCS "src/bsp_display.c" "src/bsp_touch.c" "src/bsp_backlight.c"
    INCLUDE_DIRS "include"
    PRIV_INCLUDE_DIRS "src"
    REQUIRES lvgl
    PRIV_REQUIRES esp_lcd esp_lcd_st77922 esp_lvgl_port esp_driver_spi esp_driver_gpio esp_driver_i2c
                  esp_driver_ledc)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra -Werror)
