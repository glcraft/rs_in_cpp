cmake_minimum_required(VERSION 3.10)

project(rs_in_cpp_APPTEST)

file(GLOB rs_in_cpp_APPTEST_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
)
add_executable(rs_in_cpp_APPTEST ${rs_in_cpp_APPTEST_FILES})

set_target_properties(rs_in_cpp PROPERTIES 
    CXX_STANDARD 17
    # CXX_STANDARD_REQUIRED ON
)