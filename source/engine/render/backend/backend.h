#pragma once

#include "context.h"
#include "swapchain.h"
#include "command.h"
#include "pipeline.h"
#include "descriptor.h"
#include "utils.h"

namespace backend {
    class Backend {
        Context mContext;
        Swapchain mSwapchain;
        Command mCommand;
        Pipeline mDefaultPipeline;
        Descriptor mDefaultDescriptor;
    public:
        bool initialize(GLFWwindow* window);
        void cleanup();

        void beginRender();
        void endRender();

        void renderMesh(const MeshConstants& constants);

        // Getters
        const Context& getContext() const { return mContext; }
        const Swapchain& getSwapchain() const { return mSwapchain; }
        const Command& getCommand() const { return mCommand; }
    };
}
