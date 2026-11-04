#pragma once

#include "context.h"
#include "utils.h"

namespace backend {
    class Command {
        static inline const Context* sContext;

        vk::raii::CommandPool mPool = nullptr;
        vk::raii::CommandBuffer mBuffer = nullptr;
    public:
        bool initialize(const Context& context, uint32_t queueFamily);

        void pipelineBarrier(vk::Image image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout) const;
        void beginRendering(vk::Extent2D extent, vk::AttachmentLoadOp loadOp, const vk::raii::ImageView& colorView, const vk::raii::ImageView& depthView = nullptr) const;
        void setRenderExtent(vk::Extent2D extent) const;

        // Getters
        const vk::raii::CommandBuffer& getBuffer() const { return mBuffer; }
    private:
        // Initializers
        static vk::raii::CommandPool initializeCommandPool(uint32_t queueFamily);
        static vk::raii::CommandBuffer initializeCommandBuffer(const vk::raii::CommandPool& pool);
    };
}
