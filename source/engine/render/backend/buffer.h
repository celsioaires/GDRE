#pragma once

#include "context.h"
#include "utils.h"

namespace backend {
    class Buffer {
        static inline const Context* sContext;
        vk::Buffer mHandle;
        VmaAllocation mAllocation;
        vk::DeviceSize mSize;
        vk::DeviceAddress mAddress;
    public:
        ~Buffer();

        bool initialize(const Context& context, BufferType type, vk::DeviceSize size, const void* data = nullptr);

        void mapMemory(const void* data) const;

        const vk::Buffer& getHandle() const { return mHandle; }
        vk::DeviceSize getSize() const { return mSize; }
        const vk::DeviceAddress getAddress() const { return mAddress; }
    private:
        static vk::Buffer initializeBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, VmaAllocation& allocation, VmaMemoryUsage memoryUsage);
    };
}
