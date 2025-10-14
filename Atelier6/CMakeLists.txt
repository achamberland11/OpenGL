cmake_minimum_required(VERSION 3.12)

project(pgj1303)

add_subdirectory(./deps/glfw)
add_subdirectory(./deps/glm)
add_subdirectory(./deps/assimp)

add_library(glad ./deps/glad/src/gl.c)
target_include_directories(glad PUBLIC ./deps/glad/include ./deps/stb)

add_executable(pgj1303
    ./src/main.cpp
    ./src/BillboardObject.cpp
    ./src/BillboardObject.h
    ./src/MeshScene.cpp
    ./src/MeshScene.h
    ./src/OpenGlDef.h
    ./src/OpenGlProgram.h
    ./src/OpenGlResource.h
    ./src/OpenGlShader.h
    ./src/OpenGlUtils.cpp
    ./src/OpenGlUtils.h
    ./src/InputProvider.h
    ./src/GlfwInputProvider.cpp
    ./src/GlfwInputProvider.h
    ./src/SceneDrawer.cpp
    ./src/SceneDrawer.h
    ./src/SceneObject.h
    ./src/EnvironmentObject.cpp
    ./src/EnvironmentObject.h
)

set_property(TARGET pgj1303 PROPERTY CXX_STANDARD 20)
set_property(TARGET pgj1303 PROPERTY DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(pgj1303 glfw glm glad assimp)
