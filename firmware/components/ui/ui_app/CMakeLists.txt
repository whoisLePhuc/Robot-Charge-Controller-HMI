# Layer: ui. The HMI application: shell, navigation, page registry, retained-mode model, Wi-Fi flow and
# pages. Public API: include/hmi_ui.h. Everything else is private (src/); rules in ../README.md.
#
# Every src/pages/page_*.c is compiled (registered in src/hmi_pages.c). A plain glob: after adding a page
# file, touch this file so CMake re-runs (CONFIGURE_DEPENDS is not allowed in IDF's requirements-scan mode).
file(GLOB PAGE_SRCS RELATIVE ${CMAKE_CURRENT_LIST_DIR} "${CMAKE_CURRENT_LIST_DIR}/src/pages/page_*.c")

# Compile only the firmware layouts. The Editor also exports sample screens and assets.
set(HMI_LAYOUT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../../lvgl_ui/components/hmi")
file(GLOB HMI_LAYOUT_SRCS "${HMI_LAYOUT_DIR}/*/*_gen.c")
set(HMI_LAYOUT_INCLUDE_DIRS)
foreach(layout IN LISTS HMI_LAYOUT_SRCS)
    get_filename_component(layout_dir "${layout}" DIRECTORY)
    list(APPEND HMI_LAYOUT_INCLUDE_DIRS "${layout_dir}")
endforeach()
list(REMOVE_DUPLICATES HMI_LAYOUT_INCLUDE_DIRS)

idf_component_register(
    SRCS "src/hmi_ui.c" "src/hmi_shell.c" "src/hmi_nav.c" "src/hmi_pages.c" "src/hmi_model.c"
         "src/hmi_wifi_ui.c" ${PAGE_SRCS} ${HMI_LAYOUT_SRCS}
    INCLUDE_DIRS "include"
    PRIV_INCLUDE_DIRS "src" ${HMI_LAYOUT_INCLUDE_DIRS}
    PRIV_REQUIRES lvgl ui_core ui_present ui_assets hmi_common rcc_link
                  hmi_settings hmi_wifi hmi_ota hmi_power esp_system)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra -Werror)

set_source_files_properties(${HMI_LAYOUT_SRCS} PROPERTIES
    COMPILE_OPTIONS "-include;${CMAKE_CURRENT_LIST_DIR}/src/hmi_lvgl_export.h")
