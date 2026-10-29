#include "swapchain.h"

#include <spdlog/spdlog.h>

namespace backend {
    bool Swapchain::initialize(const Context& context, uint32_t width, uint32_t height) {
        // Extent
        mExtent = vk::Extent2D{
            .width = width,
            .height = height
        };

        // Surface
        const vk::raii::SurfaceKHR& surface = context.getSurface();

        // Info
        vk::SwapchainCreateInfoKHR createInfo{
            .surface = *surface,
            .minImageCount = 3,
            .imageFormat = vk::Format::eB8G8R8A8Srgb,
            .imageExtent = mExtent,
            .imageArrayLayers = 1,
            .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
            .clipped = true
        };

        try {
            // Create swapchain
            mHandle = vk::raii::SwapchainKHR(context.getDevice(), createInfo);
            spdlog::info("Swapchain created successfully");

            // Get swapchain images
            mImages = mHandle.getImages();
            spdlog::info("Swapchain images retrieved successfully");
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("Failed to create swapchain: {}", error.what());
            return false;
        }

        return true;
    }
}
