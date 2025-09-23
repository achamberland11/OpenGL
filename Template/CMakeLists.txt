cmake_minimum_required(VERSION 3.12)

project(pgj1303)

add_subdirectory(./deps/glfw)

add_library(glad
    ./deps/glad/src/gl.c
)
target_include_directories(glad PRIVATE ./deps/glad/include)

add_executable(pgj1303 
    ./src/main.cpp
)

set_property(TARGET pgj1303 PROPERTY CXX_STANDARD 20)
target_include_directories(pgj1303 PRIVATE ./deps/glad/include)
target_include_directories(pgj1303 PRIVATE ./deps/glfw/include)
target_link_libraries(pgj1303 glad glfw)
