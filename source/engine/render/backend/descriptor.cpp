#include "descriptor.h"

#include <spdlog/spdlog.h>

namespace backend {
    bool DescriptorLayout::initialize(const Context& context, const std::vector<vk::DescriptorSetLayoutBinding>& bindings) {
        sContext = &context;

        // Handle
        mHandle = initializeDescriptorLayout(bindings);

        if (mHandle == nullptr) {
            return false;
        }

        return true;
    }

    vk::raii::DescriptorSetLayout DescriptorLayout::initializeDescriptorLayout(const std::vector<vk::DescriptorSetLayoutBinding>& bindings) {
        // Info
        const vk::DescriptorSetLayoutCreateInfo info{
            .bindingCount = static_cast<uint32_t>(bindings.size()),
            .pBindings = bindings.data()
        };

        vk::raii::DescriptorSetLayout handle = nullptr;

        try {
            // Create pool
            handle = vk::raii::DescriptorSetLayout(sContext->getDevice(), info);
            spdlog::info("Descriptor layout created successfully");
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("Failed to create descriptor layout: {}", error.what());
            return nullptr;
        }

        return handle;
    }

    bool DescriptorSet::initialize(const Context& context, const vk::raii::DescriptorPool& pool, const vk::raii::DescriptorSetLayout& layout) {
        sContext = &context;

        mHandle = initializeDescriptorSet(pool, layout);

        if (mHandle == nullptr) {
            return false;
        }

        return true;
    }

    void DescriptorSet::updateBinding(uint32_t index, vk::DescriptorType type, vk::Buffer buffer) const {
        // Buffer info
        vk::DescriptorBufferInfo bufferInfo;

        if (buffer != nullptr) {
            bufferInfo.buffer = buffer;
            bufferInfo.range = vk::WholeSize;
        }

        // Write
        const vk::WriteDescriptorSet write{
            .dstSet = *mHandle,
            .dstBinding = index,
            .descriptorCount = 1,
            .descriptorType = type,
            .pBufferInfo = &bufferInfo
        };

        // Update
        sContext->getDevice().updateDescriptorSets(write, {});
    }

    vk::raii::DescriptorSet DescriptorSet::initializeDescriptorSet(const vk::raii::DescriptorPool& pool, const vk::raii::DescriptorSetLayout& layout) {
        // Info
        const vk::DescriptorSetAllocateInfo info{
            .descriptorPool = *pool,
            .descriptorSetCount = 1,
            .pSetLayouts = &*layout
        };

        vk::raii::DescriptorSet handle = nullptr;

        try {
            // Allocate set
            handle = std::move(sContext->getDevice().allocateDescriptorSets(info)[0]);
            spdlog::info("Descriptor set allocated successfully");
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("Failed to allocate descriptor set: {}", error.what());
            return nullptr;
        }

        return handle;
    }

    bool Descriptor::initialize(const Context& context, const std::vector<vk::DescriptorPoolSize>& poolSizes) {
        sContext = &context;

        // Pool
        mPool = initializeDescriptorPool(poolSizes);

        if (mPool == nullptr) {
            return false;
        }

        return true;
    }

    vk::raii::DescriptorPool Descriptor::initializeDescriptorPool(const std::vector<vk::DescriptorPoolSize>& sizes) {
        // Info
        const vk::DescriptorPoolCreateInfo info{
            .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
            .maxSets = 1,
            .poolSizeCount = static_cast<uint32_t>(sizes.size()),
            .pPoolSizes = sizes.data()
        };

        vk::raii::DescriptorPool handle = nullptr;

        try {
            // Create pool
            handle = vk::raii::DescriptorPool(sContext->getDevice(), info);
            spdlog::info("Descriptor pool created successfully");
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("Failed to create descriptor pool: {}", error.what());
            return nullptr;
        }

        return handle;
    }
}
