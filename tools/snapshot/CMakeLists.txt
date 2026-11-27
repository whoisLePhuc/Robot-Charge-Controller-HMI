cmake_minimum_required(VERSION 3.16)
project(hmi_snapshot C)

# Renders the README previews (assets/ui, see export_previews.py) from the real UI components in
# firmware/components/ui. Services and the controller link are mocked in mock_runtime.c.
# LVGL comes from firmware/managed_components: run idf.py build (or reconfigure) in firmware/ first.

set(CMAKE_C_STANDARD 11)

# Options for LVGL
set(LV_CONF_SKIP ON CACHE BOOL "" FORCE)
add_definitions(-DLV_CONF_SKIP=1)
add_definitions(-DLV_USE_SNAPSHOT=1)
add_definitions(-DLV_USE_DRAW_SW=1)
add_definitions(-DLV_COLOR_DEPTH_32=1)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

set(FW "${CMAKE_CURRENT_SOURCE_DIR}/../../firmware")
set(UI "${FW}/components/ui")

# Add LVGL library
set(LVGL_ROOT_DIR "${FW}/managed_components/lvgl__lvgl")
add_subdirectory("${LVGL_ROOT_DIR}" lvgl_build)

# Every source of the four UI components (ui_app pages included).
file(GLOB UI_SRCS
    "${UI}/ui_app/src/*.c" "${UI}/ui_app/src/pages/*.c" "${UI}/ui_core/src/*.c"
    "${UI}/ui_present/src/*.c" "${UI}/ui_assets/src/*.c" "${UI}/ui_assets/src/fonts/*.c")

add_executable(hmi_render render_snapshot.c mock_runtime.c ${UI_SRCS})

file(GLOB COMPONENT_INCLUDES LIST_DIRECTORIES true "${FW}/components/*/*/include")
target_include_directories(hmi_render PRIVATE
    include
    ${COMPONENT_INCLUDES}
    "${UI}/ui_app/src"
    "${LVGL_ROOT_DIR}"
)

target_link_libraries(hmi_render PRIVATE lvgl)
