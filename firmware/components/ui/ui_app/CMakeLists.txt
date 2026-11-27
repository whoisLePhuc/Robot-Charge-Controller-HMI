# Layer: ui. The HMI application: shell, navigation, page registry, retained-mode model, Wi-Fi flow and
# pages. Public API: include/hmi_ui.h. Everything else is private (src/); rules in ../README.md.
#
# Every src/pages/page_*.c is compiled (registered in src/hmi_pages.c). A plain glob: after adding a page
# file, touch this file so CMake re-runs (CONFIGURE_DEPENDS is not allowed in IDF's requirements-scan mode).
file(GLOB PAGE_SRCS RELATIVE ${CMAKE_CURRENT_LIST_DIR} "${CMAKE_CURRENT_LIST_DIR}/src/pages/page_*.c")

idf_component_register(
    SRCS "src/hmi_ui.c" "src/hmi_shell.c" "src/hmi_nav.c" "src/hmi_pages.c" "src/hmi_model.c"
         "src/hmi_wifi_ui.c" ${PAGE_SRCS}
    INCLUDE_DIRS "include"
    PRIV_INCLUDE_DIRS "src"
    PRIV_REQUIRES lvgl ui_core ui_present ui_assets hmi_common rcc_link
                  hmi_settings hmi_wifi hmi_ota hmi_power esp_system)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra -Werror)
