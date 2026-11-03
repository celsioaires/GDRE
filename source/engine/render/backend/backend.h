#pragma once

#include "context.h"
#include "swapchain.h"
#include "pipeline.h"
#include "command.h"
#include "utils.h"
#include "buffer.h"

namespace backend {
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

        void renderSquare();

        // Getters
        const Context& getContext() const { return mContext; }
        const Swapchain& getSwapchain() const { return mSwapchain; }
        const Command& getCommand() const { return mCommand; }
    };
}
