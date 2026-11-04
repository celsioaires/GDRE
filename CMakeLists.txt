cmake_minimum_required(VERSION 4.3)

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "451f2fe2-a8a2-47c3-bc32-94786d8fc91b")

project(GDRE)

set(CMAKE_CXX_STANDARD 20)

# Vulkan
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
add_subdirectory(external/spdlog)

# GLFW
set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)

add_subdirectory(external/glfw)

# GLM
add_subdirectory(external/glm)

# fastgltf
add_subdirectory(external/fastgltf)

# GDRE sources
add_executable(GDRE
    # src
    source/application.cpp
    source/main.cpp
    # engine
    source/engine/engine.cpp
    # engine core
    source/engine/core/platform.cpp
    source/engine/core/time.cpp
    # engine gui
    source/engine/gui/gui.cpp
    source/engine/gui/hierarchy.cpp
    source/engine/gui/inspector.cpp
    source/engine/gui/window.cpp
    # engine load
    source/engine/load/loader.cpp
    # engine render
    source/engine/render/backend/backend.cpp
    source/engine/render/backend/buffer.cpp
    source/engine/render/backend/command.cpp
    source/engine/render/backend/context.cpp
    source/engine/render/backend/descriptor.cpp
    source/engine/render/backend/pipeline.cpp
    source/engine/render/backend/swapchain.cpp
    source/engine/render/camera.cpp
    source/engine/render/mesh.cpp
    source/engine/render/renderer.cpp)

# Defines
target_compile_definitions(GDRE PRIVATE
    GLFW_INCLUDE_NONE
    VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
    VULKAN_HPP_NO_SPACESHIP_OPERATOR
    VULKAN_HPP_NO_TO_STRING
    VULKAN_HPP_NO_SETTERS
    GDRE_SHADERS_DIRECTORY="${CMAKE_SOURCE_DIR}/shaders/"
    GDRE_ASSETS_DIRECTORY="${CMAKE_SOURCE_DIR}/assets/")

# PCHs
target_precompile_headers(GDRE PRIVATE
)

# Enable cpp module
if (ENABLE_CPP20_MODULE)
    target_compile_definitions(GDRE PRIVATE USE_CPP20_MODULES)
endif()

# GDRE ImGui sources
target_sources(GDRE PRIVATE
    external/imgui/backends/imgui_impl_glfw.cpp
    external/imgui/backends/imgui_impl_vulkan.cpp
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp)

target_include_directories(GDRE PRIVATE external/imgui)

# GDRE library links
target_link_libraries(GDRE PRIVATE
    spdlog::spdlog
    glfw
    glm::glm
    fastgltf::fastgltf)

# GDRE cpp module
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
