cmake_minimum_required(VERSION 3.16)
project(Restreamer C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")

add_executable(restreamer main.c)

target_link_libraries(restreamer PUBLIC pthread)

