cmake_minimum_required(VERSION 3.22.1)
project(mattdaemon)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_library(compile_flags INTERFACE)
target_compile_options(compile_flags INTERFACE
        -Wall
        -Werror
        -Wextra
)

include_directories(include)

add_subdirectory(src)

