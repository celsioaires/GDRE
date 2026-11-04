#pragma once

#include "context.h"

namespace backend {
    class DescriptorLayout {

    };

    class Descriptor {
        static inline const Context* sContext;
        vk::raii::DescriptorPool mPool = nullptr;
    public:
        bool initialize(const Context& context, const std::vector<vk::DescriptorPoolSize>& poolSizes);

        vk::raii::DescriptorSet allocateSet(const vk::raii::DescriptorSetLayout& layout) const;
    private:
        static vk::raii::DescriptorPool initializeDescriptorPool(const std::vector<vk::DescriptorPoolSize>& sizes);
        static vk::raii::DescriptorSet initializeDescriptorSet(const std::vector<vk::DescriptorSetLayout>& sets);
    };
}
