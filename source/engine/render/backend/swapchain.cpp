#include "swapchain.h"

#include <spdlog/spdlog.h>

#include <vulkan/vk_enum_string_helper.h>

namespace backend {
    bool Swapchain::initialize(const Context& context) {
        sContext = &context;

        // Window size
        int windowWidth, windowHeight;
        glfwGetWindowSize(sContext->getWindow(), &windowWidth, &windowHeight);

        // Extent
        mExtent = vk::Extent2D{
            .width = static_cast<uint32_t>(windowWidth),
            .height = static_cast<uint32_t>(windowHeight)
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

        if (mViews.back() == nullptr) {
            return false;
        }

        spdlog::info("Swapchain views created successfully");

        return true;
    }

    void Swapchain::cleanup() {
        sContext = nullptr;

        mViews.clear();
        mHandle = nullptr;
        mImages.clear();
        mExtent = vk::Extent2D{};
        mImageIndex = 0;
    }

    const vk::Image Swapchain::acquireImage(const vk::raii::Semaphore& signalSemaphore) {
        vk::ResultValue<uint32_t> resultValue = mHandle.acquireNextImage(UINT64_MAX, *signalSemaphore, nullptr);

        const vk::Result result = resultValue.result;

        if (result != vk::Result::eSuccess) {
            spdlog::error("Failed to acquire swapchain image: {}", string_VkResult(static_cast<VkResult>(result)));

            if (result == vk::Result::eErrorOutOfDateKHR) {
                return nullptr;
            }
        }

        mImageIndex = resultValue.value;

        return mImages[mImageIndex];
    }

    vk::raii::SwapchainKHR Swapchain::initializeSwapchain(const vk::Extent2D& extent) {
        // Surface
        const vk::raii::SurfaceKHR& surface = sContext->getSurface();

        // Info
        const vk::SwapchainCreateInfoKHR createInfo{
            .surface = *surface,
            .minImageCount = 3,
            .imageFormat = cImageFormat,
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
            .format = cImageFormat,
            .subresourceRange = subresource
        };

        std::vector<vk::raii::ImageView> views;

        for (vk::Image image : images) {
            info.image = image;

            vk::raii::ImageView view = nullptr;

            try {
                // Create view
                view = vk::raii::ImageView(sContext->getDevice(), info);
            }
            catch (const vk::SystemError& error) {
                // Failed
                spdlog::error("Failed to create view: {}", error.what());
                views.push_back(std::move(view));

                return views;
            }

            views.push_back(std::move(view));
        }

        return views;
    }
}
