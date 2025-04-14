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

### 1. Trên Linux

1. **Cài đặt các phụ thuộc**:

    - Tải và cài đặt [Tên phần mềm] từ [link tải].
    - Mở **Command Prompt** (hoặc **PowerShell**) và chạy lệnh cài đặt nếu có:
      ```bash
        sudo apt update # update hệ thống
      ```
      ```bash
        sudo apt install make # cài đặt make để chạy trên Ubuntu
      ```
      ```bash
        make --version # kiểm tra phiên bản
        which make # tìm nơi cài đặt
      ```


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

### 2. Trên Window

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
        bin/output # cuối cùng chỉ việc chạy lệnh này
      ```


