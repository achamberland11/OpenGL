cmake_minimum_required(VERSION 3.12)

project(pgj1303)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(TARGET_PLATFORM_MACOS TRUE)
endif()

add_subdirectory(./deps/glfw)
add_subdirectory(./deps/glm)
add_subdirectory(./deps/assimp)

add_library(glad ./deps/glad/src/gl.c)
target_include_directories(glad PUBLIC ./deps/glad/include)

add_library(imgui
    ./deps/imgui/imgui.cpp
    ./deps/imgui/imgui_draw.cpp
    ./deps/imgui/imgui_tables.cpp
    ./deps/imgui/imgui_widgets.cpp
    ./deps/imgui/backends/imgui_impl_glfw.cpp
    ./deps/imgui/backends/imgui_impl_opengl3.cpp
)
target_link_libraries(imgui PUBLIC glfw)
target_include_directories(imgui PUBLIC ./deps/imgui/ ./deps/imgui/backends/)

add_executable(pgj1303
    ./src/main.cpp
    ./src/OpenGlDef.h
    ./src/OpenGlProgram.h
    ./src/OpenGlResource.h
    ./src/OpenGlShader.h
    ./src/OpenGlUtils.cpp
    ./src/OpenGlUtils.h
    ./src/InputProvider.h
    ./src/GlfwInputProvider.cpp
    ./src/GlfwInputProvider.h
    ./src/PostProcess.h
    ./src/SceneDrawer.cpp
    ./src/SceneDrawer.h
    ./src/SceneObject.h
    ./src/scenes/CubeScene.cpp
    ./src/scenes/CubeScene.h
    ./src/scenes/MeshScene.cpp
    ./src/scenes/MeshScene.h
    ./src/scenes/PlaneScene.cpp
    ./src/scenes/PlaneScene.h
    ./src/scenes/ShadowScene.cpp
    ./src/scenes/ShadowScene.h
    ./src/scenes/TriangleScene.cpp
    ./src/scenes/TriangleScene.h
    ./src/objects/BillboardObject.cpp
    ./src/objects/BillboardObject.h
    ./src/objects/MeshObject.cpp
    ./src/objects/MeshObject.h
    ./src/postprocesses/BlurPostProcess.cpp
    ./src/postprocesses/BlurPostProcess.h
    ./src/postprocesses/BlurComputePostProcess.cpp
    ./src/postprocesses/BlurComputePostProcess.h
    ./src/postprocesses/GrayScalePostProcess.cpp
    ./src/postprocesses/GrayScalePostProcess.h
        src/postprocesses/VignettePostProcess.cpp
        src/postprocesses/VignettePostProcess.h
        src/postprocesses/LensDistortionPostProcess.cpp
        src/postprocesses/LensDistortionPostProcess.h
)

set_property(TARGET pgj1303 PROPERTY CXX_STANDARD 20)
set_property(TARGET pgj1303 PROPERTY DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

set(LINK_LIBRARIES glfw glm glad assimp imgui)

if(TARGET_PLATFORM_MACOS)
    target_compile_definitions(pgj1303 PRIVATE GL_SILENCE_DEPRECATION)
endif()
if(TARGET_PLATFORM_MACOS)
    list(APPEND LINK_LIBRARIES "-framework OpenGL")
endif()

target_link_libraries(pgj1303 PRIVATE ${LINK_LIBRARIES})
target_include_directories(pgj1303 PRIVATE ./deps/stb ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(COPY ${CMAKE_SOURCE_DIR}/models DESTINATION ${CMAKE_SOURCE_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/shaders DESTINATION ${CMAKE_SOURCE_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/textures DESTINATION ${CMAKE_SOURCE_DIR})
