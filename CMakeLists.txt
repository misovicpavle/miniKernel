cmake_minimum_required(VERSION 4.3)
project(compiler)

set(CMAKE_CXX_STANDARD 23)

add_executable(compiler kernel_main.cpp
        mmio_driver.cpp
        include/uart.h
        include/containers.h)
