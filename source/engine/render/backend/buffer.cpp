#include "buffer.h"

#include <spdlog/spdlog.h>

#include <vulkan/vk_enum_string_helper.h>

namespace backend {
    Buffer::~Buffer() {
        vmaDestroyBuffer(mContext->getAllocator(), mHandle, mAllocation);
    }

    bool Buffer::initialize(const Context& context, BufferType type, vk::DeviceSize size, const void* data) {
        mContext = &context;
        mSize = size;

        const VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_CPU_ONLY;

        vk::BufferUsageFlags usage;

        // Handle and allocation
        switch (type) {
            case eBufferTypeStaging: {
                usage = vk::BufferUsageFlagBits::eTransferSrc;
                initializeBuffer(usage, memoryUsage);
                break;
            }
            case eBufferTypeVertex: {
                usage =
                    vk::BufferUsageFlagBits::eVertexBuffer |
                    vk::BufferUsageFlagBits::eShaderDeviceAddress;

                initializeBuffer(usage, memoryUsage);
                break;
            }
            case eBufferTypeIndex: {
                usage = vk::BufferUsageFlagBits::eIndexBuffer;
                initializeBuffer(usage, memoryUsage);
                break;
            }
            case eBufferTypeUniform: {
                usage = vk::BufferUsageFlagBits::eUniformBuffer;
                initializeBuffer(usage, memoryUsage);
                break;
            }
        }

        if (mHandle == nullptr) {
            return false;
        }

        // Map memory
        if (data != nullptr) {
            mapMemory(data);
        }

        // Get address
        if (type == eBufferTypeVertex) {
            const vk::raii::Device& device = mContext->getDevice();
            const vk::BufferDeviceAddressInfo info{ .buffer = mHandle };

            mAddress = device.getBufferAddress(info);
        }

        return true;
    }

    void Buffer::mapMemory(const void* data) const {
        const VmaAllocator& allocator = mContext->getAllocator();

        void* mappedMemory;

        // Map
        if (vmaMapMemory(allocator, mAllocation, &mappedMemory) != VK_SUCCESS) {
            spdlog::error("Failed to map buffer memory");
            return;
        }

        memcpy(mappedMemory, data, mSize);

        vmaUnmapMemory(allocator, mAllocation);
    }

    bool Buffer::initializeBuffer(vk::BufferUsageFlags usage, VmaMemoryUsage memoryUsage) {
        // Allocation
        const VmaAllocationCreateInfo allocationInfo{ .usage = memoryUsage };

        // Info
        const vk::BufferCreateInfo info{
            .size = mSize,
            .usage = usage,
        };

        // Initialize
        VkResult result = vmaCreateBuffer(
            mContext->getAllocator(),
            reinterpret_cast<const VkBufferCreateInfo*>(&info),
            &allocationInfo,
            reinterpret_cast<VkBuffer*>(&mHandle),
            &mAllocation,
            nullptr
        );

        std::string type = "Unknown";

        if (usage & vk::BufferUsageFlagBits::eVertexBuffer) {
            type = "Vertex";
        }
        else if (usage & vk::BufferUsageFlagBits::eIndexBuffer) {
            type = "Index";
        }
        else if (usage & vk::BufferUsageFlagBits::eUniformBuffer) {
            type = "Uniform";
        }

        if (result != VK_SUCCESS) {
            // Failed
            spdlog::error("Failed to create {} buffer: {}", std::tolower(type[0]), string_VkResult(result));
            return false;
        }

        spdlog::info("{} buffer created successfully", type);

        return true;
    }
}
