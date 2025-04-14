# Parallel
# Data-Mining

## Giới thiệu

Đây là một chương trình sử dụng parallel và multithread trên thuật toán 
khai thác mẫu phổ biến N-List.

## Yêu cầu hệ thống

- **Windows**: Windows 10 trở lên
- **Linux**: Ubuntu 20.04 hoặc các bản phân phối tương tự
- **Ngôn ngữ lập trình**: [C/C++]
- **Phụ thuộc**:
  - [Trên Ubuntu yêu cầu cài đặt make và valgrind]

## Cài đặt

### 1. Trên Linux/Windows với Makefile

1. **Cài đặt các phụ thuộc**:

   Để sử dụng `make` trên Windows, bạn cần cài đặt một môi trường hỗ trợ như **MinGW**, **MSYS2**, hoặc **WSL**.

   ### Cách 1: Dùng MSYS2 (Khuyên dùng)

   - Tải và cài đặt MSYS2 tại: [https://www.msys2.org/](https://www.msys2.org/)
   - Sau khi cài đặt, mở **MSYS2 MSYS** terminal và chạy:
     ```bash
     pacman -Syu       # cập nhật hệ thống
     pacman -S make gcc  # cài đặt make và trình biên dịch g++
     ```
   - Kiểm tra cài đặt:
     ```bash
     make --version     # kiểm tra phiên bản
     which make         # tìm đường dẫn make
     ```

   ### Cách 2: Dùng MinGW

   - Tải MinGW tại: [https://sourceforge.net/projects/mingw/](https://sourceforge.net/projects/mingw/)
   - Trong quá trình cài đặt, chọn các gói sau:
     - `mingw32-gcc-g++`
     - `msys-base`
     - `mingw32-make`
   - Sau khi cài xong, thêm đường dẫn MinGW (`bin/`) vào `PATH` trong Environment Variables.
   - Đổi lệnh `make` thành `mingw32-make` nếu `make` không hoạt động:
     ```bash
     mingw32-make --version
     ```

   ### Cách 3: Dùng WSL (Windows Subsystem for Linux)

   - Mở **Microsoft Store**, tìm và cài **Ubuntu**.
   - Mở terminal Ubuntu và chạy:
     ```bash
     sudo apt update
     sudo apt install make g++
     ```
   - Sau đó bạn có thể dùng lệnh `make` như trên Linux.

    # Cập nhật hệ thống

2. **Chạy chương trình**:

    - Điều hướng đến thư mục chứa chương trình:
      ```bash
       cd Multi\Parallel\N_List\src
      ```
    - Chạy chương trình:
      ```bash
      make # để chạy các file trong chương trình
      make run # để chạy chương trình
      make check # kiểm tra các khối trong chương trình trong qua valgrind
      ```

### 2. Trên Window không cài Makefile

1. **Chạy chương trình**:

    - Chạy chương trình:
      ```bash
        g++ -std=c++20 -Wall -Iinclude  -c src/NodePPC.cpp -o bin/NodePPC.o
        g++ -std=c++20 -Wall -Iinclude  -c src/NodeN_List.cpp -o bin/NodeN_List.o
        g++ -std=c++20 -Wall -Iinclude  -c src/PPCTree.cpp -o bin/PPCTree.o
        g++ -std=c++20 -Wall -Iinclude  -c src/PrePosCount.cpp -o bin/PrePosCount.o
        g++ -std=c++20 -Wall -Iinclude  -c src/ThreadPool.cpp -o bin/ThreadPool.o
        g++ -std=c++20 -Wall -Iinclude  -c src/main.cpp -o bin/main.o
        g++ -std=c++20 -Wall -Iinclude  -o bin/output bin/NodeN_List.o bin/NodePPC.o bin/PPCTree.o bin/PrePosCount.o bin/ThreadPool.o bin/main.o
        # chạy hết các lện bên trên theo thứ tự cuối
        bin/output.exe # cuối cùng chỉ việc chạy lệnh này
      ```


