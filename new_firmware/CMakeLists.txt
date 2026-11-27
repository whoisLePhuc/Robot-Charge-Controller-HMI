# HMI firmware for the Elecrow DLE06235B / ES3C35P ESP32-S3 display, built with idf.py (ESP-IDF 6.1.0).
# sdkconfig.defaults, partitions.csv and dependencies.lock in this directory are picked up by IDF's defaults.
cmake_minimum_required(VERSION 3.22)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(firmware)
