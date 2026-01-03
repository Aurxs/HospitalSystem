cmake_minimum_required(VERSION 3.10)
project(HospitalSystem)

# 设置 C 语言标准
set(CMAKE_C_STANDARD 99)

# --- 1. 跨平台查找 Curses/ncurses 库 ---
if(WIN32)
    # 首先尝试查找 vcpkg 导出的 PDCurses 目标（不同 vcpkg 版本/端口可能使用不同的命名）
    find_package(PDCurses QUIET)
    if (TARGET PDCurses::PDCurses)
        set(CURSES_LIBRARIES PDCurses::PDCurses)
    elseif (TARGET pdcurses::pdcurses)
        set(CURSES_LIBRARIES pdcurses::pdcurses)
    else ()
        # 如果没有导出目标，尝试用 FindCurses 并提示 vcpkg 的安装位置（如果通过 action 安装 vcpkg，则在工作区有 vcpkg/）
        if (EXISTS "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
            set(VCPKG_HINT "${CMAKE_SOURCE_DIR}/vcpkg")
            find_package(Curses REQUIRED HINTS "${VCPKG_HINT}")
        else ()
            find_package(Curses REQUIRED)
        endif ()
        include_directories(${CURSES_INCLUDE_DIRS})
        if (NOT CURSES_LIBRARIES)
            set(CURSES_LIBRARIES ${CURSES_LIBRARIES}) # 保持变量存在（如果 find_package 已填充）
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
if (CURSES_LIBRARIES)
    target_link_libraries(HospitalSystem ${CURSES_LIBRARIES})
endif ()

# 生成测试程序: RunTest
add_executable(RunTest ${TEST_SOURCES})
if (CURSES_LIBRARIES)
    target_link_libraries(RunTest ${CURSES_LIBRARIES})
endif ()

# --- 5. Windows 特殊处理 (可选) ---
if(WIN32)
    # 解决部分 Windows 环境下控制台编码乱码问题
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()