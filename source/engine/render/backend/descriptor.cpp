#include "descriptor.h"

#include <spdlog/spdlog.h>

namespace backend {
    bool Descriptor::initialize(const Context& context, const std::vector<vk::DescriptorPoolSize>& poolSizes) {
        sContext = &context;

        // Pool
        mPool = initializeDescriptorPool(poolSizes);

        if (mPool == nullptr) {
            return false;
        }

        return true;
    }

    vk::raii::DescriptorSet Descriptor::allocateSet(const vk::raii::DescriptorSetLayout& layout) const {
        // Info
        const vk::DescriptorSetAllocateInfo info{
            .descriptorPool = *mPool,
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

    vk::raii::DescriptorPool Descriptor::initializeDescriptorPool(const std::vector<vk::DescriptorPoolSize>& sizes) {
        // Info
        const vk::DescriptorPoolCreateInfo info{
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
