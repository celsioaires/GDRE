#include "backend.h"

#include <spdlog/spdlog.h>

namespace backend {
    bool Backend::initialize(Info info) {
        if (
            !mContext.initialize(info.window) ||
            !mSwapchain.initialize(mContext, info.width, info.height)
        ) {
            return false;
        }

        if (!mCommand.initialize(mContext, mContext.getGraphicsFamily())) {
            return false;
        }

        // Pipelines
        if (
            // Graphics
            !mDefaultPipeline.initialize(mContext, vk::PipelineBindPoint::eGraphics, { "compiled/default.spv" })
            // Compute
        ) {
            return false;
        }

        return true;
    }

    void Backend::cleanup() {
        const vk::raii::Device& device = mContext.getDevice();

        try {
            device.waitIdle();
            spdlog::info("Wait idle device successfully");
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("Failed to wait idle device: {}", error.what());
        }
    }

    void Backend::renderTriangle() {
        const vk::raii::Device& device = mContext.getDevice();
        const vk::raii::Fence& submitFence = mContext.getSubmitFence();
        const vk::raii::SwapchainKHR& swapchain = mSwapchain.getHandle();
        const vk::raii::CommandBuffer& commandBuffer = mCommand.getBuffer();
        const vk::Extent2D swapchainExtent = mSwapchain.getExtent();
        const vk::raii::Semaphore& acquireSemaphore = mContext.getAcquireSemaphore();
        const vk::raii::Semaphore& submitSemaphore = mContext.getSubmitSemaphore();
        const vk::raii::Queue& graphicsQueue = mContext.getGraphicsQueue();

        uint32_t imageIndex;

        try {
            // Wait fence
            vk::Result waitResult = device.waitForFences(*submitFence, vk::True, UINT64_MAX);

            if (waitResult != vk::Result::eSuccess) {

            }

            // Reset fence
            device.resetFences(*submitFence);

            // Acquire swapchain image
            vk::ResultValue<uint32_t> acquireResult = swapchain.acquireNextImage(UINT64_MAX, acquireSemaphore, nullptr);

            if (acquireResult.result != vk::Result::eSuccess) {

            }

            imageIndex = acquireResult.value;

            // Begin command buffer
            commandBuffer.begin(vk::CommandBufferBeginInfo{});
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("test() error: {}", error.what());
        }

        const vk::Image swapchainImage = mSwapchain.getImages()[imageIndex];

        // Image barrier
        mCommand.pipelineBarrier(swapchainImage, vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal);

        // Rendering attachments
        const std::vector<vk::RenderingAttachmentInfo> renderingAttachments{
            // Color
            {
                .imageView = mSwapchain.getViews()[imageIndex],
                .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
                .loadOp = vk::AttachmentLoadOp::eClear
            }
        };

        // Begin rendering
        mCommand.beginRendering(swapchainExtent, renderingAttachments);

        // Bind default pipeline
        commandBuffer.bindPipeline(mDefaultPipeline.getBindPoint(), *mDefaultPipeline.getHandle());

        // Set viewport
        mCommand.setViewport(swapchainExtent);

        // Set scissor
        commandBuffer.setScissor(0, vk::Rect2D{ .extent = swapchainExtent });

        // Draw
        commandBuffer.draw(3, 1, 0, 0);

        // End rendering
        commandBuffer.endRendering();

        // Image barrier
        mCommand.pipelineBarrier(swapchainImage, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR);

        try {
            commandBuffer.end();

            const vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;

            // Submit info
            const vk::SubmitInfo submitInfo{
                .waitSemaphoreCount = 1,
                .pWaitSemaphores = &*acquireSemaphore,
                .pWaitDstStageMask = &waitStage,
                .commandBufferCount = 1,
                .pCommandBuffers = &*commandBuffer,
                .signalSemaphoreCount = 1,
                .pSignalSemaphores = &*submitSemaphore
            };

            // Submit command buffer
            graphicsQueue.submit(submitInfo, *submitFence);

            // Present info
            const vk::PresentInfoKHR presentInfo{
                .waitSemaphoreCount = 1,
                .pWaitSemaphores = &*submitSemaphore,
                .swapchainCount = 1,
                .pSwapchains = &*swapchain,
                .pImageIndices = &imageIndex
            };

            // Present swapchain
            vk::Result presentResult = graphicsQueue.presentKHR(presentInfo);

            if (presentResult != vk::Result::eSuccess) {

            }
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("test() error: {}", error.what());
        }
    }
}
