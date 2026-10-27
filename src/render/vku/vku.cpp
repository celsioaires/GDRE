#include "vku.h"

#include <spdlog/spdlog.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace vku {
    bool VKU::initialize() {
        vk::ApplicationInfo applicationInfo{
            .pApplicationName = "GDRE",
            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
            .pEngineName = "GDRE",
            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
            .apiVersion = vk::ApiVersion14
        };

        uint32_t extensionCount;
        const char** extensions = glfwGetRequiredInstanceExtensions(&extensionCount);

        std::vector<const char*> layers;

#if !defined (NDEBUG)
        layers.push_back("VK_LAYER_KHRONOS_validation");
#endif

        vk::InstanceCreateInfo instanceInfo{
            .pApplicationInfo = &applicationInfo,
            .enabledLayerCount = static_cast<uint32_t>(layers.size()),
            .ppEnabledLayerNames = layers.data(),
            .enabledExtensionCount = extensionCount,
            .ppEnabledExtensionNames = extensions
        };

        try {
            mInstance = vk::raii::Instance(mContext, instanceInfo);
            spdlog::info("VKU instance created successfully");
        }
        catch (const vk::SystemError& error) {
            spdlog::error("VKU error: {}", error.what());
            return false;
        }

        uint32_t graphicsQueueIndex;

        std::vector<vk::raii::PhysicalDevice> physicalDevices = mInstance.enumeratePhysicalDevices();

        for (const vk::raii::PhysicalDevice& device : physicalDevices) {
            bool vulkan14Support;

            if (device.getProperties().apiVersion >= vk::ApiVersion14) {
                vulkan14Support = true;
            }

            bool featuresSupport;

            vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features> features =
                device.getFeatures2<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features>();

            if (features.get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering) {
                featuresSupport = true;
            }

            bool swapchainSupport;

            std::vector<vk::ExtensionProperties> extensionProperties = device.enumerateDeviceExtensionProperties();

            for (const vk::ExtensionProperties& extensionProperty : extensionProperties) {
                if (strcmp(extensionProperty.extensionName, vk::KHRSwapchainExtensionName) == 0) {
                    swapchainSupport = true;
                }
            }

            bool graphicsQueueSupport;

            std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

            int i = 0;

            for (const vk::QueueFamilyProperties& queueFamily : queueFamilies) {
                if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
                    graphicsQueueSupport = true;
                    graphicsQueueIndex = i;
                }

                i++;
            }

            if (vulkan14Support && featuresSupport && swapchainSupport && graphicsQueueSupport) {
                mPhysicalDevice = device;
                break;
            }
        }

        if (mPhysicalDevice == nullptr) {
            spdlog::error("VKU failed to pick physical device");
            return false;
        }

        float queuePriority = 0.5f;

        vk::DeviceQueueCreateInfo deviceQueueInfo{
            .queueFamilyIndex = graphicsQueueIndex,
            .queueCount = 1,
            .pQueuePriorities = &queuePriority
        };

        const char* swapchainExtension = vk::KHRSwapchainExtensionName;

        vk::DeviceCreateInfo deviceInfo{
            .queueCreateInfoCount = 1,
            .pQueueCreateInfos = &deviceQueueInfo,
            .enabledExtensionCount = 1,
            .ppEnabledExtensionNames = &swapchainExtension
        };

        try {
            mDevice = vk::raii::Device(mPhysicalDevice, deviceInfo);
            spdlog::info("VKU device created successfully");
        }
        catch (const vk::SystemError& error) {
            spdlog::error("VKU error: {}", error.what());
            return false;
        }

        return true;
    }
}
