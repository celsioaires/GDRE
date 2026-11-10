#include "image.h"

#include <spdlog/spdlog.h>

#include <vulkan/vk_enum_string_helper.h>

namespace backend {
    bool Image::initialize(const Context& context, ImageType type, vk::Extent2D extent) {
        mContext = &context;
        mExtent = extent;

        bool failed = false;

        // Initialize
        switch (type) {
            case eImageTypeColor: {
                mFormat = vk::Format::eB8G8R8A8Unorm;

                failed =
                    !initializeImage(vk::ImageUsageFlagBits::eColorAttachment) ||
                    !initializeView(vk::ImageAspectFlagBits::eColor);

                break;
            }
            case eImageTypeDepth: {
                mFormat = vk::Format::eD32Sfloat;

                failed =
                    !initializeImage(vk::ImageUsageFlagBits::eDepthStencilAttachment) ||
                    !initializeView(vk::ImageAspectFlagBits::eDepth);

                break;
            }
        }

        if (failed) {
            return false;
        }

        return true;
    }

    void Image::cleanup() {
        mView = nullptr;

        vmaDestroyImage(mContext->getAllocator(), mHandle, mAllocation);

        mFormat = vk::Format::eUndefined;
        mExtent = vk::Extent2D{};
        mContext = nullptr;
    }

    bool Image::initializeImage(vk::ImageUsageFlags usage) {
        // Allocation info
        VmaAllocationCreateInfo allocationInfo{ .usage = VMA_MEMORY_USAGE_GPU_ONLY };

        // Info
        vk::ImageCreateInfo info{
            .imageType = vk::ImageType::e2D,
            .format = mFormat,
            .extent = { mExtent.width, mExtent.height, 1 },
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = vk::SampleCountFlagBits::e1,
            .usage = usage
        };

        // Initialize
        VkResult result = vmaCreateImage(
            mContext->getAllocator(),
            info,
            &allocationInfo,
            reinterpret_cast<VkImage*>(&mHandle),
            &mAllocation,
            nullptr
        );

        if (result != VK_SUCCESS) {
            // Failed
            switch (static_cast<VkImageUsageFlags>(usage)) {
                case VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT: {
                    spdlog::error("Failed to create color image: {}", string_VkResult(result));
                    break;
                }
                case VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT: {
                    spdlog::error("Failed to create depth image: {}", string_VkResult(result));
                    break;
                }
            }

            return false;
        }

        // Successfully
        switch (static_cast<VkImageUsageFlags>(usage)) {
            case VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT: {
                spdlog::info("Color image created successfully");
                break;
            }
            case VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT: {
                spdlog::info("Depth image created successfully");
                break;
            }
        }

        return true;
    }

    bool Image::initializeView(vk::ImageAspectFlags aspect) {
        // Subresource
        vk::ImageSubresourceRange subresource{
            .aspectMask = aspect,
            .levelCount = 1,
            .layerCount = 1
        };

        // Info
        vk::ImageViewCreateInfo info {
            .image = mHandle,
            .viewType = vk::ImageViewType::e2D,
            .format = mFormat,
            .subresourceRange = subresource
        };

        // Initialize
        try {
            mView = vk::raii::ImageView(mContext->getDevice(), info);

            // Successfully
            switch (static_cast<VkImageAspectFlags>(aspect)) {
                case VK_IMAGE_ASPECT_COLOR_BIT: {
                    spdlog::info("Color view created successfully");
                    break;
                }
                case VK_IMAGE_ASPECT_DEPTH_BIT: {
                    spdlog::info("Depth view created successfully");
                    break;
                }
            }
        }
        catch (const vk::SystemError& error) {
            // Failed
            switch (static_cast<VkImageAspectFlags>(aspect)) {
                case VK_IMAGE_ASPECT_COLOR_BIT: {
                    spdlog::error("Failed to create color view: {}", error.what());
                    break;
                }
                case VK_IMAGE_ASPECT_DEPTH_BIT: {
                    spdlog::error("Failed to create depth view: {}", error.what());
                    break;
                }
            }

            return false;
        }

        return true;
    }
}
