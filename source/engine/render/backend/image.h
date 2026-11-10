#pragma once

#include "context.h"

namespace backend {
    enum ImageType {
        eImageTypeColor,
        eImageTypeDepth
    };

    class Image {
        // Objects
        const Context* mContext;
        vk::Extent2D mExtent;
        vk::Format mFormat;
        vk::Image mHandle;
        VmaAllocation mAllocation;
        vk::raii::ImageView mView = nullptr;
    public:
        ~Image() { cleanup(); }

        bool initialize(const Context& context, ImageType type, vk::Extent2D extent);
        void cleanup();

        // Getters
        const vk::Image& getHandle() const { return mHandle; }
        const vk::raii::ImageView& getView() const { return mView; }
    private:
        // Initializers
        bool initializeImage(vk::ImageUsageFlags usage);
        bool initializeView(vk::ImageAspectFlags aspect);
    };
}