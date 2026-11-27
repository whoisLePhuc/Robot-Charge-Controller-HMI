# Layer: services. Wi-Fi station for the HMI Wi-Fi screens; posts HMI_WIFI_EVENT link events.
# Knows nothing about OTA: app.c connects the two.
idf_component_register(
    SRCS "src/hmi_wifi.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_event
    PRIV_REQUIRES esp_wifi esp_netif esp_timer nvs_flash)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra -Werror)
