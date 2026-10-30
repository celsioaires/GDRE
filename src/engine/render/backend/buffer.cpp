#include "buffer.h"

#include <spdlog/spdlog.h>

namespace backend {
    bool Buffer::initialize(const Context &context) {
        sContext = &context;

        // Handle
        mHandle = initializeBuffer();

        if (mHandle == nullptr) {
            return false;
        }
    }

    vk::raii::Buffer Buffer::initializeBuffer() {
        // Info
        const vk::BufferCreateInfo info{

        };

        vk::raii::Buffer handle = nullptr;

        try {
            // Create buffer
            handle = vk::raii::Buffer(sContext->getDevice(), info);
            spdlog::info("Buffer created successfully");
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("Failed to create buffer: {}", error.what());
            return nullptr;
        }

        return handle;
    }
}
