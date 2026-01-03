cmake_minimum_required(VERSION 3.10)
project(HospitalSystem)

# 设置 C 语言标准
set(CMAKE_C_STANDARD 99)

# --- 1. 跨平台查找 Curses/ncurses 库 ---
if(WIN32)
    # 在 Windows 上，建议通过 vcpkg 安装 pdcurses: vcpkg install pdcurses

    # 1. 优先尝试通过 CONFIG 模式查找 pdcurses (vcpkg 默认方式)
    find_package(pdcurses CONFIG QUIET)

    if (pdcurses_FOUND)
        set(CURSES_LIBRARIES pdcurses::pdcurses)
    else ()
        # 2. 尝试查找 PDCurses (可能是其他方式安装)
        find_package(PDCurses QUIET)

        if (PDCurses_FOUND)
            set(CURSES_LIBRARIES PDCurses::PDCurses)
        else ()
            # 3. 最后尝试标准的 Curses 查找
            find_package(Curses QUIET)

            if (CURSES_FOUND)
                include_directories(${CURSES_INCLUDE_DIRS})
            else ()
                message(FATAL_ERROR "Could not find Curses library. On Windows, please install pdcurses via vcpkg:\n  vcpkg install pdcurses\nOr ensure Curses is installed and configured.")
            endif ()
        endif ()
    endif()
else()
    # macOS 和 Linux 系统
    find_package(Curses REQUIRED)
    include_directories(${CURSES_INCLUDE_DIRS})
endif()

# --- 2. 包含项目头文件目录 ---
include_directories(include)

# --- 3. 定义源文件列表 ---
# 核心逻辑文件（不含 main）
set(CORE_SOURCES
        src/patient.c
        src/doctor.c
        src/drug.c
        src/file_io.c
        src/auth.c
        src/ui.c
        src/registration.c
        src/bill.c
)

# 主程序源文件
set(APP_SOURCES
        main.c
        ${CORE_SOURCES}
)

# 测试程序源文件
set(TEST_SOURCES
        ${CORE_SOURCES}
        tests/test_patient.c
        tests/test_doctor.c
        tests/test.c
        tests/test_drug.c
        tests/test_registration.c
        tests/test_bill.c
        tests/test_file_io.c
        tests/test_auth.c
)

# --- 4. 创建目标并链接库 ---

# 生成主程序: HospitalSystem
add_executable(HospitalSystem ${APP_SOURCES})
target_link_libraries(HospitalSystem ${CURSES_LIBRARIES})

# 生成测试程序: RunTest
add_executable(RunTest ${TEST_SOURCES})
target_link_libraries(RunTest ${CURSES_LIBRARIES})

# --- 5. Windows 特殊处理 (可选) ---
if(WIN32)
    # 解决部分 Windows 环境下控制台编码乱码问题
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()