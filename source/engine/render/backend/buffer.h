#pragma once

#include "context.h"
#include "utils.h"

namespace backend {
    class Buffer {
        static inline const Context* sContext;

        // Objects
        vk::Buffer mHandle;
        VmaAllocation mAllocation;
        vk::DeviceSize mSize;
        vk::DeviceAddress mAddress;
    public:
        Buffer() = default;
        ~Buffer();

        Buffer(Buffer&& other) noexcept :
            mHandle(std::exchange(other.mHandle, vk::Buffer{})),
            mAllocation(std::exchange(other.mAllocation, nullptr)),
            mSize(std::exchange(other.mSize, 0)),
            mAddress(std::exchange(other.mAddress, 0)) {}

        bool initialize(const Context& context, BufferType type, vk::DeviceSize size, const void* data = nullptr);

        void mapMemory(const void* data) const;

        const vk::Buffer& getHandle() const { return mHandle; }
        vk::DeviceSize getSize() const { return mSize; }
        const vk::DeviceAddress getAddress() const { return mAddress; }
    private:
        static vk::Buffer initializeBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, VmaAllocation& allocation, VmaMemoryUsage memoryUsage);
    };
}
