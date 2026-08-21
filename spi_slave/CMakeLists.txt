cmake_minimum_required(VERSION 3.16)

list(APPEND EXTRA_COMPONENT_DIRS "../shared")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(spi_slave)