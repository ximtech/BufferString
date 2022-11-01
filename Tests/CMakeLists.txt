cmake_minimum_required(VERSION 3.20)
project(Tests C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(ROOT_DIR "..")
include_directories(${ROOT_DIR}/)

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

# Create the gcov target. Run coverage tests with 'make gcov'
#add_custom_target(gcov
#        COMMAND mkdir -p coverage
#        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
#        )
#
#add_custom_command(TARGET gcov
#        COMMAND echo "=================== GCOV ===================="
#        COMMAND gcov -b ${CMAKE_SOURCE_DIR}/*.c -o ${OBJECT_DIR}
#        | grep -A 5 "BufferString.c" > CoverageSummary.tmp
#        COMMAND cat CoverageSummary.tmp
#        COMMAND echo "-- Coverage files have been output to ${CMAKE_BINARY_DIR}/coverage"
#        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/coverage  # Need separate command for this line
#        )