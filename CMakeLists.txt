cmake_minimum_required(VERSION 3.10)
project(rs_in_cpp_solution)

set(BUILD_APPTEST OFF CACHE BOOL "Build the application to test compilation and runtime compliance")

add_subdirectory(lib)
if (BUILD_APPTEST)
    add_subdirectory(app_test)
    add_dependencies(rs_in_cpp_APPTEST rs_in_cpp)
endif()