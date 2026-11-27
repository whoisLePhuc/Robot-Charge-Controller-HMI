# Layer: services. Persistent HMI settings in NVS (brightness, sleep timeout, dim level, language).
# A leaf: depends on no project component.
idf_component_register(
    SRCS "src/hmi_settings.c"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES nvs_flash)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra -Werror)
