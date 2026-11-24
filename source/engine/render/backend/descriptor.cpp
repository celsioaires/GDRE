#include "descriptor.h"

#include <spdlog/spdlog.h>

namespace backend {
    bool DescriptorLayout::initialize(const Context& context, const std::vector<vk::DescriptorSetLayoutBinding>& bindings) {
        mContext = &context;

        // Handle
        if (!initializeDescriptorLayout(bindings)) {
            return false;
        }

        return true;
    }

    bool DescriptorLayout::initializeDescriptorLayout(const std::vector<vk::DescriptorSetLayoutBinding>& bindings) {
        // Info
        const vk::DescriptorSetLayoutCreateInfo info{
            .bindingCount = static_cast<uint32_t>(bindings.size()),
            .pBindings = bindings.data()
        };

        // Initialize
        try {
            mHandle = vk::raii::DescriptorSetLayout(mContext->getDevice(), info);
            spdlog::info("Descriptor layout created successfully");
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("Failed to create descriptor layout: {}", error.what());
            return false;
        }

        return true;
    }

    bool DescriptorPool::initialize(const Context& context, const std::vector<vk::DescriptorPoolSize>& poolSizes) {
        mContext = &context;

        // Pool
        if (!initializeDescriptorPool(poolSizes)) {
            return false;
        }

        return true;
    }

    bool DescriptorPool::initializeDescriptorPool(const std::vector<vk::DescriptorPoolSize>& sizes) {
        // Info
        const vk::DescriptorPoolCreateInfo info{
            .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
            .maxSets = 10000,
            .poolSizeCount = static_cast<uint32_t>(sizes.size()),
            .pPoolSizes = sizes.data()
        };

        // Initialize
        try {
            mHandle = vk::raii::DescriptorPool(mContext->getDevice(), info);
            spdlog::info("Descriptor pool created successfully");
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("Failed to create descriptor pool: {}", error.what());
            return false;
        }

        return true;
    }

    bool DescriptorSet::initialize(const Context& context, const DescriptorPool& pool, const DescriptorLayout& layout) {
        mContext = &context;

        if (!initializeDescriptorSet(pool.getHandle(), layout.getHandle())) {
            return false;
        }

        return true;
    }

    void DescriptorSet::updateBuffer(uint32_t binding, vk::DescriptorType type, vk::Buffer buffer) const {
        // Info
        const vk::DescriptorBufferInfo info{
            .buffer = buffer,
            .range = vk::WholeSize
        };

        // Write
        const vk::WriteDescriptorSet write{
            .dstSet = *mHandle,
            .dstBinding = binding,
            .descriptorCount = 1,
            .descriptorType = type,
            .pBufferInfo = &info
        };

        // Update
        mContext->getDevice().updateDescriptorSets(write, {});
    }

    void DescriptorSet::updateImage(uint32_t binding, vk::DescriptorType type, const Image& image) const {
        // Info
        const vk::DescriptorImageInfo info{
            .sampler = *image.getSampler(),
            .imageView = *image.getView(),
            .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
        };

        // Write
        const vk::WriteDescriptorSet write{
            .dstSet = *mHandle,
            .dstBinding = binding,
            .descriptorCount = 1,
            .descriptorType = type,
            .pImageInfo = &info
        };

        // Update
        mContext->getDevice().updateDescriptorSets(write, {});
    }

    bool DescriptorSet::initializeDescriptorSet(const vk::raii::DescriptorPool& pool, const vk::raii::DescriptorSetLayout& layout) {
        // Info
        const vk::DescriptorSetAllocateInfo info{
            .descriptorPool = *pool,
            .descriptorSetCount = 1,
            .pSetLayouts = &*layout
        };

        // Initialize
        try {
            mHandle = std::move(mContext->getDevice().allocateDescriptorSets(info)[0]);
            spdlog::info("Descriptor set allocated successfully");
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("Failed to allocate descriptor set: {}", error.what());
            return false;
        }

        return true;
    }
}
