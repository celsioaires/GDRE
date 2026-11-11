#include "swapchain.h"

#include <spdlog/spdlog.h>

#include <vulkan/vk_enum_string_helper.h>

namespace backend {
    bool Swapchain::initialize(const Context& context) {
        mContext = &context;

        // Window size
        int windowWidth, windowHeight;
        glfwGetWindowSize(mContext->getWindow(), &windowWidth, &windowHeight);

        // Extent
        mExtent = vk::Extent2D{
            .width = static_cast<uint32_t>(windowWidth),
            .height = static_cast<uint32_t>(windowHeight)
        };

        // Handle
        if (!initializeSwapchain()) {
            return false;
        }

        // Images
        if (!initializeImages()) {
            return false;
        }

        // Views
        if (!initializeViews()) {
            return false;
        }

        return true;
    }

    void Swapchain::cleanup() {
        mContext = nullptr;
        mViews.clear();
        mHandle = nullptr;
        mImages.clear();
        mExtent = vk::Extent2D{};
        mImageIndex = 0;
    }

    const vk::Image Swapchain::acquireImage(const vk::raii::Semaphore& signalSemaphore) {
        vk::ResultValue<uint32_t> resultValue = mHandle.acquireNextImage(UINT64_MAX, *signalSemaphore, nullptr);

        const VkResult result = static_cast<VkResult>(resultValue.result);

        if (result != VK_SUCCESS) {
            switch (result) {
                case VK_SUBOPTIMAL_KHR: {
                    spdlog::warn("Failed to acquire swapchain image");
                    break;
                }
                case VK_ERROR_OUT_OF_DATE_KHR: {
                    spdlog::error("Failed to acquire swapchain image. The swapchain will be recreated");
                    return nullptr;
                }
                default: {
                    spdlog::warn("Failed to acquire swapchain image: {}", string_VkResult(result));
                }
            }
        }

        mImageIndex = resultValue.value;

        return mImages[mImageIndex];
    }

    bool Swapchain::initializeSwapchain() {
        // Surface
        const vk::raii::SurfaceKHR& surface = mContext->getSurface();

        // Info
        const vk::SwapchainCreateInfoKHR createInfo{
            .surface = *surface,
            .minImageCount = 3,
            .imageFormat = cImageFormat,
            .imageExtent = mExtent,
            .imageArrayLayers = 1,
            .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
            .presentMode = vk::PresentModeKHR::eFifo,
            .clipped = true
        };

        vk::raii::SwapchainKHR handle = nullptr;

        // Initialize
        try {
            mHandle = vk::raii::SwapchainKHR(mContext->getDevice(), createInfo);
            spdlog::info("Swapchain created successfully");
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("Failed to create swapchain: {}", error.what());
            return false;
        }

        return true;
    }

    bool Swapchain::initializeImages() {
        // Initialize
        try {
            mImages = mHandle.getImages();
            spdlog::info("Swapchain images retrieved successfully");
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("Failed to retrieve swapchain images: {}", error.what());
            return false;
        }

        return true;
    }

    bool Swapchain::initializeViews() {
        // Subresource
        const vk::ImageSubresourceRange subresource{
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .levelCount = 1,
            .layerCount = 1
        };

        // Info
        vk::ImageViewCreateInfo info{
            .viewType = vk::ImageViewType::e2D,
            .format = cImageFormat,
            .subresourceRange = subresource
        };

        // Views
        for (const vk::Image& image : mImages) {
            info.image = image;

            // Initialize
            try {
                mViews.emplace_back(mContext->getDevice(), info);
            }
            catch (const vk::SystemError& error) {
                // Failed
                spdlog::error("Failed to create view: {}", error.what());
                return false;
            }
        }

        spdlog::info("Swapchain views created successfully");

        return true;
    }
}
