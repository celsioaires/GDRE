#pragma once

#if defined (USE_CPP20_MODULES)
    import vulkan_hpp;
#else
#include <vulkan/vulkan_raii.hpp>
#endif

struct GLFWwindow;

namespace vku {
    class Context {
        vk::raii::Context mContext;
        vk::raii::Instance mInstance = nullptr;
        vk::raii::SurfaceKHR mSurface = nullptr;
        vk::raii::PhysicalDevice mPhysicalDevice = nullptr;
        vk::raii::Device mDevice = nullptr;
        vk::raii::Queue mGraphicsQueue = nullptr;
    public:
        bool initialize(GLFWwindow* window);
    };
}
