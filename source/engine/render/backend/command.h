#pragma once

#include "context.h"
#include "utils.h"
#include "buffer.h"
#include "image.h"

namespace backend {
    class Command {
        // Objects
        const Context* mContext;
        vk::raii::CommandPool mPool = nullptr;
        vk::raii::CommandBuffer mBuffer = nullptr;
    public:
        bool initialize(const Context& context, uint32_t queueFamily);

        void pipelineImageBarrier(vk::Image image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout) const;
        void beginRendering(vk::Extent2D extent, vk::AttachmentLoadOp loadOp, const vk::raii::ImageView& colorView, const vk::raii::ImageView& depthView = nullptr) const;
        void setRenderExtent(vk::Extent2D extent) const;
        void copyBufferToImage(const Buffer& buffer, const Image& image, vk::Extent2D extent) const;

        // Getters
        const vk::raii::CommandBuffer& getBuffer() const { return mBuffer; }
    private:
        // Initializers
        bool initializeCommandPool(uint32_t queueFamily);
        bool initializeCommandBuffer();
    };
}
