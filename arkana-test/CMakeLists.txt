set(PROJECT_NAME "arkana-test")
message(STATUS "PROJECT_NAME: ${PROJECT_NAME}")
cmake_minimum_required(VERSION 3.15)

if (POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

enable_testing()

## googletest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)
set(gtest_force_shared_crt OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE INTERNAL "" FORCE)
set(INSTALL_GTEST OFF CACHE INTERNAL "" FORCE)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

## Glob source files
file(
    GLOB_RECURSE SOURCE_FILES 
    LIST_DIRECTORIES false
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    "*.cpp"
    "*.h"
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE arkana gtest)
target_precompile_headers(${PROJECT_NAME} PRIVATE pch.h)
gtest_discover_tests(${PROJECT_NAME})

## source_group by directory path
foreach (SOURCE_FILE IN ITEMS ${SOURCE_FILES})
    message(STATUS "${SOURCE_FILE}")
    get_filename_component(SOURCE_PATH "${SOURCE_FILE}" DIRECTORY)
    string(REPLACE "/" "\\" GROUP_PATH "${SOURCE_PATH}")
    source_group("${GROUP_PATH}" FILES "${SOURCE_FILE}")
endforeach ()

## MSVC CRT linckage setting
if (BUILD_WITH_STATIC_MSVC_CRT)
    set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set_property(TARGET gtest PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

