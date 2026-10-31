cmake_minimum_required(VERSION 4.3)

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "451f2fe2-a8a2-47c3-bc32-94786d8fc91b")

project(GDRE)

set(CMAKE_CXX_STANDARD 20)

# Find vulkan
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

    # Defines
    target_compile_definitions(VulkanCppModule PUBLIC
        VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
        VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1)

    # Sources
    target_sources(VulkanCppModule PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        BASE_DIRS "${Vulkan_INCLUDE_DIR}"
        FILES "${Vulkan_INCLUDE_DIR}/vulkan/vulkan.cppm")

    target_include_directories(VulkanCppModule PRIVATE "${Vulkan_INCLUDE_DIR}")

    target_link_libraries(VulkanCppModule PUBLIC Vulkan::Vulkan)
endif()

# spdlog
add_subdirectory(libs/spdlog)

# GLFW
set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "GLFW_BUILD_WAYLAND OFF" FORCE)
set(GLFW_BUILD_X11 ON CACHE BOOL "GLFW_BUILD_X11" FORCE)

add_subdirectory(libs/glfw)

# GLM
add_subdirectory(libs/glm)

# fastgltf
add_subdirectory(libs/fastgltf)

# GDRE executable
add_executable(GDRE
    # src
    src/application.cpp
    src/main.cpp
    # engine
    src/engine/engine.cpp
    # engine core
    src/engine/core/platform.cpp
    src/engine/core/time.cpp
    # engine gui
    src/engine/gui/gui.cpp
    src/engine/gui/window.cpp
    # engine load
    src/engine/load/loader.cpp
    # engine render
    src/engine/render/backend/backend.cpp
    src/engine/render/backend/buffer.cpp
    src/engine/render/backend/command.cpp
    src/engine/render/backend/context.cpp
    src/engine/render/backend/pipeline.cpp
    src/engine/render/backend/swapchain.cpp
    src/engine/render/renderer.cpp)

# Defines
target_compile_definitions(GDRE PRIVATE
    VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
    GDRE_SHADERS_DIRECTORY="${CMAKE_SOURCE_DIR}/shaders/"
    GDRE_ASSETS_DIRECTORY="${CMAKE_SOURCE_DIR}/assets/")

if (ENABLE_CPP20_MODULE)
    target_compile_definitions(GDRE PRIVATE USE_CPP20_MODULES)
endif()

# ImGui sources
target_sources(GDRE PRIVATE
    libs/imgui/imgui.cpp
    libs/imgui/imgui_draw.cpp
    libs/imgui/imgui_tables.cpp
    libs/imgui/imgui_widgets.cpp)

target_include_directories(GDRE PRIVATE
    libs/imgui)

# GDRE executable links
target_link_libraries(GDRE PRIVATE
    spdlog::spdlog
    glfw
    glm::glm
    fastgltf::fastgltf)

# GDRE Cpp module
if (ENABLE_CPP20_MODULE)
    target_link_libraries(GDRE PRIVATE Vulkan::cppm)
else()
    target_link_libraries(GDRE PRIVATE Vulkan::Vulkan)
endif()

# Slang shader compilation
function(add_slang_shader_target TARGET)
    set(SHADERS_DIR "${CMAKE_SOURCE_DIR}/shaders")
    set(SHADER_OUTPUT "${SHADERS_DIR}/compiled/default.spv")
    set(SHADER_SOURCE "${SHADERS_DIR}/default.slang")
    set(SHADER_SCRIPT "${SHADERS_DIR}/compile.sh")

    add_custom_command(
        OUTPUT "${SHADER_OUTPUT}"
        COMMAND bash "${SHADER_SCRIPT}"
        WORKING_DIRECTORY "${SHADERS_DIR}"
        DEPENDS "${SHADER_SCRIPT}" "${SHADER_SOURCE}"
        COMMENT "Compiling Slang shaders"
        VERBATIM)

    add_custom_target(${TARGET} DEPENDS "${SHADER_OUTPUT}")
endfunction()

add_slang_shader_target(compile_shaders)
add_dependencies(GDRE compile_shaders)
