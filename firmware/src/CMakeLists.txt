idf_component_register(
    SRCS "main.c" "hmi_board.c" "hmi_ui.c" "hmi_icons.c" "rcc_codec.c" "rcc_link.c"
         "hmi_fonts/hmi_f10.c" "hmi_fonts/hmi_f12.c" "hmi_fonts/hmi_f12b.c" "hmi_fonts/hmi_f14b.c"
         "hmi_fonts/hmi_f16b.c" "hmi_fonts/hmi_f20b.c" "hmi_fonts/hmi_f22b.c" "hmi_fonts/hmi_f28b.c"
         "hmi_fonts/hmi_hero28.c" "hmi_fonts/hmi_hero40.c" "hmi_fonts/hmi_hero48.c"
    REQUIRES driver esp_lcd esp_driver_spi esp_driver_gpio esp_driver_uart esp_timer esp_hw_support
)
