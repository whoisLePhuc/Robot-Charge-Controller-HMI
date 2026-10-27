#include "rcc_link.h"

#include <string.h>

/* Elecrow S3 currently has no controller UART mapping: expose read-only offline UI. */
esp_err_t rcc_link_start(void)
{
    return ESP_ERR_NOT_SUPPORTED;
}

void rcc_link_snapshot(rcc_snapshot_t *out)
{
    if (out != NULL) {
        memset(out, 0, sizeof(*out));
    }
}

bool rcc_link_request_start(void)
{
    return false;
}

bool rcc_link_request_stop(void)
{
    return false;
}
