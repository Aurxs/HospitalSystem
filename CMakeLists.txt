cmake_minimum_required(VERSION 3.12)
project(HospitalSystem C)

# Set C Standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# --- Dependencies ---
if(WIN32)
    # On Windows, prefer pdcurses via vcpkg
    # vcpkg usually provides a config file for pdcurses
    find_package(pdcurses CONFIG QUIET)

    if (NOT pdcurses_FOUND)
        find_package(PDCurses CONFIG QUIET)
    endif ()

    if (NOT pdcurses_FOUND AND NOT PDCurses_FOUND)
        find_package(unofficial-pdcurses CONFIG QUIET)
    endif ()

    if (pdcurses_FOUND)
        message(STATUS "Found pdcurses via CONFIG: ${pdcurses_DIR}")
        set(CURSES_LIBRARIES pdcurses::pdcurses)
    elseif (PDCurses_FOUND)
        message(STATUS "Found PDCurses via CONFIG: ${PDCurses_DIR}")
        set(CURSES_LIBRARIES PDCurses::PDCurses)
    elseif (unofficial-pdcurses_FOUND)
        message(STATUS "Found unofficial-pdcurses via CONFIG: ${unofficial-pdcurses_DIR}")
        set(CURSES_LIBRARIES unofficial::pdcurses::pdcurses)
    else ()
        # Fallback: Try standard FindCurses (might find other implementations)
        message(STATUS "pdcurses CONFIG not found, trying standard FindCurses...")
        find_package(Curses QUIET)
        if (CURSES_FOUND)
            include_directories(${CURSES_INCLUDE_DIRS})
        else ()
            message(FATAL_ERROR "Curses library not found. On Windows, please install pdcurses via vcpkg: vcpkg install pdcurses:x64-windows")
        endif ()
    endif()
else()
    # macOS and Linux
    find_package(Curses REQUIRED)
    include_directories(${CURSES_INCLUDE_DIRS})
endif()

# --- Include Directories ---
include_directories(include)

# --- Source Files ---
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

set(APP_SOURCES
        main.c
        ${CORE_SOURCES}
)

set(TEST_SOURCES
        tests/test.c
        tests/test_patient.c
        tests/test_doctor.c
        tests/test_drug.c
        tests/test_registration.c
        tests/test_bill.c
        tests/test_file_io.c
        tests/test_auth.c
        ${CORE_SOURCES}
)

# --- Targets ---

# Main Application
add_executable(HospitalSystem ${APP_SOURCES})
target_link_libraries(HospitalSystem PRIVATE ${CURSES_LIBRARIES})

# Test Application
add_executable(RunTest ${TEST_SOURCES})
target_link_libraries(RunTest PRIVATE ${CURSES_LIBRARIES})

# --- Platform Specifics ---
if(WIN32)
    # Fix for some Windows console encoding/warnings
    target_compile_definitions(HospitalSystem PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_definitions(RunTest PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()