#pragma once

#include "context.h"
#include "swapchain.h"
#include "pipeline.h"

namespace backend {
    struct WSI {
        GLFWwindow* window = nullptr;
        uint32_t width = 0;
        uint32_t height = 0;
    };

    class Backend {
        Context mContext;
        Swapchain mSwapchain;
        Pipeline mGraphicsPipeline;
    public:
        bool initialize(const WSI& wsi);
    };
}
