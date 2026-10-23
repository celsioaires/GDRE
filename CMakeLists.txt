cmake_minimum_required(VERSION 4.3)
project(GDRE)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(vendor/spdlog)
add_subdirectory(vendor/glfw)

add_executable(GDRE
        src/main.cpp)

target_link_libraries(GDRE PRIVATE
        spdlog::spdlog
        glfw)
