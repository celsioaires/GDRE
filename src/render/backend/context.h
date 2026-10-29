#pragma once

#if defined (USE_CPP20_MODULES)
    import vulkan_hpp;
#else
#include <vulkan/vulkan_raii.hpp>
#endif

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace backend {
    class Context {
        vk::raii::Context mHandle;
        vk::raii::Instance mInstance = nullptr;
#if !defined (NDEBUG)
        vk::raii::DebugUtilsMessengerEXT mDebugMessenger = nullptr;
#endif
        vk::raii::SurfaceKHR mSurface = nullptr;
        vk::raii::PhysicalDevice mPhysicalDevice = nullptr;
        uint32_t mGraphicsFamily = 0;
        vk::raii::Device mDevice = nullptr;
        vk::raii::Queue mGraphicsQueue = nullptr;
    public:
        bool initialize(GLFWwindow* window);

        // Getters
        const vk::raii::SurfaceKHR& getSurface() const { return mSurface; }
        const vk::raii::PhysicalDevice& getPhysicalDevice() const { return mPhysicalDevice; }
        uint32_t getGraphicsFamily() const { return mGraphicsFamily; }
        const vk::raii::Device& getDevice() const { return mDevice; }
    };
}
