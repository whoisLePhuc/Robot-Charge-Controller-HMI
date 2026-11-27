#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HMI_LANG_EN = 0,
    HMI_LANG_VI = 1,
} hmi_language_t;

typedef struct {
    uint8_t brightness_pct;   /* 10 .. 100 %, default 100 */
    uint16_t sleep_timeout_s; /* Inactivity seconds, 0 = never, default 180 (3 min) */
    uint8_t dim_pct;          /* Dimmed brightness percentage, default 15 % */
    uint8_t language;         /* hmi_language_t, default 0 (EN) */
} hmi_settings_t;

esp_err_t hmi_settings_init(void);
const hmi_settings_t *hmi_settings_get(void);
esp_err_t hmi_settings_save(const hmi_settings_t *cfg);
esp_err_t hmi_settings_set_brightness(uint8_t pct);
esp_err_t hmi_settings_set_sleep_timeout(uint16_t seconds);
esp_err_t hmi_settings_set_dim_pct(uint8_t pct);
esp_err_t hmi_settings_set_language(uint8_t lang);

#ifdef __cplusplus
}
#endif
