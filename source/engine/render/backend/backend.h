#pragma once

#include "context.h"
#include "swapchain.h"
#include "pipeline.h"
#include "command.h"
#include "buffer.h"

namespace backend {
    struct BackendInfo {
        GLFWwindow* window;
        uint32_t width;
        uint32_t height;
    };

    class Backend {
        Context mContext;
        Swapchain mSwapchain;
        Command mCommand;
        Constants mDefaultConstants;
        Pipeline mDefaultPipeline;
        Buffer mTriangleBuffer;
    public:
        bool initialize(BackendInfo info);
        void cleanup();

        void beginRender();
        void endRender();

        void renderCube();

        // Getters
        const Context& getContext() const { return mContext; }
        const Swapchain& getSwapchain() const { return mSwapchain; }
        const Command& getCommand() const { return mCommand; }
    };
}
