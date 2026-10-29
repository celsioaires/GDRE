#pragma once

#include "context.h"

namespace backend {
    class Swapchain {
        vk::raii::SwapchainKHR mHandle = nullptr;
        vk::Extent2D mExtent;
        std::vector<vk::Image> mImages;
    public:
        bool initialize(const Context& context, uint32_t width, uint32_t height);
    };
}
