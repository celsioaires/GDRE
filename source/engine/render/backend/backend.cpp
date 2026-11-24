#include "backend.h"

#include <spdlog/spdlog.h>

#include <vulkan/vk_enum_string_helper.h>

namespace backend {
    bool Backend::initialize(GLFWwindow* window) {
        const uint32_t graphicsFamily = mContext.getGraphicsFamily();

        // Context, swapchain and commands
        if (
            !mContext.initialize(window) ||
            !mSwapchain.initialize(mContext) ||
            !mCommand.initialize(mContext, graphicsFamily) ||
            !mImmediateCommand.initialize(mContext, graphicsFamily)
        ) {
            return false;
        }

        // Default descriptor pool
        if (!mDefaultDescriptorPool.initialize(
            mContext, {
                { .type = vk::DescriptorType::eUniformBuffer, .descriptorCount = 10 },
                { .type = vk::DescriptorType::eCombinedImageSampler, .descriptorCount = 10000 }
            }
        )) {
            return false;
        }

        // Descriptor layouts
        if (
            // Camera
            !mCameraDescriptorLayout.initialize(
                mContext, {
                    {
                        .binding = 0,
                        .descriptorType = vk::DescriptorType::eUniformBuffer,
                        .descriptorCount = 1,
                        .stageFlags = vk::ShaderStageFlagBits::eVertex
                    }
                }
            ) ||
            // Material
            !mMaterialDescriptorLayout.initialize(
                mContext, {
                    {
                        .binding = 0,
                        .descriptorType = vk::DescriptorType::eUniformBuffer,
                        .descriptorCount = 1,
                        .stageFlags = vk::ShaderStageFlagBits::eFragment
                    }, {
                        .binding = 1,
                        .descriptorType = vk::DescriptorType::eCombinedImageSampler,
                        .descriptorCount = 1,
                        .stageFlags = vk::ShaderStageFlagBits::eFragment
                    }, {
                        .binding = 2,
                        .descriptorType = vk::DescriptorType::eCombinedImageSampler,
                        .descriptorCount = 1,
                        .stageFlags = vk::ShaderStageFlagBits::eFragment
                    }
                }
            )
        ) {
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
            { *mCameraDescriptorLayout.getHandle(), *mMaterialDescriptorLayout.getHandle() },
            { mDrawPushConstants }
        )) {
            return false;
        }

        // Depth image
        if (!mDepthImage.initialize(mContext, eImageTypeDepth, mSwapchain.getExtent())) {
            return false;
        }

        return true;
    }

    void Backend::beginCommands() {
        vk::Image swapchainImage = nullptr;

        try {
            // Acquire swapchain image
            swapchainImage = mSwapchain.acquireImage(mContext.getAcquireSemaphore());

            if (swapchainImage == nullptr) {
                mContext.waitIdle();

                // Cleanup resources
                mDepthImage.cleanup();
                mSwapchain.cleanup();

                // Initialize resources
                mSwapchain.initialize(mContext);
                mDepthImage.initialize(mContext, eImageTypeDepth, mSwapchain.getExtent());

                beginCommands(); // Retry
            }

            // Reset fence
            mContext.resetFence();

            // Begin command buffer
            mCommand.getBuffer().begin({});
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("Failed to begin render: {}", error.what());
        }

        // Swapchain image barrier (undefined to color)
        mCommand.pipelineImageBarrier(
            swapchainImage,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eColorAttachmentOptimal
        );

        // Depth image barrier (undefined to depth)
        mCommand.pipelineImageBarrier(
            mDepthImage.getHandle(),
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eDepthAttachmentOptimal
        );
    }

    void Backend::endCommands() {
        const vk::raii::CommandBuffer& commandBuffer = mCommand.getBuffer();

        // Swapchain image barrier (color to present)
        mCommand.pipelineImageBarrier(
            mSwapchain.getCurrentImage(),
            vk::ImageLayout::eColorAttachmentOptimal,
            vk::ImageLayout::ePresentSrcKHR
        );

        try {
            commandBuffer.end();

            mContext.submitQueue(commandBuffer);

            // Present swapchain
            if (!mContext.presentQueue(mSwapchain.getHandle(), mSwapchain.getImageIndex())) {
                mContext.waitIdle();

                // Cleanup resources
                mDepthImage.cleanup();
                mSwapchain.cleanup();

                // Initialize resources
                mSwapchain.initialize(mContext);
                mDepthImage.initialize(mContext, eImageTypeDepth, mSwapchain.getExtent());
            }
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("Failed to end render: {}", error.what());
        }
    }

    void Backend::waitCommands() {
        mContext.waitForFence();
    }

    void Backend::beginImmediateCommands() const {
        mContext.resetFence(true);

        mImmediateCommand.getBuffer().begin({});
    }

    void Backend::endImmediateCommands() const {
        const vk::raii::CommandBuffer& commandBuffer = mImmediateCommand.getBuffer();

        commandBuffer.end();

        mContext.submitQueue(commandBuffer, true);
        mContext.waitForFence(true);
    }

    void Backend::beginRender(const DescriptorSet& matrixDescriptor) const {
        const vk::raii::CommandBuffer& commandBuffer = mCommand.getBuffer();
        const vk::Extent2D swapchainExtent = mSwapchain.getExtent();
        const vk::PipelineBindPoint bindPoint = mDefaultPipeline.getBindPoint();

        // Begin rendering
        mCommand.beginRendering(
            swapchainExtent,
            vk::AttachmentLoadOp::eClear,
            mSwapchain.getCurrentView(),
            mDepthImage.getView()
        );

        // Bind default pipeline
        commandBuffer.bindPipeline(bindPoint, *mDefaultPipeline.getHandle());

        // Set render extent
        mCommand.setRenderExtent(swapchainExtent);

        // Bind descriptor sets
        commandBuffer.bindDescriptorSets(
            bindPoint,
            *mDefaultPipeline.getLayout(),
            0,
            *matrixDescriptor.getHandle(),
            {}
        );
    }

    void Backend::endRender() const {
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

    void Backend::drawPrimitives(const PushConstantsData& pushConstants, const DescriptorSet& materialDescriptor, const Buffer& buffer, uint32_t count) const {
        const vk::raii::CommandBuffer& commandBuffer = mCommand.getBuffer();

        commandBuffer.bindDescriptorSets(
            mDefaultPipeline.getBindPoint(),
            *mDefaultPipeline.getLayout(),
            1,
            *materialDescriptor.getHandle(),
            {}
        );

        // Bind index buffer
        commandBuffer.bindIndexBuffer(buffer.getHandle(), 0, vk::IndexType::eUint32);

        // Push draw constants
        commandBuffer.pushConstants(
            *mDefaultPipeline.getLayout(),
            mDrawPushConstants.stage,
            0,
            sizeof(PushConstantsData),
            &pushConstants
        );

        // Draw indexed vertices
        commandBuffer.drawIndexed(count, 1, 0, 0, 0);
    }

    void Backend::waitContext() const {
        mContext.waitIdle();
    }
}
