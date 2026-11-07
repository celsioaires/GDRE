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
        Descriptor mDefaultDescriptor;
        DescriptorLayout mDefaultDescriptorLayout;
        PushConstants mDrawPushConstants;
        Pipeline mDefaultPipeline;
    public:
        bool initialize(GLFWwindow* window);
        void cleanup();

        void beginCommands();
        void endCommands();
        void waitCommands();

        void beginRender(const DescriptorSet& descriptorSet);
        void endRender();

        void drawVertices(const PushConstantsData& pushConstantsData, uint32_t count);

        // Getters
        const Context& getContext() const { return mContext; }
        const Swapchain& getSwapchain() const { return mSwapchain; }
        const Command& getCommand() const { return mCommand; }
        const DescriptorLayout& getDefaultDescriptorLayout() const { return mDefaultDescriptorLayout; }
        const Descriptor& getDefaultDescriptor() const { return mDefaultDescriptor; };
    };
}
