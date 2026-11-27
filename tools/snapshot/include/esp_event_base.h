/* Simulator stand-in for ESP-IDF esp_event_base.h (hmi_wifi.h declares its event base). */
#pragma once
typedef const char *esp_event_base_t;
#define ESP_EVENT_DECLARE_BASE(id) extern esp_event_base_t const id
