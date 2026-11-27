/*
 * Composition root. Start order (see docs/firmware-restructure-design.md, section 4):
 *   1. boot report and memory check (16 MB flash, 8 MB PSRAM); stop here if it does not match
 *   2. task watchdog
 *   3. services: settings (NVS), OTA, Wi-Fi -> OTA wiring on HMI_WIFI_EVENT, Wi-Fi
 *   4. board display, then controller link, UI and touch under one LVGL lock; backlight to the stored level
 *   5. confirm the running image (cancel rollback), then the alive loop
 * Modules do not start each other: every cross-module connection is made here.
 */
#include "app.h"

#include <inttypes.h>
#include <stdbool.h>

#include "esp_err.h"
#include "esp_event.h"
#include "esp_flash.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_lvgl_port.h"
#include "esp_psram.h"
#include "esp_system.h"
#include "esp_task_wdt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "bsp_elecrow_s3.h"
#include "hmi_config.h"
#include "hmi_ota.h"
#include "hmi_power.h"
#include "hmi_settings.h"
#include "hmi_ui.h"
#include "hmi_wifi.h"
#include "rcc_link.h"

static const char *TAG = "app";

static const char *reset_reason_name(esp_reset_reason_t r)
{
    switch (r) {
    case ESP_RST_POWERON:   return "POWERON";
    case ESP_RST_EXT:       return "EXTERNAL_PIN";
    case ESP_RST_SW:        return "SW_RESET";
    case ESP_RST_PANIC:     return "EXCEPTION_PANIC";
    case ESP_RST_INT_WDT:   return "INT_WATCHDOG";
    case ESP_RST_TASK_WDT:  return "TASK_WATCHDOG";
    case ESP_RST_WDT:       return "OTHER_WATCHDOG";
    case ESP_RST_DEEPSLEEP: return "DEEP_SLEEP";
    case ESP_RST_BROWNOUT:  return "BROWNOUT_RESET";
    case ESP_RST_SDIO:      return "SDIO";
    default:                return "UNKNOWN";
    }
}

/* 1. Boot report; false if flash/PSRAM do not match the board. */
static bool boot_check(void)
{
    uint32_t flash_bytes = 0;
    const esp_reset_reason_t rst_reason = esp_reset_reason();
    ESP_LOGI(TAG, "Boot reason: %s (%d)", reset_reason_name(rst_reason), (int)rst_reason);
    ESP_LOGI(TAG, "Elecrow DLE06235B environment bring-up; IDF %s", esp_get_idf_version());
    ESP_ERROR_CHECK(esp_flash_get_physical_size(NULL, &flash_bytes));
    const size_t psram_bytes = esp_psram_get_size();
    ESP_LOGI(TAG, "Flash: %" PRIu32 " bytes; PSRAM: %zu bytes", flash_bytes, psram_bytes);
    if (flash_bytes != 16u * 1024u * 1024u || !esp_psram_is_initialized() ||
        psram_bytes != 8u * 1024u * 1024u) {
        ESP_LOGE(TAG, "Memory configuration does not match the expected 16 MB flash / 8 MB PSRAM");
        return false;
    }
    return true;
}

/* 2. Task watchdog (HMI_TWDT_TIMEOUT_MS, panic). The LVGL task subscribes itself (hmi_shell). */
static void start_watchdog(void)
{
    const esp_task_wdt_config_t twdt_config = {
        .timeout_ms = HMI_TWDT_TIMEOUT_MS,
        .idle_core_mask = 0,
        .trigger_panic = true,
    };
    esp_err_t twdt_err = esp_task_wdt_init(&twdt_config);
    if (twdt_err == ESP_ERR_INVALID_STATE) {
        twdt_err = esp_task_wdt_reconfigure(&twdt_config);
    }
    if (twdt_err != ESP_OK) {
        ESP_LOGW(TAG, "TWDT init: %s", esp_err_to_name(twdt_err));
    } else {
        ESP_LOGI(TAG, "TWDT initialized (%d ms timeout, panic enabled)", HMI_TWDT_TIMEOUT_MS);
    }
}

/* Wi-Fi link -> OTA web server: the server runs while the station has an IP. */
static void on_wifi_link(void *arg, esp_event_base_t base, int32_t id, void *data)
{
    (void)arg; (void)base; (void)data;
    if (id == HMI_WIFI_EVENT_CONNECTED) (void)hmi_ota_start_server();
    else if (id == HMI_WIFI_EVENT_LINK_LOST) hmi_ota_stop_server();
}

/* 3. Settings (NVS) first: Wi-Fi credentials and display settings live there. The OTA handler is
 *    registered before Wi-Fi starts, so the first HMI_WIFI_EVENT_CONNECTED cannot be missed. */
static void start_services(void)
{
    ESP_ERROR_CHECK(hmi_settings_init());
    ESP_ERROR_CHECK(hmi_ota_init());
    /* The default loop must exist before the handler is registered; hmi_wifi_init() accepts an existing one. */
    const esp_err_t loop_err = esp_event_loop_create_default();
    if (loop_err != ESP_ERR_INVALID_STATE) ESP_ERROR_CHECK(loop_err);
    ESP_ERROR_CHECK(esp_event_handler_register(HMI_WIFI_EVENT, ESP_EVENT_ANY_ID, on_wifi_link, NULL));
    if (!hmi_wifi_init()) ESP_LOGW(TAG, "Wi-Fi unavailable; the Wi-Fi screens will say so");
}

/* 4. Board: backlight at its floor, LCD, LVGL port. Then link, UI and touch under one LVGL lock.
 *    A failed link or touch is logged and the UI still runs (the link shows offline). */
static void start_display_and_ui(void)
{
    lv_display_t *display = NULL;
    ESP_ERROR_CHECK(bsp_display_start(&display));
    if (!lvgl_port_lock(1000)) ESP_ERROR_CHECK(ESP_ERR_TIMEOUT);
    const esp_err_t link_err = rcc_link_start();
    if (link_err != ESP_OK) {
        ESP_LOGE(TAG, "rcc_link_start failed: %s", esp_err_to_name(link_err));
    }
    hmi_ui_start(link_err == ESP_OK);
    const esp_err_t touch_err = bsp_touch_start(display, hmi_power_on_touch);
    if (touch_err != ESP_OK) {
        ESP_LOGE(TAG, "Touch init failed: %s", esp_err_to_name(touch_err));
    }
    /* The LVGL task owns rendering; do not render on the initialization stack. */
    lv_obj_invalidate(lv_display_get_screen_active(display));
    lvgl_port_unlock();
    hmi_power_start();
    ESP_LOGI(TAG, "Five-page HMI ready; controller link started on UART%d (TX=GPIO%d, RX=GPIO%d).",
             RCC_LINK_UART_PORT, RCC_LINK_TX_GPIO, RCC_LINK_RX_GPIO);
}

void app_run(void)
{
    if (!boot_check()) return;
    start_watchdog();
    start_services();
    start_display_and_ui();

    /* 5. Everything is up: confirm the running image and cancel rollback. */
    (void)hmi_ota_mark_valid();

    for (;;) {
        ESP_LOGI(TAG, "Alive; free internal heap: %zu; free PSRAM heap: %zu; main stack minimum: %u bytes",
                 heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT),
                 heap_caps_get_free_size(MALLOC_CAP_SPIRAM),
                 (unsigned)uxTaskGetStackHighWaterMark(NULL));
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
