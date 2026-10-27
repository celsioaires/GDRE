#pragma once

#if defined (USE_CPP20_MODULES)
    import vulkan_hpp;
#else
    #include <vulkan/vulkan_raii.hpp>
#endif

namespace vku {
    class VKU {
        vk::raii::Context mContext;
        vk::raii::Instance mInstance = nullptr;
        vk::raii::PhysicalDevice mPhysicalDevice = nullptr;
        vk::raii::Device mDevice = nullptr;
    public:
        bool initialize();
    };
}
