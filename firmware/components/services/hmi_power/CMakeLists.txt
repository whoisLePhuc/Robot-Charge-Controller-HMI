# Layer: services. Display power policy (brightness, dim/sleep, wake on touch) over the BSP backlight.
# The one allowed service-to-service dependency: hmi_settings, the leaf store it reads and writes.
idf_component_register(
    SRCS "src/hmi_power.c"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES bsp_elecrow_s3 hmi_settings esp_timer)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra -Werror)
