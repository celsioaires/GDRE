#include "command.h"

#include <spdlog/spdlog.h>

namespace backend {
    bool Command::initialize(const Context& context, uint32_t queueFamily) {
        mContext = &context;

        // Pool
        if (!initializeCommandPool(queueFamily)) {
            return false;
        }

        // Buffer
        if (!initializeCommandBuffer()) {
            return false;
        }

        return true;
    }

    void Command::pipelineBarrier(vk::Image image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout) const {
        // Aspect
        vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor;

        if (newLayout == vk::ImageLayout::eDepthAttachmentOptimal) {
            aspect = vk::ImageAspectFlagBits::eDepth;
        }

        // Subresource
        const vk::ImageSubresourceRange subresource{
            .aspectMask = aspect,
            .levelCount = 1,
            .layerCount = 1
        };

        // Barrier
        const vk::ImageMemoryBarrier2 barrier{
            .dstStageMask = vk::PipelineStageFlagBits2::eAllCommands,
            .dstAccessMask = vk::AccessFlagBits2::eMemoryRead,
            .oldLayout = oldLayout,
            .newLayout = newLayout,
            .image = image,
            .subresourceRange = subresource
        };

        // Info
        const vk::DependencyInfo info{
            .imageMemoryBarrierCount = 1,
            .pImageMemoryBarriers = &barrier
        };

        mBuffer.pipelineBarrier2(info);
    }

    void Command::beginRendering(vk::Extent2D extent, vk::AttachmentLoadOp loadOp, const vk::raii::ImageView& colorView, const vk::raii::ImageView& depthView) const {
        vk::ClearValue clearValue = vk::ClearColorValue{ 0, 0, 0, 1 };

        // Color attachment
        const vk::RenderingAttachmentInfo colorAttachment{
            .imageView = *colorView,
            .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
            .loadOp = loadOp,
            .clearValue = clearValue
        };

        // Depth attachment
        vk::RenderingAttachmentInfo depthAttachment;

        if (depthView != nullptr) {
            clearValue = vk::ClearDepthStencilValue{ 1, 0 };

            depthAttachment = {
                .imageView = *depthView,
                .imageLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
                .loadOp = loadOp,
                .clearValue = clearValue
            };
        }

        // Info
        const vk::RenderingInfo info{
            .renderArea = { .extent = extent },
            .layerCount = 1,
            .colorAttachmentCount = 1,
            .pColorAttachments = &colorAttachment,
            .pDepthAttachment = &depthAttachment
        };

        mBuffer.beginRendering(info);
    }

    void Command::setRenderExtent(vk::Extent2D extent) const {
        // Viewport
        const vk::Viewport viewport{
            .width = static_cast<float>(extent.width),
            .height = static_cast<float>(extent.height),
            .maxDepth = 1.0f,
        };

        mBuffer.setViewport(0, viewport);

        // Scissor
        const vk::Rect2D scissor{ .extent = extent };

        mBuffer.setScissor(0, scissor);
    }

    bool Command::initializeCommandPool(uint32_t queueFamily) {
        // Info
        const vk::CommandPoolCreateInfo createInfo{
            .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
            .queueFamilyIndex = queueFamily
        };

        // Initialize
        try {
            mPool = vk::raii::CommandPool(mContext->getDevice(), createInfo);
            spdlog::info("Command pool created successfully");
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("Failed to create command pool: {}", error.what());
            return false;
        }

        return true;
    }

    bool Command::initializeCommandBuffer() {
        // Info
        const vk::CommandBufferAllocateInfo allocateInfo{
            .commandPool = mPool,
            .commandBufferCount = 1
        };

        // Initialize
        try {
            mBuffer = std::move(vk::raii::CommandBuffers(mContext->getDevice(), allocateInfo).front());
            spdlog::info("Command buffer allocated successfully");
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("Failed to allocate command buffer: {}", error.what());
            return false;
        }

        return true;
    }
}
