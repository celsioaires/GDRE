#pragma once

#include "context.h"

namespace backend {
    class Swapchain {
        // Objects
        const Context* mContext = nullptr;
        vk::Extent2D mExtent;
        vk::raii::SwapchainKHR mHandle = nullptr;
        std::vector<vk::Image> mImages;
        std::vector<vk::raii::ImageView> mViews;
        uint32_t mImageIndex = 0;
    public:
        bool initialize(const Context& context);
        void cleanup();

        const vk::Image acquireImage(const vk::raii::Semaphore& signalSemaphore);

        // Getters
        const vk::Extent2D getExtent() const { return mExtent; }
        const vk::raii::SwapchainKHR& getHandle() const { return mHandle; }
        const uint32_t getImageIndex() const { return mImageIndex; }
        const vk::Image& getCurrentImage() const { return mImages[mImageIndex]; }
        const vk::raii::ImageView& getCurrentView() const { return mViews[mImageIndex]; }
    private:
        // Initializers
        bool initializeSwapchain();
        bool initializeImages();
        bool initializeViews();

        // Format
        static inline const vk::Format sFormat = vk::Format::eB8G8R8A8Unorm;
    public:
        static const vk::Format& getFormat() { return sFormat; }
    };
}
