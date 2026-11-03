#include "backend.h"

#include <spdlog/spdlog.h>

#include <glm/gtc/matrix_transform.hpp>

namespace backend {
    bool Backend::initialize(BackendInfo info) {
        // Context, swapchain and command
        if (
            !mContext.initialize(info.window) ||
            !mSwapchain.initialize(mContext) ||
            !mCommand.initialize(mContext, mContext.getGraphicsFamily())
        ) {
            return false;
        }

        // Constants
        mDefaultConstants.stage = vk::ShaderStageFlagBits::eVertex;

        // Pipelines
        if (
            // Graphics
            !mDefaultPipeline.initialize(
                mContext,
                ePipelineTypeGraphics,
                { "compiled/default.spv" },
                mDefaultConstants
            )
            // Compute
        ) {
            return false;
        }

        // Triangle vertices
        const std::vector<Vertex> triangleVertices{
            { .position{ 0.0, -0.5 }, .color{ 1, 0, 0 } },
            { .position{ 0.5, 0.5 }, .color{ 0, 1, 0 } },
            { .position{ -0.5, 0.5 }, .color{ 0, 0, 1 } }
        };

        // Triangle buffer
        if (!mTriangleBuffer.initialize(
            mContext,
            eBufferTypeVertex,
            triangleVertices.size() * sizeof(Vertex),
            triangleVertices.data()
        )) {
            return false;
        }

        return true;
    }

    void Backend::cleanup() {
        mContext.waitIdle();
    }

    void Backend::beginRender() {
        const vk::raii::Device& device = mContext.getDevice();
        const vk::raii::Fence& submitFence = mContext.getSubmitFence();
        const vk::raii::CommandBuffer& commandBuffer = mCommand.getBuffer();

        vk::Image swapchainImage;

        try {
            // Wait fence
            vk::Result waitResult = device.waitForFences(*submitFence, true, UINT64_MAX);

            if (waitResult != vk::Result::eSuccess) {

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

                beginRender();
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

    void Backend::endRender() {
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

    void Backend::renderSquare() {
        const vk::raii::CommandBuffer& commandBuffer = mCommand.getBuffer();
        const vk::Extent2D swapchainExtent = mSwapchain.getExtent();

        // Begin rendering
        mCommand.beginRendering(swapchainExtent, vk::AttachmentLoadOp::eClear, mSwapchain.getCurrentView());

        // Bind default pipeline
        commandBuffer.bindPipeline(mDefaultPipeline.getBindPoint(), *mDefaultPipeline.getHandle());

        // Set render extent
        mCommand.setRenderExtent(swapchainExtent);

        // Push constants
        mDefaultConstants.data = {
            .vertexBuffer = mTriangleBuffer.getAddress(),
            .transformMatrix = glm::translate(glm::mat4(1), glm::vec3(0, 0, 0))//glm::rotate(glm::mat4(1), static_cast<float>(glfwGetTime()), glm::vec3(0, 0, 1))
        };

        mCommand.pushConstants(mDefaultConstants, mDefaultPipeline.getLayout());

        // Draw vertices
        commandBuffer.draw(3, 1, 0, 0);

        // End rendering
        commandBuffer.endRendering();
    }
}
