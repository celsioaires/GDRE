#pragma once

#include "context.h"
#include "image.h"

namespace backend {
    class DescriptorLayout {
        const Context* mContext = nullptr;
        vk::raii::DescriptorSetLayout mHandle = nullptr;
    public:
        bool initialize(const Context& context, const std::vector<vk::DescriptorSetLayoutBinding>& bindings);

        // Getters
        const vk::raii::DescriptorSetLayout& getHandle() const { return mHandle; }
    private:
        // Initializers
        bool initializeDescriptorLayout(const std::vector<vk::DescriptorSetLayoutBinding>& bindings);
    };

    class DescriptorPool {
        const Context* mContext = nullptr;
        vk::raii::DescriptorPool mHandle = nullptr;
    public:
        bool initialize(const Context& context, const std::vector<vk::DescriptorPoolSize>& poolSizes);

        // Getters
        const vk::raii::DescriptorPool& getHandle() const { return mHandle; }
    private:
        // Initializers
        bool initializeDescriptorPool(const std::vector<vk::DescriptorPoolSize>& sizes);
    };

    class DescriptorSet {
        const Context* mContext;
        vk::raii::DescriptorSet mHandle = nullptr;
    public:
        bool initialize(const Context& context, const DescriptorPool& pool, const DescriptorLayout& layout);

        void updateBuffer(uint32_t binding, vk::DescriptorType type, vk::Buffer buffer) const;
        void updateImage(uint32_t binding, vk::DescriptorType type, const Image& image) const;

        // Getters
        const vk::raii::DescriptorSet& getHandle() const { return mHandle; }
    private:
        // Initializers
        bool initializeDescriptorSet(const vk::raii::DescriptorPool& pool, const vk::raii::DescriptorSetLayout& layout);
    };
}
