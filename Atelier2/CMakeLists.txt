cmake_minimum_required(VERSION 3.12)

project(pgj1303)

add_subdirectory(./deps/glfw)
add_subdirectory(./deps/glm)

add_library(glad ./deps/glad/src/gl.c)
target_include_directories(glad PUBLIC ./deps/glad/include)

add_executable(pgj1303
    ./src/main.cpp
    ./src/TriangleScene.cpp
    ./src/TriangleScene.h
    ./src/OpenGlDef.h
    ./src/OpenGlProgram.h
    ./src/OpenGlResource.h
    ./src/OpenGlShader.h
)

set_property(TARGET pgj1303 PROPERTY CXX_STANDARD 20)
set_property(TARGET pgj1303 PROPERTY DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(pgj1303 PRIVATE ./deps/glad/include)
target_include_directories(pgj1303 PRIVATE ./deps/glfw/include)
target_include_directories(pgj1303 PRIVATE ./src)
target_link_libraries(pgj1303 glfw glm glad)
