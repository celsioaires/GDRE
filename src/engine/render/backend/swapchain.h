#pragma once

#include "context.h"

namespace backend {
    class Swapchain {
        vk::Extent2D mExtent;
        vk::raii::SwapchainKHR mHandle = nullptr;
        std::vector<vk::Image> mImages;
        std::vector<vk::raii::ImageView> mViews;
    public:
        bool initialize(const Context& context, uint32_t width, uint32_t height);

        // Getters
        const vk::Extent2D& getExtent() const { return mExtent; }
        const vk::raii::SwapchainKHR& getHandle() const { return mHandle; }
        const std::vector<vk::Image>& getImages() const { return mImages; }
        const std::vector<vk::raii::ImageView>& getViews() const { return mViews; }
    private:
        static inline const Context* pContext = nullptr; // const Context& context
        static inline const vk::Format C_IMAGE_FORMAT = vk::Format::eB8G8R8A8Srgb;

        // Initializers
        static vk::raii::SwapchainKHR initializeSwapchain(const vk::Extent2D& extent);
        static std::vector<vk::Image> initializeImages(const vk::raii::SwapchainKHR& swapchain);
        static std::vector<vk::raii::ImageView> initializeViews(const std::vector<vk::Image>& images);
    };
}
