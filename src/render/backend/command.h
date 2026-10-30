#pragma once

#include "context.h"

namespace backend {
    class Command {
        vk::raii::CommandPool mPool = nullptr;
        vk::raii::CommandBuffer mBuffer = nullptr;
    public:
        bool initialize(const Context& context, uint32_t queueFamily);

        void pipelineBarrier(vk::Image image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);

        // Getters
        const vk::raii::CommandBuffer& getBuffer() { return mBuffer; }
    private:
        inline static const Context* sContext = nullptr; // const Context& context

        // Initializers
        static vk::raii::CommandPool initializePool(uint32_t queueFamily);
        static vk::raii::CommandBuffer initializeBuffer(const vk::raii::CommandPool& pool);
    };
}
