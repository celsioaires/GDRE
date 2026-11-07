#include "backend.h"

#include <spdlog/spdlog.h>

#include <vulkan/vk_enum_string_helper.h>

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

        // Draw push constants
        mDrawPushConstants = {
            .data = { .transformMatrix = glm::mat4(1) },
            .stage = vk::ShaderStageFlagBits::eVertex
        };

        // Default pipeline
        if (!mDefaultPipeline.initialize(
            mContext,
            ePipelineTypeGraphics,
            { "compiled/default.spv" },
            mDefaultDescriptorLayout.getHandle(),
            { mDrawPushConstants }
        )) {
            return false;
        }

        return true;
    }

    void Backend::cleanup() {
        mContext.waitIdle();
    }

    void Backend::beginCommands() {
        vk::Image swapchainImage;

        try {
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
            mContext.getDevice().resetFences(*mContext.getSubmitFence());

            // Begin command buffer
            mCommand.getBuffer().begin({});
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

    void Backend::waitCommands() {
        // Wait submit fence
        vk::Result waitResult = mContext.getDevice().waitForFences(*mContext.getSubmitFence(), true, UINT64_MAX);

        if (waitResult != vk::Result::eSuccess) {
            spdlog::error("Failed to wait for fence", string_VkResult(static_cast<VkResult>(waitResult)));
        }
    }

    void Backend::beginRender(const DescriptorSet& descriptorSet) {
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
        commandBuffer.bindDescriptorSets(bindPoint, *mDefaultPipeline.getLayout(), 0, *descriptorSet.getHandle(), {});
    }

    void Backend::endRender() {
        // End rendering
        mCommand.getBuffer().endRendering();
    }

    void Backend::drawVertices(const PushConstantsData& pushConstants, uint32_t count) {
        const vk::raii::CommandBuffer& commandBuffer = mCommand.getBuffer();

        // Push draw constants
        mDrawPushConstants.data = pushConstants;

        commandBuffer.pushConstants(
            *mDefaultPipeline.getLayout(),
            mDrawPushConstants.stage,
            0,
            sizeof(PushConstantsData),
            &mDrawPushConstants.data
        );

        // Draw vertices
        commandBuffer.draw(count, 1, 0, 0);
    }
}
