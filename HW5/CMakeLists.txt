cmake_minimum_required(VERSION 3.12)
project(HW5)

include_directories(${persistent_set_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 14)

add_executable(test unit_test_persistent_set.cpp
        gtest/gtest_main.cc
        gtest/gtest-all.cc
        gtest/gtest.h
        per_set.h
        custom_shared_ptr.h
        linked_ptr.h)

add_executable(HW5 custom_shared_ptr.h main.cpp per_set.h linked_ptr.h)

target_link_libraries(test -lpthread)