cmake_minimum_required(VERSION 4.3)
project(GDRE)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(libs/spdlog)
add_subdirectory(libs/glfw)
add_subdirectory(libs/glm)
add_subdirectory(libs/fastgltf)

add_executable(GDRE
        src/main.cpp)

target_link_libraries(GDRE PRIVATE
        spdlog::spdlog
        glfw
        glm::glm
        fastgltf::fastgltf)
