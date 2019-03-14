cmake_minimum_required(VERSION 3.13)
project(Variant)

include_directories(${Variant_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 17)

add_executable(Variant main.cpp variant.h)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -pedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -D_GLIBCXX_DEBUG")
endif()