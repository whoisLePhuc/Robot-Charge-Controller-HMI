# HMI firmware for the Elecrow DLE06235B / ES3C35P ESP32-S3 display, built with idf.py (ESP-IDF 6.1.0).
# sdkconfig.defaults, partitions.csv and dependencies.lock in this directory are picked up by IDF's defaults.
cmake_minimum_required(VERSION 3.22)

# Components live under components/<layer>/<module>; see components/README.md for the layering rules.
set(EXTRA_COMPONENT_DIRS
    "${CMAKE_CURRENT_LIST_DIR}/components/common"
    "${CMAKE_CURRENT_LIST_DIR}/components/bsp"
    "${CMAKE_CURRENT_LIST_DIR}/components/protocol"
    "${CMAKE_CURRENT_LIST_DIR}/components/services")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(firmware)
