#include "swapchain.h"

#include <spdlog/spdlog.h>

namespace vku {
    bool Swapchain::initialize(const Context& context) {
        // Surface
        const vk::raii::SurfaceKHR& surface = context.getSurface();

        // Surface capabilities
        vk::SurfaceCapabilitiesKHR surfaceCapabilities =
            context.getPhysicalDevice().getSurfaceCapabilitiesKHR(*surface);

        // Swapchain info
        vk::SwapchainCreateInfoKHR swapchainInfo{
            .surface = *surface,
            .minImageCount = 3,
            .imageFormat = vk::Format::eB8G8R8A8Srgb,
            .imageColorSpace  = vk::ColorSpaceKHR::eSrgbNonlinear,
            .imageExtent = surfaceCapabilities.currentExtent,
            .imageArrayLayers = 1,
            .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
            .imageSharingMode = vk::SharingMode::eExclusive,
            .preTransform = surfaceCapabilities.currentTransform,
            .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
            .presentMode = vk::PresentModeKHR::eImmediate,
            .clipped = true
        };

        try {
            // Create swapchain
            mSwapchain = vk::raii::SwapchainKHR(context.getDevice(), swapchainInfo);
            spdlog::info("VKU created swapchain successfully");

            // Get swapchain images
            mSwapchainImages = mSwapchain.getImages();
            spdlog::info("VKU got swapchain images successfully");
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("VKU error: {}", error.what());
            return false;
        }

        return true;
    }
}
