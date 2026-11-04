#include "buffer.h"

#include <spdlog/spdlog.h>

namespace backend {
    bool Buffer::initialize(const Context& context, BufferType type, vk::DeviceSize size, const void* data) {
        sContext = &context;
        mSize = size;

        const VmaMemoryUsage memoryUsage =
            data == nullptr ? VMA_MEMORY_USAGE_GPU_ONLY : VMA_MEMORY_USAGE_CPU_ONLY;

        VkBufferUsageFlags usage;

        // Handle and allocation
        switch (type) {
            case eBufferTypeStaging: {
                usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
                mHandle = initializeBuffer(size, usage, &mAllocation, memoryUsage);
                break;
            }
            case eBufferTypeVertex: {
                usage =
                    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                    VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
                    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

                mHandle = initializeBuffer(size, usage, &mAllocation, memoryUsage);
                break;
            }
            case eBufferTypeIndex: {
                usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
                mHandle = initializeBuffer(size, usage, &mAllocation, memoryUsage);
                break;
            }
            case eBufferTypeUniform: {
                usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
                mHandle = initializeBuffer(size, usage, &mAllocation, memoryUsage);
                break;
            }
        }

        if (mHandle == nullptr) {
            return false;
        }

        // Map memory
        if (data != nullptr) {
            const VmaAllocator& allocator = sContext->getAllocator();

            void* mappedMemory;

            if (vmaMapMemory(allocator, mAllocation, &mappedMemory) != VK_SUCCESS) {
                spdlog::error("Failed to map buffer memory");
                return false;
            }

            memcpy(mappedMemory, data, size);

            vmaUnmapMemory(allocator, mAllocation);
        }

        // Get address
        if (type == eBufferTypeVertex) {
            const vk::raii::Device& device = sContext->getDevice();
            const vk::BufferDeviceAddressInfo info{ .buffer = mHandle };

            mAddress = device.getBufferAddress(info);
        }

        return true;
    }

    Buffer::~Buffer() {
        vmaDestroyBuffer(sContext->getAllocator(), mHandle, mAllocation);
    }

    VkBuffer Buffer::initializeBuffer(vk::DeviceSize size, VkBufferUsageFlags usage, VmaAllocation* allocation, VmaMemoryUsage memoryUsage) {
        // Allocation
        const VmaAllocationCreateInfo allocationInfo{
            .usage = memoryUsage
        };

        // Info
        const VkBufferCreateInfo info{
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = size,
            .usage = usage,
        };

        // Handle
        VkBuffer handle;

        if (vmaCreateBuffer(
            sContext->getAllocator(),
            &info,
            &allocationInfo,
            &handle,
            allocation,
            nullptr
        ) != VK_SUCCESS) {
            // Failed
            spdlog::error("Failed to create buffer");
            return nullptr;
        }

        spdlog::info("Buffer created successfully");

        return handle;
    }
}
