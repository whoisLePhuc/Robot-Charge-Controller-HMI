# Composition root (app.c): the only component that sees every other one. main.c is the IDF entry point.
idf_component_register(
    SRCS "main.c" "app.c"
    PRIV_REQUIRES esp_psram spi_flash esp_event esp_lvgl_port
                  hmi_common rcc_link bsp_elecrow_s3 hmi_settings hmi_wifi hmi_ota hmi_power ui_app)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra -Werror)
