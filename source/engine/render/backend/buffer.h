#pragma once

#include "context.h"
#include "utils.h"

namespace backend {
    class Buffer {
        static inline const Context* sContext;

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
        static VkBuffer initializeBuffer(vk::DeviceSize size, VkBufferUsageFlags usage, VmaAllocation* allocation, VmaMemoryUsage memoryUsage);
    };
}
