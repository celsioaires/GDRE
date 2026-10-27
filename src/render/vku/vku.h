#pragma once

#include "context.h"
#include "swapchain.h"

namespace vku {
    class VKU {
        Context mContext;
        Swapchain mSwapchain;
    public:
        bool initialize(GLFWwindow* window);
    };
}
