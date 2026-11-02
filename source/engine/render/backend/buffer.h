#pragma once

#include "context.h"

namespace backend {
    enum BufferType {
        eBufferTypeStaging,
        eBufferTypeVertex,
        eBufferTypeIndex,
        eBufferTypeUniform,
    };

    class Buffer {
        VkBuffer mHandle;
        VmaAllocation mAllocation;
        vk::DeviceSize mSize;
        vk::DeviceAddress mAddress;
    public:
        bool initialize(const Context& context, BufferType type, vk::DeviceSize size, const void* data = nullptr);

        ~Buffer();

        vk::DeviceSize getSize() const { return mSize; }
        vk::DeviceAddress getAddress() const { return mAddress; }
    private:
        static inline const Context* sContext = nullptr; // const Context& context

        // Initializers
        static VkBuffer initializeBuffer(vk::DeviceSize size, VkBufferUsageFlags usage, VmaAllocation* allocation, VmaMemoryUsage memoryUsage);
    };
}
