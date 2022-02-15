set(PROJECT_NAME "arkana")
message(STATUS "PROJECT_NAME: ${PROJECT_NAME}")
cmake_minimum_required(VERSION 3.14)

## Glob source files
file(GLOB_RECURSE SOURCE_FILES 
    LIST_DIRECTORIES false
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    "*.cpp"
    "*.h"
    "*.hpp"
)

add_library(${PROJECT_NAME} ${SOURCE_FILES})

## source_group by directory path
foreach (SOURCE_FILE IN ITEMS ${SOURCE_FILES})
    message(STATUS "${SOURCE_FILE}")
    get_filename_component(SOURCE_PATH "${SOURCE_FILE}" DIRECTORY )
    string(REPLACE "/" "\\" GROUP_PATH "${SOURCE_PATH}")
    source_group("${GROUP_PATH}" FILES "${SOURCE_FILE}")
endforeach ()

## MSVC CRT linckage setting
if (BUILD_WITH_STATIC_MSVC_CRT)
    set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
else()
    set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

## Compiler options
if ( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC"
  OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    set_source_files_properties(camellia/camellia-avx2.cpp       PROPERTIES COMPILE_FLAGS "/arch:AVX2")
    set_source_files_properties(camellia/camellia-avx2aesni.cpp  PROPERTIES COMPILE_FLAGS "/arch:AVX2")
    set_source_files_properties(crc32/crc32-avx2.cpp             PROPERTIES COMPILE_FLAGS "/arch:AVX2")
    set_source_files_properties(crc32/crc32-avx2clmul.cpp        PROPERTIES COMPILE_FLAGS "/arch:AVX2")
else ()
    set_source_files_properties(camellia/camellia-avx2.cpp       PROPERTIES COMPILE_FLAGS "-mavx2")
    set_source_files_properties(camellia/camellia-avx2aesni.cpp  PROPERTIES COMPILE_FLAGS "-mavx2 -maes")
    set_source_files_properties(crc32/crc32-avx2.cpp             PROPERTIES COMPILE_FLAGS "-mavx2")
    set_source_files_properties(crc32/crc32-avx2clmul.cpp        PROPERTIES COMPILE_FLAGS "-mavx2 -mpclmul")
endif ()

