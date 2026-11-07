#pragma once

#include "context.h"

namespace backend {
    class DescriptorLayout {
        vk::raii::DescriptorSetLayout mHandle = nullptr;
    public:
        bool initialize(const Context& context, const std::vector<vk::DescriptorSetLayoutBinding>& bindings);

        // Getters
        const vk::raii::DescriptorSetLayout& getHandle() const { return mHandle; }
    private:
        static inline const Context* sContext;

        // Initializers
        static vk::raii::DescriptorSetLayout initializeDescriptorLayout(const std::vector<vk::DescriptorSetLayoutBinding>& bindings);
    };

    class DescriptorSet {
        vk::raii::DescriptorSet mHandle = nullptr;
    public:
        bool initialize(const Context& context, const vk::raii::DescriptorPool& pool, const vk::raii::DescriptorSetLayout& layout);

        void updateBinding(uint32_t index, vk::DescriptorType type, vk::Buffer buffer) const;

        // Getters
        const vk::raii::DescriptorSet& getHandle() const { return mHandle; }
    private:
        static inline const Context* sContext;

        // Initializers
        static vk::raii::DescriptorSet initializeDescriptorSet(const vk::raii::DescriptorPool& pool, const vk::raii::DescriptorSetLayout& layout);
    };

    class Descriptor {
        vk::raii::DescriptorPool mPool = nullptr;
        std::vector<vk::raii::DescriptorSet> mSets;
    public:
        bool initialize(const Context& context, const std::vector<vk::DescriptorPoolSize>& poolSizes);

        // Getters
        const vk::raii::DescriptorPool& getPool() const { return mPool; }
    private:
        static inline const Context* sContext;

        // Initializers
        static vk::raii::DescriptorPool initializeDescriptorPool(const std::vector<vk::DescriptorPoolSize>& sizes);
    };
}
