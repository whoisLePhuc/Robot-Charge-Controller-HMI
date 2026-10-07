/* Robot Charge Controller HMI: panel, touch, operational UART and LVGL pages. */
#include "hmi_board.h"
#include "hmi_ui.h"
#include "rcc_codec.h"
#include "rcc_link.h"
#include "esp_lvgl_port.h"

void app_main(void)
{
    const hmi_board_status_t board = hmi_board_init();
    if (board.display != ESP_OK) return;
    const bool link_started = rcc_codec_self_test() && rcc_link_start() == ESP_OK;
    if (lvgl_port_lock(1000)) {
        hmi_ui_start(link_started);
        lvgl_port_unlock();
    }
}
