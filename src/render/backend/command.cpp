#include "command.h"

#include <spdlog/spdlog.h>

namespace backend {
    bool Command::initialize(const Context& context, uint32_t queueFamily) {
        pContext = &context;

        // Pool
        mPool = initializePool(queueFamily);

        if (mPool == nullptr) {
            return false;
        }

        // Pool
        mBuffer = initializeBuffer();

        if (mBuffer == nullptr) {
            return false;
        }

        return true;
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
            handle = vk::raii::CommandPool(pContext->getDevice(), createInfo);
            spdlog::info("Command pool created successfully");
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("Failed to create command pool: {}", error.what());
            return nullptr;
        }

        return handle;
    }

    vk::raii::CommandBuffer Command::initializeBuffer() {
        // Info
        vk::CommandBufferAllocateInfo allocateInfo{
            .commandPool = mPool,
            .commandBufferCount = 1
        };

        vk::raii::CommandBuffer handle = nullptr;

        try {
            // Allocate buffer
            handle = std::move(vk::raii::CommandBuffers(pContext->getDevice(), allocateInfo).front());
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
