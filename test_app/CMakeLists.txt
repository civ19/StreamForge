cmake_minimum_required(VERSION 3.16)

# 1. Map directly to your physical hard drive folders
set(EXTRA_COMPONENT_DIRS 
    "${CMAKE_CURRENT_SOURCE_DIR}/../spi_master/main/spi_master_pipeline"
    "${CMAKE_CURRENT_SOURCE_DIR}/../spi_slave/main/spi_slave_pipeline"
    "${CMAKE_CURRENT_SOURCE_DIR}/../spi_slave/main/buffering"
    "${CMAKE_CURRENT_SOURCE_DIR}/../shared/utils"
)

# 2. Pull in core project definitions natively
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(streamforge_tests)
