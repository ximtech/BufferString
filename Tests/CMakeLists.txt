cmake_minimum_required(VERSION 3.20)
project(Tests C)

include(../cmake/CodeCoverage.cmake)

set(CMAKE_C_STANDARD 99)
set(CMAKE_VERBOSE_MAKEFILE ON)

append_coverage_compiler_flags()

set(ROOT_DIR "..")
include_directories(${ROOT_DIR}/)

set(COVERAGE_EXCLUDES
        '${ROOT_DIR}/Tests/*'
        '${ROOT_DIR}/Tests/BufferString/*'
        '${ROOT_DIR}/Tests/munit/*')

add_compile_definitions(ENABLE_FLOAT_FORMATTING)

get_filename_component(BUILD_DIRECTORY_NAME "${CMAKE_CURRENT_BINARY_DIR}" NAME)
add_subdirectory(${ROOT_DIR} ${BUILD_DIRECTORY_NAME})

add_executable(
        Tests main.c
        munit/munit.h
        munit/munit.c
        munit/example.c)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} BufferString)