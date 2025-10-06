cmake_minimum_required(VERSION 3.12)

project(pgj1303)

add_subdirectory(./deps/glfw)
add_subdirectory(./deps/glm)
add_subdirectory(./deps/assimp)

add_library(glad ./deps/glad/src/gl.c)
target_include_directories(glad PUBLIC ./deps/glad/include)

add_executable(pgj1303
    ./src/main.cpp
    ./src/MeshScene.cpp
    ./src/MeshScene.h
    ./src/OpenGlDef.h
    ./src/OpenGlProgram.h
    ./src/OpenGlResource.h
    ./src/OpenGlShader.h
)

message(${CMAKE_CURRENT_SOURCE_DIR})
set_property(TARGET pgj1303 PROPERTY CXX_STANDARD 20)
set_property(TARGET pgj1303 PROPERTY DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(pgj1303 glfw glm glad assimp)
