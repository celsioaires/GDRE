#pragma once

#include "context.h"
#include "swapchain.h"
#include "command.h"
#include "pipeline.h"
#include "descriptor.h"
#include "buffer.h"
#include "image.h"
#include "utils.h"

namespace backend {
    class Backend {
        Context mContext;
        Swapchain mSwapchain;
        Command mCommand;
        Command mImmediateCommand;
        Descriptor mDefaultDescriptor;
        DescriptorLayout mCameraDescriptorLayout;
        DescriptorLayout mMaterialDescriptorLayout;
        PushConstants mDrawPushConstants;
        Pipeline mDefaultPipeline;
        Image mDepthImage;
    public:
        bool initialize(GLFWwindow* window);

        // Commands
        void beginCommands();
        void endCommands();
        void waitCommands();

        void beginImmediateCommands() const;
        void endImmediateCommands() const;

        // Render
        void beginRender(const DescriptorSet& matrixDescriptor) const;
        void endRender() const;

        // Draw
        void drawVertices(const PushConstantsData& pushConstants, uint32_t count);
        void drawIndexedVertices(const PushConstantsData& pushConstants, const DescriptorSet& materialDescriptor, const Buffer& buffer, uint32_t count) const;

        void waitContext() const;

        // Getters
        const Context& getContext() const { return mContext; }
        const Swapchain& getSwapchain() const { return mSwapchain; }
        const Command& getCommand() const { return mCommand; }
        const Command& getImmediateCommand() const { return mImmediateCommand; }
        const Descriptor& getDefaultDescriptor() const { return mDefaultDescriptor; };
        const DescriptorLayout& getCameraDescriptorLayout() const { return mCameraDescriptorLayout; }
        const DescriptorLayout& getMaterialDescriptorLayout() const { return mMaterialDescriptorLayout; }
    };
}
