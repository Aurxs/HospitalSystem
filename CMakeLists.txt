cmake_minimum_required(VERSION 3.10)
project(HospitalSystem)

set(CMAKE_C_STANDARD 99)

# 查找 PkgConfig 包
find_package(PkgConfig REQUIRED)

# 使用 pkg-config 查找 gtk4
pkg_check_modules(GTK4 REQUIRED gtk4)

include_directories(include ${GTK4_INCLUDE_DIRS})
link_directories(${GTK4_LIBRARY_DIRS})
add_compile_options(${GTK4_CFLAGS_OTHER})

set(SOURCE_FILES
        main_gui.c
        src/patient.c
        src/doctor.c
        src/drug.c
        src/file_io.c
        src/auth.c
        src/registration.c
        src/bill.c
)

add_executable(HospitalSystem ${SOURCE_FILES})
# 将 GTK4 库链接到主可执行文件
target_link_libraries(HospitalSystem ${GTK4_LIBRARIES})

add_executable(RunTest
        src/patient.c
        src/doctor.c
        src/drug.c
        src/file_io.c
        src/auth.c
        src/registration.c
        src/bill.c
        tests/test_patient.c
        tests/test_doctor.c
        tests/test.c
        tests/test_drug.c
        tests/test_registration.c
        tests/test_bill.c
        tests/test_file_io.c
        tests/test_auth.c
)
# 如果测试代码也依赖 GTK4，也需要链接
target_link_libraries(RunTest ${GTK4_LIBRARIES})