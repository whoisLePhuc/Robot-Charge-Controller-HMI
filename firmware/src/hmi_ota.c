#include "hmi_ota.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_partition.h"
#include "esp_http_server.h"
#include "esp_system.h"
#include "esp_app_desc.h"
#include "esp_app_format.h"
#include "esp_heap_caps.h"
#include "esp_random.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

static const char *TAG = "hmi_ota";

#define OTA_BUF_SIZE 2048

static httpd_handle_t s_server = NULL;
static SemaphoreHandle_t s_lock = NULL;
static hmi_ota_status_t s_status;
static esp_ota_handle_t s_ota_handle = 0;
static const esp_partition_t *s_update_partition = NULL;
static int64_t s_arm_deadline_us;
static uint8_t s_pin_fails;

/* ---- upload authorization (caller holds s_lock) */
static void disarm_locked(void)
{
    if (!s_status.armed) return;
    s_status.armed = false;
    memset(s_status.pin, 0, sizeof(s_status.pin));
    s_pin_fails = 0;
    s_status.arm_seq++;
    s_status.version++;
}

static void expire_locked(void)
{
    if (s_status.armed && esp_timer_get_time() >= s_arm_deadline_us) {
        ESP_LOGI(TAG, "OTA upload window expired");
        disarm_locked();
    }
}

static bool pin_matches(const char *given)
{
    uint8_t diff = 0;
    for (int i = 0; i < HMI_OTA_PIN_LEN; ++i) diff |= (uint8_t)(given[i] ^ s_status.pin[i]);
    return diff == 0 && given[HMI_OTA_PIN_LEN] == '\0';
}

/* True when authorized (the PIN is then consumed); otherwise *code and *why describe the refusal. */
static bool authorize_upload(httpd_req_t *req, httpd_err_code_t *code, const char **why)
{
    char given[16] = {0};
    const bool have = httpd_req_get_hdr_value_str(req, "X-OTA-PIN", given, sizeof(given)) == ESP_OK;
    bool ok = false;
    xSemaphoreTake(s_lock, portMAX_DELAY);
    expire_locked();
    if (!s_status.armed) {
        *code = HTTPD_403_FORBIDDEN;
        *why = "OTA locked: tap ENABLE OTA on the HMI Wi-Fi screen";
    } else if (!have || !pin_matches(given)) {
        *code = HTTPD_401_UNAUTHORIZED;
        *why = "Wrong or missing PIN";
        if (++s_pin_fails >= HMI_OTA_MAX_PIN_FAILS) {
            ESP_LOGW(TAG, "Too many wrong OTA PINs; upload window closed");
            disarm_locked();
            *why = "Too many wrong PINs: OTA locked, enable it again on the HMI";
        }
    } else {
        disarm_locked();                      /* one-time PIN */
        ok = true;
    }
    xSemaphoreGive(s_lock);
    return ok;
}

static void update_status_slots(void)
{
    const esp_partition_t *running = esp_ota_get_running_partition();
    if (running) {
        snprintf(s_status.running_slot, sizeof(s_status.running_slot), "%s", running->label);
    } else {
        snprintf(s_status.running_slot, sizeof(s_status.running_slot), "unknown");
    }

    const esp_partition_t *next = esp_ota_get_next_update_partition(NULL);
    if (next) {
        snprintf(s_status.next_slot, sizeof(s_status.next_slot), "%s", next->label);
    } else {
        snprintf(s_status.next_slot, sizeof(s_status.next_slot), "none");
    }
    /* PROJECT_VER, or `git describe` of the build tree when unset. */
    snprintf(s_status.running_version, sizeof(s_status.running_version), "%s", esp_app_get_description()->version);
}

static void delayed_restart_task(void *param)
{
    (void)param;
    ESP_LOGI(TAG, "Rebooting into newly flashed firmware partition in 2 seconds...");
    vTaskDelay(pdMS_TO_TICKS(2000));
    esp_restart();
}

static const char INDEX_HTML[] = 
"<!DOCTYPE html>\n"
"<html lang='en'>\n"
"<head>\n"
"<meta charset='utf-8'>\n"
"<meta name='viewport' content='width=device-width,initial-scale=1'>\n"
"<title>RCC HMI — Web OTA Update</title>\n"
"<style>\n"
"* { box-sizing: border-box; margin: 0; padding: 0; font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; }\n"
"body { background: #030712; color: #f3f4f6; padding: 20px; display: flex; justify-content: center; }\n"
".container { max-width: 540px; width: 100%; }\n"
".card { background: #0b1120; border: 1px solid #1e293b; border-radius: 12px; padding: 20px; margin-bottom: 16px; box-shadow: 0 4px 16px rgba(0,0,0,0.5); }\n"
".header { display: flex; align-items: center; justify-content: space-between; border-bottom: 1px solid #1e293b; padding-bottom: 12px; margin-bottom: 16px; }\n"
".title { font-size: 16px; font-weight: 700; color: #38bdf8; letter-spacing: 0.5px; }\n"
".badge { font-size: 11px; font-weight: 600; padding: 3px 8px; border-radius: 9999px; background: #0369a1; color: #e0f2fe; }\n"
".grid { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; margin-bottom: 16px; }\n"
".info-box { background: #070d19; border: 1px solid #1e293b; border-radius: 8px; padding: 10px; }\n"
".info-label { font-size: 10px; font-weight: 600; color: #94a3b8; text-transform: uppercase; margin-bottom: 4px; }\n"
".info-val { font-size: 14px; font-weight: 700; color: #f8fafc; font-family: monospace; }\n"
".drop-zone { border: 2px dashed #334155; border-radius: 10px; padding: 24px; text-align: center; cursor: pointer; transition: 0.2s; background: #070d19; margin-bottom: 16px; }\n"
".drop-zone:hover { border-color: #38bdf8; background: #0c1829; }\n"
".drop-zone input { display: none; }\n"
".pin { width: 100%; padding: 11px; margin-bottom: 12px; background: #070d19; color: #f8fafc; border: 1px solid #334155; border-radius: 8px; font-size: 15px; letter-spacing: 4px; text-align: center; }\n"
".btn { width: 100%; padding: 12px; background: #0284c7; color: white; border: none; border-radius: 8px; font-size: 14px; font-weight: 600; cursor: pointer; transition: 0.2s; }\n"
".btn:hover { background: #0369a1; }\n"
".btn:disabled { background: #334155; color: #64748b; cursor: not-allowed; }\n"
".progress-wrap { display: none; margin-top: 16px; }\n"
".bar-bg { background: #1e293b; border-radius: 6px; height: 14px; overflow: hidden; margin-bottom: 8px; }\n"
".bar-fill { height: 100%; width: 0%; background: #38bdf8; transition: width 0.1s linear; }\n"
".status-txt { font-size: 12px; font-weight: 500; color: #94a3b8; text-align: center; }\n"
".alert { display: none; margin-top: 14px; padding: 12px; border-radius: 8px; font-size: 13px; font-weight: 500; text-align: center; }\n"
".alert-ok { background: #064e3b; border: 1px solid #059669; color: #a7f3d0; }\n"
".alert-err { background: #450a0a; border: 1px solid #dc2626; color: #fecaca; }\n"
"</style>\n"
"</head>\n"
"<body>\n"
"<div class='container'>\n"
"  <div class='card'>\n"
"    <div class='header'>\n"
"      <span class='title'>RCC HMI FIRMWARE UPDATE</span>\n"
"      <span class='badge'>DUAL-OTA</span>\n"
"    </div>\n"
"    <div class='grid'>\n"
"      <div class='info-box'><div class='info-label'>Active Slot</div><div class='info-val' id='slot-cur'>...</div></div>\n"
"      <div class='info-box'><div class='info-label'>Target Slot</div><div class='info-val' id='slot-next' style='color:#38bdf8;'>...</div></div>\n"
"      <div class='info-box'><div class='info-label'>Firmware</div><div class='info-val' id='fw-ver'>...</div></div>\n"
"      <div class='info-box'><div class='info-label'>Free Internal RAM</div><div class='info-val' id='ram-free'>...</div></div>\n"
"    </div>\n"
"    <div class='drop-zone' onclick=\"document.getElementById('file-input').click()\">\n"
"      <input type='file' id='file-input' accept='.bin' onchange='fileSelected(this)'>\n"
"      <p id='file-name' style='font-size:13px; color:#cbd5e1;'>Click to choose <b>firmware.bin</b></p>\n"
"      <p style='font-size:11px; color:#64748b; margin-top:4px;'>Supports official ESP32-S3 application binary</p>\n"
"    </div>\n"
"    <p id='lock-msg' style='font-size:12px; color:#f59e0b; margin-bottom:10px; display:none;'>OTA is locked. On the HMI open Wi-Fi, tap the connected network, then ENABLE OTA.</p>\n"
"    <input class='pin' id='pin' type='password' inputmode='numeric' autocomplete='off' maxlength='6' placeholder='6-digit PIN shown on the HMI' oninput='updateBtn()'>\n"
"    <button class='btn' id='btn-upload' disabled onclick='uploadFirmware()'>Upload & Flash Firmware</button>\n"
"    <div class='progress-wrap' id='prog-wrap'>\n"
"      <div class='bar-bg'><div class='bar-fill' id='bar-fill'></div></div>\n"
"      <div class='status-txt' id='status-txt'>Uploading 0%...</div>\n"
"    </div>\n"
"    <div class='alert' id='alert-box'></div>\n"
"  </div>\n"
"</div>\n"
"<script>\n"
"let selectedFile = null;\n"
"function loadStatus() {\n"
"  fetch('/api/status').then(r => r.json()).then(d => {\n"
"    document.getElementById('slot-cur').innerText = d.running_slot || 'ota_0';\n"
"    document.getElementById('slot-next').innerText = d.next_slot || 'ota_1';\n"
"    document.getElementById('fw-ver').innerText = d.version || 'unknown';\n"
"    document.getElementById('ram-free').innerText = (d.heap_free ? Math.round(d.heap_free/1024) + ' KB' : 'N/A');\n"
"    document.getElementById('lock-msg').style.display = d.armed ? 'none' : 'block';\n"
"  }).catch(() => {});\n"
"}\n"
"loadStatus();\n"
"setInterval(loadStatus, 5000);\n"
"function pinOk() { return /^[0-9]{6}$/.test(document.getElementById('pin').value); }\n"
"function updateBtn() { document.getElementById('btn-upload').disabled = !(selectedFile && pinOk()); }\n"
"function fileSelected(inp) {\n"
"  if (inp.files && inp.files[0]) {\n"
"    selectedFile = inp.files[0];\n"
"    const fn = document.getElementById('file-name');\n"
"    fn.textContent = 'Selected: ' + selectedFile.name + ' (' + (selectedFile.size/1024).toFixed(1) + ' KB)';\n"
"    updateBtn();\n"
"  }\n"
"}\n"
"function uploadFirmware() {\n"
"  if (!selectedFile || !pinOk()) return;\n"
"  const btn = document.getElementById('btn-upload');\n"
"  btn.disabled = true;\n"
"  const pw = document.getElementById('prog-wrap');\n"
"  const bf = document.getElementById('bar-fill');\n"
"  const st = document.getElementById('status-txt');\n"
"  const al = document.getElementById('alert-box');\n"
"  pw.style.display = 'block';\n"
"  al.style.display = 'none';\n"
"  const xhr = new XMLHttpRequest();\n"
"  xhr.open('POST', '/update', true);\n"
"  xhr.setRequestHeader('X-OTA-PIN', document.getElementById('pin').value);\n"
"  xhr.upload.onprogress = function(e) {\n"
"    if (e.lengthComputable) {\n"
"      const p = Math.round((e.loaded / e.total) * 100);\n"
"      bf.style.width = p + '%';\n"
"      st.innerText = 'Writing flash: ' + p + '% (' + (e.loaded/1024).toFixed(0) + ' / ' + (e.total/1024).toFixed(0) + ' KB)';\n"
"    }\n"
"  };\n"
"  xhr.onload = function() {\n"
"    if (xhr.status === 200) {\n"
"      bf.style.width = '100%';\n"
"      st.innerText = 'Verification complete!';\n"
"      al.className = 'alert alert-ok';\n"
"      al.innerText = 'Firmware flashed successfully! Rebooting station into new partition in 2 seconds...';\n"
"      al.style.display = 'block';\n"
"      setTimeout(() => { location.reload(); }, 6000);\n"
"    } else {\n"
"      al.className = 'alert alert-err';\n"
"      al.innerText = (xhr.status === 401 || xhr.status === 403 ? 'Refused: ' : 'Flash failed: ') + (xhr.responseText || 'Error ' + xhr.status);\n"
"      al.style.display = 'block';\n"
"      pw.style.display = 'none';\n"
"      document.getElementById('pin').value = '';\n"
"      loadStatus();\n"
"      updateBtn();\n"
"    }\n"
"  };\n"
"  xhr.onerror = function() {\n"
"    al.className = 'alert alert-err';\n"
"    al.innerText = 'Network error during upload. The PIN was used up if the upload started; enable OTA again on the HMI.';\n"
"    al.style.display = 'block';\n"
"    document.getElementById('pin').value = '';\n"
"    updateBtn();\n"
"  };\n"
"  xhr.send(selectedFile);\n"
"}\n"
"</script>\n"
"</body>\n"
"</html>\n";

static esp_err_t index_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    return httpd_resp_send(req, INDEX_HTML, sizeof(INDEX_HTML) - 1);
}

static esp_err_t status_get_handler(httpd_req_t *req)
{
    char buf[288];
    xSemaphoreTake(s_lock, portMAX_DELAY);
    update_status_slots();
    expire_locked();
    const size_t free_ram = heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    snprintf(buf, sizeof(buf),
             "{\"running_slot\":\"%s\",\"next_slot\":\"%s\",\"version\":\"%s\",\"heap_free\":%zu,\"state\":%d,\"progress\":%u,\"armed\":%s}",
             s_status.running_slot, s_status.next_slot, s_status.running_version, free_ram, (int)s_status.state, s_status.progress_pct,
             s_status.armed ? "true" : "false");
    xSemaphoreGive(s_lock);
    httpd_resp_set_type(req, "application/json");
    return httpd_resp_send(req, buf, strlen(buf));
}

static esp_err_t update_post_handler(httpd_req_t *req)
{
    httpd_err_code_t deny_code;
    const char *deny_why;
    if (!authorize_upload(req, &deny_code, &deny_why)) {
        ESP_LOGW(TAG, "OTA upload refused: %s", deny_why);
        /* ESP_OK lets httpd drain the body so the browser receives this status. */
        httpd_resp_send_err(req, deny_code, deny_why);
        return ESP_OK;
    }

    char *ota_buf = malloc(OTA_BUF_SIZE);
    if (!ota_buf) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Out of internal heap memory");
        return ESP_FAIL;
    }

    const int total_bytes = req->content_len;
    if (total_bytes <= 0) {
        free(ota_buf);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Content-Length header required");
        return ESP_FAIL;
    }

    s_update_partition = esp_ota_get_next_update_partition(NULL);
    if (!s_update_partition) {
        free(ota_buf);
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "No OTA partition available");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Starting OTA update to partition '%s' (0x%08" PRIx32 ", size: %" PRIu32 " bytes). File size: %d bytes",
             s_update_partition->label, s_update_partition->address, s_update_partition->size, total_bytes);

    if (xSemaphoreTake(s_lock, portMAX_DELAY) == pdTRUE) {
        s_status.state = HMI_OTA_RECEIVING;
        s_status.progress_pct = 0;
        s_status.bytes_written = 0;
        s_status.total_bytes = (size_t)total_bytes;
        s_status.error_msg[0] = '\0';
        s_status.version++;
        xSemaphoreGive(s_lock);
    }

    int remaining = total_bytes;
    bool is_first_chunk = true;
    esp_err_t err = ESP_OK;

    while (remaining > 0) {
        const int to_read = remaining > OTA_BUF_SIZE ? OTA_BUF_SIZE : remaining;
        const int received = httpd_req_recv(req, ota_buf, to_read);
        if (received <= 0) {
            if (received == HTTPD_SOCK_ERR_TIMEOUT) {
                continue;
            }
            ESP_LOGE(TAG, "HTTP receive error (%d)", received);
            err = ESP_FAIL;
            break;
        }

        if (is_first_chunk) {
            is_first_chunk = false;
            if (received < 1 || (uint8_t)ota_buf[0] != ESP_IMAGE_HEADER_MAGIC) {
                ESP_LOGE(TAG, "Invalid binary magic byte: 0x%02X (expected 0xE9)", (uint8_t)ota_buf[0]);
                err = ESP_ERR_OTA_VALIDATE_FAILED;
                break;
            }
            err = esp_ota_begin(s_update_partition, OTA_WITH_SEQUENTIAL_WRITES, &s_ota_handle);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "esp_ota_begin failed: %s", esp_err_to_name(err));
                break;
            }
        }

        err = esp_ota_write(s_ota_handle, ota_buf, (size_t)received);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "esp_ota_write failed: %s", esp_err_to_name(err));
            break;
        }

        remaining -= received;
        if (xSemaphoreTake(s_lock, portMAX_DELAY) == pdTRUE) {
            s_status.bytes_written += (size_t)received;
            s_status.progress_pct = (uint8_t)((s_status.bytes_written * 100) / (size_t)total_bytes);
            s_status.version++;
            xSemaphoreGive(s_lock);
        }
    }

    free(ota_buf);

    if (err == ESP_OK) {
        if (xSemaphoreTake(s_lock, portMAX_DELAY) == pdTRUE) {
            s_status.state = HMI_OTA_VERIFYING;
            s_status.version++;
            xSemaphoreGive(s_lock);
        }
        err = esp_ota_end(s_ota_handle);
        s_ota_handle = 0;
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "esp_ota_end validation failed: %s", esp_err_to_name(err));
        } else {
            err = esp_ota_set_boot_partition(s_update_partition);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "esp_ota_set_boot_partition failed: %s", esp_err_to_name(err));
            } else {
                ESP_LOGI(TAG, "OTA upgrade successful! Next boot partition: '%s'", s_update_partition->label);
            }
        }
    } else {
        if (s_ota_handle != 0) {
            esp_ota_abort(s_ota_handle);
            s_ota_handle = 0;
        }
    }

    if (xSemaphoreTake(s_lock, portMAX_DELAY) == pdTRUE) {
        if (err == ESP_OK) {
            s_status.state = HMI_OTA_SUCCESS;
            s_status.progress_pct = 100;
        } else {
            s_status.state = HMI_OTA_FAILED;
            snprintf(s_status.error_msg, sizeof(s_status.error_msg), "%s", esp_err_to_name(err));
        }
        s_status.version++;
        xSemaphoreGive(s_lock);
    }

    if (err == ESP_OK) {
        httpd_resp_set_type(req, "application/json");
        httpd_resp_send(req, "{\"status\":\"ok\"}", 15);
        xTaskCreate(delayed_restart_task, "ota_reboot", 3072, NULL, 5, NULL);
        return ESP_OK;
    }

    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, esp_err_to_name(err));
    return ESP_FAIL;
}

esp_err_t hmi_ota_init(void)
{
    if (s_lock) return ESP_OK;
    s_lock = xSemaphoreCreateMutex();
    if (!s_lock) return ESP_ERR_NO_MEM;
    update_status_slots();
    return ESP_OK;
}

void hmi_ota_arm(void)
{
    if (!s_lock) return;
    xSemaphoreTake(s_lock, portMAX_DELAY);
    /* Rejection sampling keeps the 6-digit PIN uniform. */
    uint32_t r;
    do { r = esp_random(); } while (r >= 4294000000u);
    snprintf(s_status.pin, sizeof(s_status.pin), "%06" PRIu32, r % 1000000u);
    s_status.armed = true;
    s_pin_fails = 0;
    s_arm_deadline_us = esp_timer_get_time() + (int64_t)HMI_OTA_ARM_WINDOW_S * 1000000;
    s_status.arm_seq++;
    s_status.version++;
    xSemaphoreGive(s_lock);
    ESP_LOGI(TAG, "OTA upload window opened for %d s", HMI_OTA_ARM_WINDOW_S);   /* PIN is not logged */
}

void hmi_ota_disarm(void)
{
    if (!s_lock) return;
    xSemaphoreTake(s_lock, portMAX_DELAY);
    disarm_locked();
    xSemaphoreGive(s_lock);
}

esp_err_t hmi_ota_start_server(void)
{
    if (s_server) {
        return ESP_OK;
    }
    if (!s_lock) {
        return ESP_ERR_INVALID_STATE;
    }

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 80;
    config.max_uri_handlers = 8;
    config.stack_size = 5120;
    config.lru_purge_enable = true;

    ESP_LOGI(TAG, "Starting Web OTA HTTP server on port 80...");
    esp_err_t err = httpd_start(&s_server, &config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start HTTP server: %s", esp_err_to_name(err));
        return err;
    }

    const httpd_uri_t uri_get_index = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = index_get_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(s_server, &uri_get_index);

    const httpd_uri_t uri_get_update = {
        .uri = "/update",
        .method = HTTP_GET,
        .handler = index_get_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(s_server, &uri_get_update);

    const httpd_uri_t uri_get_status = {
        .uri = "/api/status",
        .method = HTTP_GET,
        .handler = status_get_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(s_server, &uri_get_status);

    const httpd_uri_t uri_post_update = {
        .uri = "/update",
        .method = HTTP_POST,
        .handler = update_post_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(s_server, &uri_post_update);

    ESP_LOGI(TAG, "Web OTA server ready at http://<ip>/");
    return ESP_OK;
}

void hmi_ota_stop_server(void)
{
    if (s_server) {
        ESP_LOGI(TAG, "Stopping Web OTA HTTP server...");
        httpd_stop(s_server);
        s_server = NULL;
    }
    hmi_ota_disarm();
}

bool hmi_ota_server_is_running(void)
{
    return s_server != NULL;
}

void hmi_ota_get_status(hmi_ota_status_t *out)
{
    if (!out) return;
    if (s_lock && xSemaphoreTake(s_lock, pdMS_TO_TICKS(50)) == pdTRUE) {
        expire_locked();
        *out = s_status;
        xSemaphoreGive(s_lock);
    } else {
        *out = s_status;
    }
}

esp_err_t hmi_ota_mark_valid(void)
{
    const esp_partition_t *running = esp_ota_get_running_partition();
    if (!running) return ESP_FAIL;
    ESP_LOGI(TAG, "Running firmware partition: '%s' (0x%08" PRIx32 ")", running->label, running->address);

    esp_ota_img_states_t ota_state;
    if (esp_ota_get_state_partition(running, &ota_state) == ESP_OK) {
        if (ota_state == ESP_OTA_IMG_PENDING_VERIFY) {
            ESP_LOGI(TAG, "Newly flashed OTA image detected! Validating image and cancelling rollback...");
            esp_err_t err = esp_ota_mark_app_valid_cancel_rollback();
            if (err == ESP_OK) {
                ESP_LOGI(TAG, "App successfully marked VALID. Rollback cancelled.");
            } else {
                ESP_LOGE(TAG, "Failed to mark app valid: %s", esp_err_to_name(err));
            }
            return err;
        } else if (ota_state == ESP_OTA_IMG_VALID) {
            ESP_LOGI(TAG, "Current firmware partition is already marked VALID.");
        }
    }
    return ESP_OK;
}
