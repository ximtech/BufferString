cmake_minimum_required(VERSION 3.20)
project(BufferString VERSION 1.0 LANGUAGES C)

include(cmake/CPM.cmake)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES
        BufferString.c
        include/BufferString.h)

option(ENABLE_FLOAT_FORMATTING "Set to ON to enable floating point formatting" ${ENABLE_FLOAT_FORMATTING})

if (ENABLE_FLOAT_FORMATTING)
    add_compile_definitions(ENABLE_FLOAT_FORMATTING)
endif()

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

target_include_directories(${PROJECT_NAME} PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}.h
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Export
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${PROJECT_NAME})

install(EXPORT ${PROJECT_NAME}Export
        FILE ${PROJECT_NAME}-config.cmake
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${PROJECT_NAME})