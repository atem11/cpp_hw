cmake_minimum_required(VERSION 3.13)
project(Bind)

set(CMAKE_CXX_STANDARD 14)

add_executable(test test.cpp
        gtest/gtest_main.cc
        gtest/gtest-all.cc
        gtest/gtest.h
        bind.h)

add_executable(Bind main.cpp bind.h)

target_link_libraries(test -lpthread)