# Layer: services. Web OTA: PIN-armed HTTP upload server, slot status, rollback confirmation.
# Started/stopped by app.c on HMI_WIFI_EVENT; depends on no project component.
idf_component_register(
    SRCS "src/hmi_ota.c"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES app_update esp_app_format esp_partition esp_http_server esp_timer)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra -Werror)
