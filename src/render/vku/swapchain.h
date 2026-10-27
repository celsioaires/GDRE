#pragma once

#include "context.h"

namespace vku {
    class Swapchain {
        vk::raii::SwapchainKHR mSwapchain = nullptr;
        std::vector<vk::Image> mSwapchainImages;
    public:
        bool initialize(const Context& context);
    };
}
