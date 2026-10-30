#include "swapchain.h"

#include <spdlog/spdlog.h>

namespace backend {
    bool Swapchain::initialize(const Context& context, uint32_t width, uint32_t height) {
        sContext = &context;

        // Extent
        mExtent = vk::Extent2D{
            .width = width,
            .height = height
        };

        // Handle
        mHandle = initializeSwapchain(mExtent);

        if (mHandle == nullptr) {
            return false;
        }

        // Images
        mImages = initializeImages(mHandle);

        // Views
        mViews = initializeViews(mImages);

        return true;
    }

    vk::raii::SwapchainKHR Swapchain::initializeSwapchain(const vk::Extent2D& extent) {
        // Surface
        const vk::raii::SurfaceKHR& surface = sContext->getSurface();

        // Info
        const vk::SwapchainCreateInfoKHR createInfo{
            .surface = *surface,
            .minImageCount = 3,
            .imageFormat = sFormat,
            .imageExtent = extent,
            .imageArrayLayers = 1,
            .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
            .clipped = true
        };

        vk::raii::SwapchainKHR handle = nullptr;

        try {
            // Create swapchain
            handle = vk::raii::SwapchainKHR(sContext->getDevice(), createInfo);
            spdlog::info("Swapchain created successfully");

        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("Failed to create swapchain: {}", error.what());
            return nullptr;
        }

        return handle;
    }

    std::vector<vk::Image> Swapchain::initializeImages(const vk::raii::SwapchainKHR& swapchain) {
        std::vector<vk::Image> images;

        try {
            images = swapchain.getImages();
            spdlog::info("Swapchain images retrieved successfully");
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("Failed to retrieve swapchain images: {}", error.what());
            return images;
        }

        return images;
    }

    std::vector<vk::raii::ImageView> Swapchain::initializeViews(const std::vector<vk::Image>& images) {
        // Subresource
        const vk::ImageSubresourceRange subresource{
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .levelCount = 1,
            .layerCount = 1
        };

        // Info
        vk::ImageViewCreateInfo info{
            .viewType = vk::ImageViewType::e2D,
            .format = sFormat,
            .subresourceRange = subresource
        };

        std::vector<vk::raii::ImageView> views;

        for (vk::Image image : images) {
            info.image = image;

            vk::raii::ImageView view = nullptr;

            try {
                // Create view
                view = vk::raii::ImageView(sContext->getDevice(), info);
                spdlog::info("View created successfully");
            }
            catch (const vk::SystemError& error) {
                // Failed
                spdlog::error("Failed to create view: {}", error.what());
                return views;
            }

            views.push_back(std::move(view));
        }

        return views;
    }
}
