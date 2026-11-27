# Layer: ui. Presentation logic: text, tones, derived state and views from rcc_snapshot_t.
# Pure C, no LVGL, host-tested (test/host/test_hmi_present.c).
idf_component_register(
    SRCS "src/hmi_present.c"
    INCLUDE_DIRS "include"
    REQUIRES hmi_common)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra -Werror)
