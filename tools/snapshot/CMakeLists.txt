cmake_minimum_required(VERSION 3.16)
project(hmi_snapshot C)

set(CMAKE_C_STANDARD 11)

# Options for LVGL
set(LV_CONF_SKIP ON CACHE BOOL "" FORCE)
add_definitions(-DLV_CONF_SKIP=1)
add_definitions(-DLV_USE_SNAPSHOT=1)
add_definitions(-DLV_USE_DRAW_SW=1)
add_definitions(-DLV_COLOR_DEPTH_32=1)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

# Add LVGL library
set(LVGL_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../firmware/managed_components/lvgl__lvgl")
add_subdirectory("${LVGL_ROOT_DIR}" lvgl_build)

# Snapshot target
add_executable(hmi_render
    render_snapshot.c
    mock_runtime.c
    ../../firmware/src/hmi_ui.c
    ../../firmware/src/hmi_icons.c
    ../../firmware/src/hmi_fonts/hmi_f10.c
    ../../firmware/src/hmi_fonts/hmi_f12.c
    ../../firmware/src/hmi_fonts/hmi_f12b.c
    ../../firmware/src/hmi_fonts/hmi_f14b.c
    ../../firmware/src/hmi_fonts/hmi_f16b.c
    ../../firmware/src/hmi_fonts/hmi_f22b.c
    ../../firmware/src/hmi_fonts/hmi_f28b.c
    ../../firmware/src/hmi_fonts/hmi_hero28.c
)

target_include_directories(hmi_render PRIVATE
    include
    ../../firmware/src
    "${LVGL_ROOT_DIR}"
)

target_link_libraries(hmi_render PRIVATE lvgl)
