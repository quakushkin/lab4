cmake_minimum_required(VERSION 3.23)
project(labwork_4_mishapolyakov111)
set(CMAKE_EXE_LINKER_FLAGS "-static")

set(CMAKE_CXX_STANDARD 20)

add_executable(labwork_4_mishapolyakov111
        bin/main.cpp
        lib/hamarc.cpp
        lib/parsing.cpp
        lib/hammimgcode.cpp)

include_directories(lib)
include_directories(bin)
