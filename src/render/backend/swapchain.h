#pragma once

#include "context.h"

namespace backend {
    class Swapchain {
        vk::Extent2D mExtent;
        inline static const vk::Format sFormat = vk::Format::eB8G8R8A8Srgb;
        vk::raii::SwapchainKHR mHandle = nullptr;
        std::vector<vk::Image> mImages;
        std::vector<vk::raii::ImageView> mViews;
    public:
        bool initialize(const Context& context, uint32_t width, uint32_t height);

        // Getters
        const vk::Extent2D& getExtent() { return mExtent; }
        const vk::raii::SwapchainKHR& getHandle() { return mHandle; }
        const std::vector<vk::Image>& getImages() { return mImages; }
        const std::vector<vk::raii::ImageView>& getViews() { return mViews; }
    private:
        inline static const Context* sContext = nullptr; // const Context& context

        // Initializers
        static vk::raii::SwapchainKHR initializeSwapchain(const vk::Extent2D& extent);
        static std::vector<vk::Image> initializeImages(const vk::raii::SwapchainKHR& swapchain);
        static std::vector<vk::raii::ImageView> initializeViews(const std::vector<vk::Image>& images);
    };
}
