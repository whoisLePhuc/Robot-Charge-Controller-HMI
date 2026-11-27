# Layer: ui. Theme (palette HC_*, fonts HF_*, tone -> colour) and the absolute-positioned LVGL widgets.
idf_component_register(
    SRCS "src/hmi_theme.c" "src/hmi_widgets.c"
    INCLUDE_DIRS "include"
    REQUIRES lvgl ui_assets ui_present)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra -Werror)
