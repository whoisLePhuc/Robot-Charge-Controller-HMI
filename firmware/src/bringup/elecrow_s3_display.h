#pragma once

#include "esp_err.h"

/* ST77922 QSPI chart plus stage 3 touch targets; no controller commands. */
esp_err_t elecrow_s3_display_start(void);
