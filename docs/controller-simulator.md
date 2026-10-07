# Controller giả lập qua UART

Chương trình firmware/tools/controller_sim.py chạy trên PC và đóng vai RCC ở đầu
bên kia UART0 của màn hình. Màn hình chạy firmware HMI thật; các lệnh PING, GET_*,
START/STOP được truyền qua USB/CH340C và trả lời bằng giao thức nhị phân RCC V1.
Không cần build hoặc nạp thêm firmware để chạy chương trình này.

Khác với firmware/tools/sim/ (render UI trên PC), chương trình này kiểm tra UART,
codec, polling, kết quả lệnh và cập nhật LVGL trên màn hình thật.

## 1. Kết nối và khởi chạy

1. Màn hình đã được nạp firmware HMI, kết nối USB và hiện ở COM4.
2. Giữ controller thật ngắt khỏi P1 TX/RX. PC là peer duy nhất trong phép thử này.
3. Đóng PlatformIO Serial Monitor và các chương trình đang giữ COM4. Không upload
   firmware khi simulator đang chạy.
4. Mở terminal PowerShell:

~~~powershell
cd D:\Projects\Robot_Charge_Controller\Robot-Charge-Controller-HMI\firmware
& "$env:USERPROFILE\.platformio\penv\Scripts\python.exe" tools/controller_sim.py --port COM4
~~~

Python của PlatformIO trên máy hiện tại đã có pyserial 3.5. Nếu dùng Python khác:

~~~powershell
python -m pip install -r tools/controller_sim_requirements.txt
python tools/controller_sim.py --port COM4
~~~

Chương trình giữ DTR/RTS ở mức deasserted trước khi mở cổng để tránh chủ động kích
hoạt auto-reset/BOOT. Một số driver/USB bridge vẫn có thể tạo xung ngắn khi mở cổng;
nếu màn hình không chạy, nhấn RESET một lần sau khi simulator đã mở.

Mặc định: OPERATIONAL/IDLE, VOUT 48 V, IOUT 0 A, không fault/inhibit và cho phép
UART START/STOP. Chờ vài giây để polling cập nhật cả Device, Measure và Faults.
Image ID giả lập là ASCII SIM-RCC1 (Device hiển thị các byte dạng hex),
build flags 0x07, running links 0x04. Header HMI có thể chỉ hiện ONLINE vì giao thức
hiện tại không có cờ simulator. Các con số của phép thử này là dữ liệu mô phỏng.

Nhập help để xem lệnh, status để xem mô hình. quit hoặc Ctrl+C đóng COM4.

## 2. Thử START và STOP

Ở trạng thái mặc định, chạm START trên màn hình và xác nhận:

- Simulator trả ACCEPTED với đúng request_id.
- PRECHECK → RELAY_CLOSING sau 300 ms → CHARGE_VERIFY sau 600 ms.
- Sau khoảng 1,3 s, đặt CHARGING_ESTABLISHED, thêm event CHARGE_ESTABLISHED,
  trả COMPLETED cho START. IOUT lúc này mặc định 2 A.
- Chạm STOP: relay command OFF trước reply, kết thúc phiên, đặt REMOTE_INHIBIT,
  trả ACCEPTED rồi COMPLETED sau khoảng 80 ms.
- Contact feedback luôn UNKNOWN (0), như board hiện tại.

Các thời gian trên là lựa chọn của test double để thấy chuyển trạng thái, không phải
timing budget hoặc thời gian đã đo của controller thật. Polling có thể không hiển thị
từng trạng thái trung gian.

Sau STOP, có thể START từ INHIBITED nếu UI cho phép, hoặc nhập ready để cưỡng bức
mô hình trở về IDLE.

## 3. Các lệnh terminal

Nhập mỗi lệnh trên một dòng:

| Lệnh | Tác dụng trong mô hình |
|---|---|
| ready | Relay OFF, IDLE, xóa fault/inhibit mô phỏng |
| charging | Cưỡng bức một phiên CHARGING để kiểm tra UI |
| complete | Kết thúc phiên với reason COMPLETE |
| fault overcurrent | Fault 0x0402, mask bit11; relay OFF |
| fault voltage / fault reverse / fault adc-stale | Các fault tương ứng |
| fault internal / fault nvs / fault not-established | Thêm các fault để thử nhãn UI |
| fault clear | Xóa fault mô phỏng |
| inhibit remote / inhibit reset / inhibit recovery | Mask bit0/bit1/bit2; kết thúc phiên |
| inhibit clear | Xóa inhibit mô phỏng |
| authority deny | START/STOP trả REJECTED/UNAUTHORIZED_SOURCE; GET_* vẫn hoạt động |
| authority allow | Cho phép START/STOP lại |
| link silence | Dừng toàn bộ reply; phiên mô phỏng tiếp tục |
| link normal | Phục hồi reply cho các yêu cầu mới |
| measurement invalid | Trả dữ liệu với status bits không hợp lệ |
| measurement stale | Age 2000 ms, bỏ fresh bit của cả hai kênh |
| measurement floor | VOUT ≤ 3,380 V, BELOW_FLOOR; START bị CHARGER_REQUEST_ABSENT |
| measurement missing | GET_MEASUREMENTS trả FAILED/MEASUREMENT_INVALID, không data |
| measurement valid | Phục hồi status bits hợp lệ |
| values 48000 -500 | VOUT 48 V, IOUT -0,5 A trong CHARGING/VERIFY |
| events 40 | Thêm 40 sự kiện để thử pagination và overwritten/missed |
| reboot | Đổi boot_id, mất log/ledger, hủy lệnh đang chờ, relay OFF |
| status | In trạng thái hiện tại |
| quit | Thoát, giải phóng COM4 |

ready/fault clear/inhibit clear điều khiển test model trên PC, không phải service
commands mà HMI gửi. authority deny mô phỏng interface không có quyền dù node trust
flag vẫn bật. Sau authority allow, cảnh báo REJECTED trước đó có thể còn hiện trên
HMI khoảng 15 s.

## 4. Tiêm lỗi luồng lệnh và đường truyền

Đặt chế độ trước khi chạm START/STOP:

| Lệnh | Kết quả mong muốn |
|---|---|
| control accepted-only | START chỉ ACCEPTED, PRECHECK/relay OFF; HMI NOT CONFIRMED sau timeout 15 s |
| control start-fail | START ACCEPTED rồi FAILED/CHARGE_NOT_ESTABLISHED, relay OFF, RECOVERY_INHIBIT |
| control stop-unconfirmed | STOP tác dụng trong mô hình nhưng mọi STOP result bị bỏ; GET_STATUS vẫn trả lời |
| control stop-fail | STOP relay OFF nhưng FAILED/PERSISTENCE_FAILED |
| control stop-queue-full | STOP relay OFF nhưng REJECTED/QUEUE_FULL |
| control normal | Phục hồi luồng lệnh bình thường |
| wire bad-crc | Gửi mọi reply với CRC16 sai; HMI phải loại bỏ |
| wire drop-next | Bỏ đúng một reply kế tiếp; thử retry cùng request_id |
| wire normal | Phục hồi CRC và hủy drop-next |
| delay 100 | Trì hoãn mọi reply 100 ms; vượt timeout mỗi attempt của HMI |
| delay 0 | Phục hồi phản hồi ngay |

accepted-only cố ý không phát terminal result để thử timeout. Đổi control normal
không tự hoàn tất START đã treo: dùng STOP hoặc reboot.
stop-unconfirmed bỏ cả ACCEPTED lẫn terminal STOP result: HMI có thể NOT CONFIRMED
sau khoảng 150 ms trong khi trang trạng thái đã thấy relay OFF.

Khôi phục một phép thử START bình thường, nhập từng dòng:

~~~text
control normal
link normal
wire normal
delay 0
measurement valid
ready
~~~

Mất link không tự ngắt phiên. Trên màn hình, số đo/trạng thái phải được đánh dấu
không còn hiện tại; kết quả STOP không được suy ra từ việc đã gửi.

## 5. Tùy chọn khi chạy

~~~powershell
python tools/controller_sim.py --port COM4 --scenario charging
python tools/controller_sim.py --port COM4 --scenario fault
python tools/controller_sim.py --port COM4 --verbose --no-console --duration 30
~~~

Các scenario: ready (mặc định), charging, fault, inhibited, unauthorized.
Mặc định chỉ log START/STOP; --verbose log cả polling.
--duration giới hạn số giây chạy; mặc định chạy đến quit/Ctrl+C.
Khi thoát, in số request, reply, invalid frame và reply bị drop.

## 6. Giao thức và giới hạn

- COBS + CRC16/CCITT-FALSE cho frame, CRC32/ISO-HDLC cho object.
- Controller address 0x01, reply đến source request (HMI hiện dùng 0x02).
  Bỏ broadcast, sai địa chỉ/CRC/version/length, ID 0, source 0x01/0xFF.
- Parser có giới hạn body/object, kiểm tra fragment order, reassembly hết hạn 2 s.
- Query schemas: Device 40, Status 48, Measure 32, Faults 12 bytes.
- Event log 32 entry, page entries tối đa 224 bytes, hỗ trợ OBJECT_FIRST/OBJECT_DATA.
  max_count=0 lấy nhiều nhất vừa page.
- Cùng source/request_id/nội dung START/STOP trả cached result, không thực hiện lại.
  Khác nội dung trả DUPLICATE_ID_CONFLICT. Giữ terminal result 2 s; pending không bị
  xóa theo retention. Normal ledger 8, STOP ledger 4. Queries không chiếm ledger.
- Không có NVS, ADC hoặc relay vật lý. Không mô phỏng toàn bộ threshold/debounce/
  rearm/calibration hoặc tự kết thúc vì dòng thấp.
- State/fault/inhibit events chưa mô phỏng coalescing/rate limit. Không gửi
  unsolicited EVENT: HMI đọc GET_EVENT_LOG.
- reboot giữ inhibit trong bộ nhớ tiến trình, xóa fault/ledger/log. Đóng chương trình
  và chạy lại bắt đầu mô hình mới. Không mô phỏng phục hồi NVS thật.
- SET_TIME chưa triển khai; service commands bị từ chối trên operational UART.
- Số đo và timing là fixture, không phải bằng chứng phần cứng.

## 7. Kiểm thử tự động

Từ firmware, không cần COM4:

~~~powershell
python -B -m unittest discover -s test/host -p test_controller_sim.py -v
~~~

Test gồm golden vector độc lập, CRC, COBS/fragmentation/expiry/recovery, schemas,
START/STOP, duplicate/conflict, fault khi START pending, STOP trước relay,
timeout scenarios, ledger, reboot, event pagination/overwrites và fake serial loop.
Core dùng thư viện chuẩn Python; test fake serial cần pyserial.

Đối chiếu thêm với codec độc lập của repo controller:

~~~powershell
$env:RCC_REFERENCE_CODEC_PATH = 'D:\Projects\Robot_Charge_Controller\Robot_Charge_Controller\tools\calibration\src\rcc_calibration_tool\protocol\codec.py'
python -B -m unittest discover -s test/host -p test_controller_sim.py -v
~~~

Không đặt biến này thì phép đối chiếu optional bị skip; các test khác vẫn chạy.
Simulator không phụ thuộc repo controller khi vận hành. Test Python qua không thay
thế quan sát màn hình/START/STOP trên COM4. Ghi lại phiên bản HMI và kịch bản khi
kiểm thử vật lý.

## 8. Kết quả kiểm tra ngày 2026-10-06

- Python PlatformIO, pyserial 3.5: 33 test PASS, gồm phép đối chiếu optional với
  codec độc lập của repo controller. Test model dùng clock giả để kiểm tra thời hạn.
- Chạy simulator scenario ready trên COM4, 115200 8N1, trong 8 s:
  RX 46 request / TX 46 reply / invalid_frames 0 / dropped_replies 0.
  Nhận đủ PING, GET_DEVICE_INFO, GET_STATUS, GET_MEASUREMENTS, GET_FAULTS,
  GET_EVENT_LOG. Không cần reset hoặc nạp lại HMI trong phép thử này.
- Phép thử vật lý chỉ quan sát traffic UART; không quan sát màn hình trực tiếp,
  không chạm START/STOP và không kết nối controller thật.
- Simulator đã thoát và đóng COM4 sau phép thử. Màn hình sẽ mất link khi không có
  peer trả lời; chạy lệnh ở mục 1 để tiếp tục kiểm thử tương tác.
