# Layer: protocol. Controller UART link: RX/TX task, coherent rcc_snapshot_t, START/STOP requests.
idf_component_register(
    SRCS "src/rcc_link.c"
    INCLUDE_DIRS "include"
    REQUIRES hmi_common
    PRIV_REQUIRES rcc_codec esp_driver_uart esp_driver_gpio esp_timer)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra -Werror)
