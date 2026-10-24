cmake_minimum_required(VERSION 4.3)
project(GDRE)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(libs/spdlog)
add_subdirectory(libs/glfw)
add_subdirectory(libs/glm)
add_subdirectory(libs/fastgltf)

add_executable(GDRE
    src/main.cpp
    src/engine.cpp
    src/core/platform.cpp
    src/core/time.cpp
    src/gui/window.cpp
    src/load/loader.cpp
    src/render/renderer.cpp
    src/gui/gui.cpp)

target_sources(GDRE PRIVATE
    libs/imgui/imgui.cpp
    libs/imgui/imgui_draw.cpp
    libs/imgui/imgui_tables.cpp
    libs/imgui/imgui_widgets.cpp)

target_include_directories(GDRE PRIVATE
    libs/imgui)

target_link_libraries(GDRE PRIVATE
    spdlog::spdlog
    glfw
    glm::glm
    fastgltf::fastgltf)
