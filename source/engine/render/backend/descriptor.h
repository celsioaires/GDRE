#pragma once

#include "context.h"

namespace backend {
    class DescriptorLayout {
        static inline const Context* sContext;
        vk::raii::DescriptorSetLayout mHandle = nullptr;
    public:
        bool initialize(const Context& context, const std::vector<vk::DescriptorSetLayoutBinding>& bindings);

        // Getters
        const vk::raii::DescriptorSetLayout& getHandle() const { return mHandle; }
    private:
        // Initializers
        static vk::raii::DescriptorSetLayout initializeDescriptorLayout(const std::vector<vk::DescriptorSetLayoutBinding>& bindings);
    };

    struct DescriptorSet {
        vk::raii::DescriptorSet handle = nullptr;
    };

    class Descriptor {
        static inline const Context* sContext;
        vk::raii::DescriptorPool mPool = nullptr;
        std::vector<vk::raii::DescriptorSet> mSets;
    public:
        bool initialize(const Context& context, const std::vector<vk::DescriptorPoolSize>& poolSizes);

        vk::raii::DescriptorSet allocateSet(const vk::raii::DescriptorSetLayout& layout) const;
        void updateSet(const vk::raii::DescriptorSet& set, uint32_t binding, vk::DescriptorType type, vk::Buffer buffer = nullptr) const;
    private:
        // Initializers
        static vk::raii::DescriptorPool initializeDescriptorPool(const std::vector<vk::DescriptorPoolSize>& sizes);
    };
}
