cmake_minimum_required(VERSION 3.10)

project(rs_in_cpp LANGUAGES CXX VERSION 0.0.1)

file(GLOB rs_in_cpp_FILES
)

add_library(rs_in_cpp INTERFACE ${rs_in_cpp_FILES})

set_target_properties(rs_in_cpp PROPERTIES 
    CXX_STANDARD 17
    # CXX_STANDARD_REQUIRED ON
)