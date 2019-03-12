cmake_minimum_required(VERSION 3.13)
project(bind)

include_directories(${bind_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 17)

add_executable(test_me test.cpp
        gtest/gtest_main.cc
        gtest/gtest-all.cc
        gtest/gtest.h
        bind.h
        )

add_executable(bind main.cpp bind.h)
target_link_libraries(test_me -lpthread)