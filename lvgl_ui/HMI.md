# Robot Charge Controller HMI trong LVGL Pro

Mở thư mục `lvgl_ui` trong LVGL Pro Editor. Chọn một file trong `screens/` để xem và chỉnh ở độ phân giải 480 × 320. Màn hình được dựng từ cây widget của simulator firmware. Bạn có thể chọn riêng từng vùng, nút, nhãn hoặc icon để chỉnh trong Design mode hoặc XML.

| Nội dung | File XML |
| --- | --- |
| Năm trang chính | `hmi_overview`, `hmi_measure`, `hmi_faults`, `hmi_session`, `hmi_device` |
| Trạng thái HMI | Hậu tố `_idle`, `_precheck`, `_inhibited`, `_fault`, `_offline`, `_remote`; các trang không có hậu tố dùng dữ liệu Charging |
| Tab Device | `hmi_device_link`, `hmi_device_settings` |
| Xác nhận và kết quả lệnh | `hmi_confirm`, `hmi_command_*` |
| Wi-Fi và bàn phím | `hmi_wifi_*` |
| Tiến độ OTA | `hmi_ota_*` |

Trong chế độ Preview, thanh điều hướng chuyển giữa năm trang. Có thể chạy thử luồng START từ `hmi_overview_idle` và luồng Wi-Fi từ `hmi_device_settings`. Các trạng thái kết quả khác mở trực tiếp từ danh sách XML.

## Chỉnh giao diện firmware

Các component trong `components/hmi/` là bố cục được firmware sử dụng. Các nhóm `rcc_overview_*`, `rcc_measure_*`, `rcc_faults_*`, `rcc_session_*`, `rcc_device_*`, `rcc_wifi_*` và `rcc_shell_*` tương ứng với từng phần giao diện. Card điện áp, dòng và công suất dùng `rcc_telemetry_card`.

Mở component cần chỉnh trong LVGL Pro, chỉnh bố cục rồi bấm Ctrl+B để xuất lại mã C. Build firmware bằng `idf.py build`. Giữ tên các widget đã có để firmware cập nhật dữ liệu và gắn callback. Các màn hình theo trạng thái trong `screens/` cung cấp dữ liệu mẫu để xem trước.

Firmware sử dụng `CONFIG_LV_USE_OBJ_NAME=y` và `CONFIG_LV_MEM_SIZE=131072`. Nếu đã có `sdkconfig` từ trước, cập nhật hai cấu hình này bằng `idf.py menuconfig` trước khi build.
