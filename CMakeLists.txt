cmake_minimum_required(VERSION 3.10)
project(HospitalSystem)

set(CMAKE_C_STANDARD 99)

include_directories(include)

set(SOURCE_FILES
        main.c
        src/patient.c
        src/doctor.c
        src/drug.c
        src/file_io.c
        src/auth.c
        src/ui.c
        src/registration.c
        src/bill.c
)

add_executable(HospitalSystem ${SOURCE_FILES})

add_executable(RunTest
        src/patient.c
        src/doctor.c
        src/drug.c
        src/file_io.c
        src/auth.c
        src/ui.c
        src/registration.c
        src/bill.c
        tests/test_patient.c
        tests/test_doctor.c
        tests/test.c
        tests/test_drug.c
        tests/test_registration.c
        tests/test_bill.c
        tests/test_file_io.c
)