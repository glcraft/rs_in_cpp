cmake_minimum_required(VERSION 3.10)

project(rs_in_cpp LANGUAGES CXX VERSION 0.0.1)

file(GLOB rs_in_cpp_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/include/rust/Result.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/rust/Iterators.h"
)

add_library(rs_in_cpp INTERFACE ${rs_in_cpp_FILES})

target_include_directories(rs_in_cpp INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

set_target_properties(rs_in_cpp PROPERTIES 
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)