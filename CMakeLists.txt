cmake_minimum_required(VERSION 3.5)
project(optimal_parsing)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra")

include_directories(include)
include_directories(include/optimal-parsing)

add_executable(optimal_parsing
        app/app.cpp
        include/optimal-parsing/compressor.hpp
        include/optimal-parsing/dictionary.hpp
        include/optimal-parsing/dictionary_opt.hpp
        include/optimal-parsing/lzw.hpp
        include/optimal-parsing/parser.hpp)
