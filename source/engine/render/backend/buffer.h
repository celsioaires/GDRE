#pragma once

#include "context.h"
#include "utils.h"

namespace backend {
    enum BufferType {
        eBufferTypeStaging,
        eBufferTypeVertex,
        eBufferTypeIndex,
        eBufferTypeUniform,
    };

    class Buffer {
        // Objects
        const Context* mContext = nullptr;
        vk::Buffer mHandle = nullptr;
        VmaAllocation mAllocation = nullptr;
        vk::DeviceSize mSize = 0;
        vk::DeviceAddress mAddress = 0;
    public:
        ~Buffer();

        bool initialize(const Context& context, BufferType type, vk::DeviceSize size, const void* data = nullptr);

        bool mapMemory(const void* data) const;

        const vk::Buffer& getHandle() const { return mHandle; }
        vk::DeviceSize getSize() const { return mSize; }
        const vk::DeviceAddress getAddress() const { return mAddress; }
    private:
        bool initializeBuffer(vk::BufferUsageFlags usage, VmaMemoryUsage memoryUsage);
    };
}
