# Bài 7 - Giao tiếp SPI

## Nội dung bài tập

_Bài tập yêu cầu:_

- Cấu hình STM32 là Master SPI

- Kết nối với một module SPI đơn giản

- Gửi một byte dữ liệu và nhận phản hồi, hiển thị dữ liệu nhận được lên terminal


***File code kết quả: [Link](https://github.com/phamsan1503-ai/BAI-7)***


### I. Kết nối với module SPI Max7219

1. STM32 reset → chạy main()

- Cấu hình SPI1 và GPIO (PA4, PA5, PA7).

- Khởi tạo MAX7219 (giải mã BCD, bật đủ 8 digit, cường độ sáng trung bình, thoát shutdown).

- Xoá toàn bộ LED (blank tất cả digit).

2. Vòng lặp while(1) chạy mãi

- Hàm Show_Effect() được gọi liên tục.

3. Trong Show_Effect()

- Pha 1: lần lượt từ digit1 → digit8 hiển thị số 8. Kết quả: LED 7-seg sáng dần từng số từ trái sang phải.

- Pha 2: lần lượt từ digit8 → digit1 tắt (blank). Kết quả: LED tắt dần từ phải sang trái.

- Lặp lại mãi → hiệu ứng “sáng dần rồi tắt dần”.

### II. Gửi một byte dữ liệu và nhận phản hồi, hiển thị dữ liệu nhận được lên terminal

1. Sau khi nạp code vào STM32F103:

- SPI1 được cấu hình ở chế độ Master, sử dụng các chân:

- PA5 → SCK (xung clock SPI)

- PA7 → MOSI (dữ liệu từ STM32 gửi đến MAX7219)

- PA4 → CS (chân chọn chip, điều khiển khi gửi dữ liệu)

2. MAX7219 được khởi tạo với:

- Decode mode bật cho tất cả 8 digit (mỗi digit hiển thị số thập phân dễ dàng).

- Độ sáng (Intensity) đặt mức trung bình.

- Scan limit = 8 (tất cả 8 LED digit hoạt động).

- Shutdown = Normal (LED được bật).

- Test mode tắt.

3. Sau đó hàm Test_LED_Scan() chạy vòng lặp vô hạn:

- Lần lượt bật từng digit (1 → 8).

- Ở mỗi digit, LED hiển thị số đúng bằng vị trí digit (digit1 = số 1, digit2 = số 2, …).

- Sau 300 ms thì digit đó tắt đi.

- Hiệu ứng nhìn trên LED sẽ giống như số chạy qua từng ô, sáng tắt dần từ trái qua phải.

=> Kết quả: LED hiển thị chạy liên tục (không cần reset STM32).

***Video  kết quả: [Link](https://drive.google.com/drive/folders/1k93tgsqX8-rIK2TkhtnaA3ixFbzGyhiK?usp=drive_link)***
