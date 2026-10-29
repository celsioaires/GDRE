#pragma once

#include "context.h"

namespace backend {
    class Command {
        const Context* pContext = nullptr;
        vk::raii::CommandPool mPool = nullptr;
        vk::raii::CommandBuffer mBuffer = nullptr;
    public:
        bool initialize(const Context& context, uint32_t queueFamily);
    private:
        vk::raii::CommandPool initializePool(uint32_t queueFamily);
        vk::raii::CommandBuffer initializeBuffer();
    };
}
