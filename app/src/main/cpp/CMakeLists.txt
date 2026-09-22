cmake_minimum_required(VERSION 3.22.1)
project(imagetoeps)

add_library(imagetoeps SHARED native-lib.cpp)

find_library(log-lib log)
find_library(gles-lib GLESv3)

target_link_libraries(imagetoeps ${log-lib} ${gles-lib})
