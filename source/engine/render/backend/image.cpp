#include "image.h"

#include <spdlog/spdlog.h>

#include <vulkan/vk_enum_string_helper.h>

#include "buffer.h"

namespace backend {
    bool Image::initialize(const Context& context, ImageType type, vk::Extent2D extent) {
        mContext = &context;
        mExtent = extent;

        bool failed = false;

        // Handle, allocation and view
        switch (type) {
            case eImageTypeDepth: {
                mFormat = vk::Format::eD32Sfloat;

                failed =
                    !initializeImage(vk::ImageUsageFlagBits::eDepthStencilAttachment) ||
                    !initializeView(vk::ImageAspectFlagBits::eDepth);

                break;
            }
            case eImageTypeSampler: {
                mFormat = vk::Format::eR8G8B8A8Unorm;

                failed =
                    !initializeImage(vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst) ||
                    !initializeView(vk::ImageAspectFlagBits::eColor);

                break;
            }
        }

        if (failed) {
            return false;
        }

        // Sampler
        if (!initializeSampler()) {
            return false;
        }

        return true;
    }

    void Image::cleanup() {
        mView = nullptr;

        vmaDestroyImage(mContext->getAllocator(), mHandle, mAllocation);

        mFormat = vk::Format::eUndefined;
        mExtent = vk::Extent2D{ 0, 0 };
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
                default: {
                    spdlog::error("Failed to create unknown image: {}", string_VkResult(result));
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
            default: {
                spdlog::info("Unknown image created successfully");
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
                default: {
                    spdlog::info("Unknown view created successfully");
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
                default: {
                    spdlog::error("Failed to create unknown view: {}", error.what());
                    break;
                }
            }

            return false;
        }

        return true;
    }

    bool Image::initializeSampler() {
        // Info
        const vk::SamplerCreateInfo info{
            .magFilter = vk::Filter::eLinear,
            .minFilter = vk::Filter::eLinear
        };

        // Initialize
        try {
            mSampler = vk::raii::Sampler(mContext->getDevice(), info);
            spdlog::info("Sampler created successfully");
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("Failed to create sampler: {}", error.what());
            return false;
        }

        return true;
    }
}
