# Layer: ui. Generated assets: LVGL fonts (src/fonts, see README there) and A8 icons (tools/gen_icons.py).
idf_component_register(
    SRCS "src/hmi_icons.c"
         "src/fonts/hmi_f10.c" "src/fonts/hmi_f12.c" "src/fonts/hmi_f12b.c" "src/fonts/hmi_f14b.c"
         "src/fonts/hmi_f16b.c" "src/fonts/hmi_f22b.c" "src/fonts/hmi_f28b.c" "src/fonts/hmi_hero28.c"
    INCLUDE_DIRS "include"
    REQUIRES lvgl)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra -Werror)
