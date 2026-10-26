cmake_minimum_required(VERSION 4.3)

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "451f2fe2-a8a2-47c3-bc32-94786d8fc91b")

project(GDRE)

set(CMAKE_CXX_STANDARD 20)

# Vulkan
set(Vulkan_INCLUDE_DIR "$ENV{VULKAN_SDK}/include")
set(Vulkan_LIBRARY "$ENV{VULKAN_SDK}/lib/VulkanLoader/lib/libvulkan.so")

find_package(Vulkan REQUIRED)

# Vulkan Cpp module
option(ENABLE_CPP20_MODULE "Enable Vulkan Cpp modules" OFF)

if (ENABLE_CPP20_MODULE)
    add_library(VulkanCppModule)
    add_library(Vulkan::cppm ALIAS VulkanCppModule)

    target_compile_features(VulkanCppModule PUBLIC cxx_std_20)

    set_target_properties(VulkanCppModule PROPERTIES CXX_MODULE_STD ON)

    target_compile_definitions(VulkanCppModule PUBLIC
        VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
        VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1)

    target_sources(VulkanCppModule PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        BASE_DIRS "${Vulkan_INCLUDE_DIR}"
        FILES "${Vulkan_INCLUDE_DIR}/vulkan/vulkan.cppm")

    target_include_directories(VulkanCppModule PRIVATE "${Vulkan_INCLUDE_DIR}")

    target_link_libraries(VulkanCppModule PUBLIC Vulkan::Vulkan)
endif()

# Libraries
add_subdirectory(libs/spdlog)
add_subdirectory(libs/glfw)
add_subdirectory(libs/glm)
add_subdirectory(libs/fastgltf)

# GDRE
add_executable(GDRE
    src/application.cpp
    src/engine.cpp
    src/main.cpp
    src/core/platform.cpp
    src/core/time.cpp
    src/gui/gui.cpp
    src/gui/window.cpp
    src/load/loader.cpp
    src/render/vku/vku.cpp
    src/render/renderer.cpp)

if (ENABLE_CPP20_MODULE)
    target_compile_definitions(GDRE PRIVATE USE_CPP20_MODULES)
endif()

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

if (ENABLE_CPP20_MODULE)
    target_link_libraries(GDRE PRIVATE Vulkan::cppm)
else()
    target_link_libraries(GDRE PRIVATE Vulkan::Vulkan)
endif()
