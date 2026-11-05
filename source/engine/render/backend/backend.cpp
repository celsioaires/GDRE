#include "backend.h"

#include <spdlog/spdlog.h>

namespace backend {
    bool Backend::initialize(GLFWwindow* window) {
        // Context, swapchain and command
        if (
            !mContext.initialize(window) ||
            !mSwapchain.initialize(mContext) ||
            !mCommand.initialize(mContext, mContext.getGraphicsFamily())
        ) {
            return false;
        }

        // Default descriptor
        if (!mDefaultDescriptor.initialize(
            mContext, {
                { .type = vk::DescriptorType::eUniformBuffer, .descriptorCount = 1 }
            }
        )) {
            return false;
        }

        // Default descriptor layout
        if (!mDefaultDescriptorLayout.initialize(
            mContext, {
                {
                    .binding = 0,
                    .descriptorType = vk::DescriptorType::eUniformBuffer,
                    .descriptorCount = 1,
                    .stageFlags = vk::ShaderStageFlagBits::eVertex
                }
            }
        )) {
            return false;
        }

        // Default pipeline
        if (!mDefaultPipeline.initialize(
            mContext,
            ePipelineTypeGraphics,
            { "compiled/default.spv" },
            mDefaultDescriptorLayout.getHandle(),
            { cMeshConstants }
        )) {
            return false;
        }

        return true;
    }

    void Backend::cleanup() {
        mContext.waitIdle();
    }

    void Backend::beginCommands() {
        const vk::raii::Device& device = mContext.getDevice();
        const vk::raii::Fence& submitFence = mContext.getSubmitFence();
        const vk::raii::CommandBuffer& commandBuffer = mCommand.getBuffer();

        vk::Image swapchainImage;

        try {
            // Wait fence
            vk::Result waitResult = device.waitForFences(*submitFence, true, UINT64_MAX);

            if (waitResult != vk::Result::eSuccess) {
                spdlog::warn("Failed to wait for fence");
            }

            // Acquire swapchain image
            swapchainImage = mSwapchain.acquireImage(mContext.getAcquireSemaphore());

            if (swapchainImage == nullptr) {
                // Recreate swapchain
                mContext.waitIdle();
                mSwapchain.cleanup();

                if (!mSwapchain.initialize(mContext)) {
                    spdlog::error("Failed to initialize swapchain");
                }

                beginCommands(); // Retry
            }

            // Reset fence
            device.resetFences(*submitFence);

            // Begin command buffer
            commandBuffer.begin({});
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("Failed to begin render: {}", error.what());
        }

        // Swapchain image barrier (undefined to color)
        mCommand.pipelineBarrier(swapchainImage, vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal);

    }

    void Backend::endCommands() {
        const vk::raii::CommandBuffer& commandBuffer = mCommand.getBuffer();

        // Swapchain image barrier (color to present)
        mCommand.pipelineBarrier(mSwapchain.getCurrentImage(), vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR);

        try {
            commandBuffer.end();

            mContext.submitQueue(commandBuffer);

            // Present swapchain
            if (!mContext.presentQueue(mSwapchain.getHandle(), mSwapchain.getImageIndex())) {
                // Recreate swapchain
                mContext.waitIdle();
                mSwapchain.cleanup();

                if (!mSwapchain.initialize(mContext)) {
                    spdlog::error("Failed to initialize swapchain");
                }
            }
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("Failed to end render: {}", error.what());
        }
    }

    void Backend::render(const DescriptorSet& descriptorSet, const MeshConstants& constants) {
        const vk::raii::CommandBuffer& commandBuffer = mCommand.getBuffer();
        const vk::Extent2D swapchainExtent = mSwapchain.getExtent();

        // Begin rendering
        mCommand.beginRendering(swapchainExtent, vk::AttachmentLoadOp::eClear, mSwapchain.getCurrentView());

        // Bind default pipeline
        const vk::PipelineBindPoint bindPoint = mDefaultPipeline.getBindPoint();

        commandBuffer.bindPipeline(bindPoint, *mDefaultPipeline.getHandle());

        // Set render extent
        mCommand.setRenderExtent(swapchainExtent);

        // Bind descriptor sets
        commandBuffer.bindDescriptorSets(bindPoint, *mDefaultPipeline.getLayout(), 0, *descriptorSet.handle, {});

        // Push constants
        commandBuffer.pushConstants(
            *mDefaultPipeline.getLayout(),
            cMeshConstants.stageFlags,
            0,
            sizeof(MeshConstants),
            &constants
        );

        // Draw vertices
        commandBuffer.draw(3, 1, 0, 0);

        // End rendering
        commandBuffer.endRendering();
    }
}
