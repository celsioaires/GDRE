#include "command.h"

#include <spdlog/spdlog.h>

namespace backend {
    bool Command::initialize(const Context& context, uint32_t queueFamily) {
        sContext = &context;

        // Pool
        mPool = initializePool(queueFamily);

        if (mPool == nullptr) {
            return false;
        }

        // Pool
        mBuffer = initializeBuffer(mPool);

        if (mBuffer == nullptr) {
            return false;
        }

        return true;
    }

    void Command::pipelineBarrier(vk::Image image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout) {
        // Subresource
        const vk::ImageSubresourceRange subresource{
            .aspectMask = vk::ImageAspectFlagBits::eColor,
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

    vk::raii::CommandPool Command::initializePool(uint32_t queueFamily) {
        // Info
        vk::CommandPoolCreateInfo createInfo{
            .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
            .queueFamilyIndex = queueFamily
        };

        vk::raii::CommandPool handle = nullptr;

        try {
            // Create pool
            handle = vk::raii::CommandPool(sContext->getDevice(), createInfo);
            spdlog::info("Command pool created successfully");
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("Failed to create command pool: {}", error.what());
            return nullptr;
        }

        return handle;
    }

    vk::raii::CommandBuffer Command::initializeBuffer(const vk::raii::CommandPool& pool) {
        // Info
        vk::CommandBufferAllocateInfo allocateInfo{
            .commandPool = pool,
            .commandBufferCount = 1
        };

        vk::raii::CommandBuffer handle = nullptr;

        try {
            // Allocate buffer
            handle = std::move(vk::raii::CommandBuffers(sContext->getDevice(), allocateInfo).front());
            spdlog::info("Command buffer allocated successfully");
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("Failed to allocate command buffer: {}", error.what());
            return nullptr;
        }

        return handle;
    }
}
