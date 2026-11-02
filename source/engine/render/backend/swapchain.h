#pragma once

#include "context.h"

namespace backend {
    class Swapchain {
        vk::Extent2D mExtent;
        vk::raii::SwapchainKHR mHandle = nullptr;
        std::vector<vk::Image> mImages;
        std::vector<vk::raii::ImageView> mViews;
        uint32_t mImageIndex;
    public:
        static inline const vk::Format cImageFormat = vk::Format::eB8G8R8A8Unorm;

        bool initialize(const Context& context, uint32_t width, uint32_t height);

        const vk::Image acquireImage(const vk::raii::Semaphore& signalSemaphore);

        // Getters
        const vk::Extent2D getExtent() const { return mExtent; }
        const vk::raii::SwapchainKHR& getHandle() const { return mHandle; }
        const uint32_t getImageIndex() const { return mImageIndex; }
        const vk::Image& getCurrentImage() const { return mImages[mImageIndex]; }
        const vk::raii::ImageView& getCurrentView() const { return mViews[mImageIndex]; }
    private:
        static inline const Context* sContext = nullptr; // const Context& context

        // Initializers
        static vk::raii::SwapchainKHR initializeSwapchain(const vk::Extent2D& extent);
        static std::vector<vk::Image> initializeImages(const vk::raii::SwapchainKHR& swapchain);
        static std::vector<vk::raii::ImageView> initializeViews(const std::vector<vk::Image>& images);
    };
}
