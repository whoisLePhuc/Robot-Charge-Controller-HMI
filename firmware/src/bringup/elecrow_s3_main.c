/* Elecrow S3 entry point: memory check, watchdog, settings, Wi-Fi/OTA, then LCD, touch, HMI and RCC UART link. */
#include <inttypes.h>

#include "esp_err.h"
#include "esp_flash.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_psram.h"
#include "esp_system.h"
#include "esp_task_wdt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "elecrow_s3_display.h"
#include "hmi_settings.h"
#include "hmi_wifi.h"
#include "hmi_ota.h"

static const char *TAG = "elecrow_s3";

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

void app_main(void)
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
        return;
    }

    /* Initialize Task Watchdog Timer (TWDT): 5000 ms timeout */
    const esp_task_wdt_config_t twdt_config = {
        .timeout_ms = 5000,
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
        ESP_LOGI(TAG, "TWDT initialized (5000 ms timeout, panic enabled)");
    }

    /* Initialize NVS and load persistent configuration */
    ESP_ERROR_CHECK(hmi_settings_init());
    ESP_ERROR_CHECK(hmi_ota_init());
    if (!hmi_wifi_init()) ESP_LOGW(TAG, "Wi-Fi unavailable; the Wi-Fi screens will say so");

    ESP_ERROR_CHECK(elecrow_s3_display_start());
    ESP_LOGI(TAG, "Five-page HMI ready; controller link started on UART1 (TX=GPIO43, RX=GPIO44).");

    /* Confirm running firmware image and cancel rollback */
    (void)hmi_ota_mark_valid();

    for (;;) {
        ESP_LOGI(TAG, "Alive; free internal heap: %zu; free PSRAM heap: %zu; main stack minimum: %u bytes",
                 heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT),
                 heap_caps_get_free_size(MALLOC_CAP_SPIRAM),
                 (unsigned)uxTaskGetStackHighWaterMark(NULL));
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
