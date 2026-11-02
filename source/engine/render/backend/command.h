#pragma once

#include "context.h"
#include "constants.h"

namespace backend {
    class Command {
        vk::raii::CommandPool mPool = nullptr;
        vk::raii::CommandBuffer mBuffer = nullptr;
    public:
        bool initialize(const Context& context, uint32_t queueFamily);

        void pipelineBarrier(vk::Image image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout) const;
        void beginRendering(vk::Extent2D extent, vk::AttachmentLoadOp loadOp, const vk::raii::ImageView& colorView, const vk::raii::ImageView& depthView = nullptr) const;
        void setRenderExtent(vk::Extent2D extent) const;
        void pushConstants(const Constants& constants, const vk::raii::PipelineLayout& pipelineLayout) const;

        // Getters
        const vk::raii::CommandBuffer& getBuffer() const { return mBuffer; }
    private:
        static inline const Context* sContext = nullptr; // const Context& context

        // Initializers
        static vk::raii::CommandPool initializePool(uint32_t queueFamily);
        static vk::raii::CommandBuffer initializeBuffer(const vk::raii::CommandPool& pool);
    };
}
