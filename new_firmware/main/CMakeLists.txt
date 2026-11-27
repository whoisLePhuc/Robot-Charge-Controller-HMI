idf_component_register(
    SRCS "bringup/elecrow_s3_main.c"
    REQUIRES esp_psram spi_flash esp_system heap log freertos
             esp_timer esp_lvgl_port esp_event
             hmi_common rcc_link bsp_elecrow_s3 hmi_settings hmi_wifi hmi_ota hmi_power ui_app
)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra -Werror)
