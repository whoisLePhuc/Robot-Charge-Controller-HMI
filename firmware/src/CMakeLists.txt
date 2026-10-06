idf_component_register(
    SRCS "main.c" "hmi_board.c"
    REQUIRES driver esp_lcd esp_driver_spi esp_driver_gpio
)
