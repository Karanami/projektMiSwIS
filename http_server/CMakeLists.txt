cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_STANDDARD 23)
set(CMAKE_C_STANDDARD_REQUIRED ON)

set(EXECUTABLE_OUTPUT_PATH "bin")

project(http_tcp VERSION 1.0)

add_executable( http_tcp 
                main.cpp
                mongoose.c )

target_include_directories(http_tcp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
