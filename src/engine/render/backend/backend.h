#pragma once

#include "context.h"
#include "swapchain.h"
#include "pipeline.h"
#include "command.h"

namespace backend {
    struct Info {
        GLFWwindow* window = nullptr;
        uint32_t width = 0;
        uint32_t height = 0;
    };

    class Backend {
        Context mContext;
        Swapchain mSwapchain;
        Pipeline mDefaultPipeline;
        Command mCommand;
    public:
        bool initialize(Info info);
        void cleanup();

        void renderTriangle();
    };
}
