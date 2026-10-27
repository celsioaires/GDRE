#include "context.h"

#include <spdlog/spdlog.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace vku {
    bool Context::initialize(GLFWwindow* window) {
          // Application info
        vk::ApplicationInfo applicationInfo{
            .pApplicationName = "GDRE",
            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
            .pEngineName = "GDRE",
            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
            .apiVersion = vk::ApiVersion14
        };

        // Instance extensions
        uint32_t instanceExtensionCount;
        const char** instanceExtensions = glfwGetRequiredInstanceExtensions(&instanceExtensionCount);

        // Layers
        std::vector<const char*> layers;

#if !defined (NDEBUG)
        layers.push_back("VK_LAYER_KHRONOS_validation");
#endif

        // Instance info
        vk::InstanceCreateInfo instanceInfo{
            .pApplicationInfo = &applicationInfo,
            .enabledLayerCount = static_cast<uint32_t>(layers.size()),
            .ppEnabledLayerNames = layers.data(),
            .enabledExtensionCount = instanceExtensionCount,
            .ppEnabledExtensionNames = instanceExtensions
        };

        try {
            // Create instance
            mInstance = vk::raii::Instance(mContext, instanceInfo);
            spdlog::info("VKU created instance successfully");
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("VKU error: {}", error.what());
            return false;
        }

        // Surface
        VkSurfaceKHR surface;

        if (glfwCreateWindowSurface(*mInstance, window, nullptr, &surface) != VK_SUCCESS) {
            spdlog::error("VKU failed to create GLFW surface");
            return false;
        }

        try {
            // Create surface
            mSurface = vk::raii::SurfaceKHR(mInstance, surface);
            spdlog::info("VKU created surface successfully");
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("VKU error: {}", error.what());
            return false;
        }

        // Physical devices
        std::vector<vk::raii::PhysicalDevice> physicalDevices = mInstance.enumeratePhysicalDevices();
        uint32_t graphicsFamilyIndex;

        // Iterate
        for (const vk::raii::PhysicalDevice& device : physicalDevices) {
            bool vulkan14Support, featuresSupport, swapchainSupport, graphicsQueueSupport;

            // Vulkan 1.4 support
            if (device.getProperties().apiVersion >= vk::ApiVersion14) {
                vulkan14Support = true;
            }

            // Features support (dynamic rendering)
            vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features> features =
                device.getFeatures2<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features>();

            if (features.get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering) {
                featuresSupport = true;
            }

            // Swapchain extension support
            std::vector<vk::ExtensionProperties> extensionProperties = device.enumerateDeviceExtensionProperties();

            for (const vk::ExtensionProperties& extensionProperty : extensionProperties) {
                if (strcmp(extensionProperty.extensionName, vk::KHRSwapchainExtensionName) == 0) {
                    swapchainSupport = true;
                }
            }

            // Graphics queue support
            std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

            int i = 0;

            for (const vk::QueueFamilyProperties& queueFamily : queueFamilies) {
                if (
                    queueFamily.queueFlags & vk::QueueFlagBits::eGraphics &&
                    device.getSurfaceSupportKHR(i, *mSurface) // ¿Surface present support?
                ) {
                    graphicsQueueSupport = true;
                    graphicsFamilyIndex = i;
                }

                i++;
            }

            // ¿Support?
            if (vulkan14Support && featuresSupport && swapchainSupport && graphicsQueueSupport) {
                mPhysicalDevice = device;
                spdlog::info("VKU selected physical device successfully");
                break;
            }
        }

        // ¿Failed?
        if (mPhysicalDevice == nullptr) {
            spdlog::error("VKU failed to select physical device");
            return false;
        }

        // Device queue
        float queuePriority = 0.5f;

        vk::DeviceQueueCreateInfo deviceQueueInfo{
            .queueFamilyIndex = graphicsFamilyIndex,
            .queueCount = 1,
            .pQueuePriorities = &queuePriority
        };

        // Device extensions
        std::vector<const char*> deviceExtensions{ vk::KHRSwapchainExtensionName };

        // Device info
        vk::DeviceCreateInfo deviceInfo{
            .queueCreateInfoCount = 1,
            .pQueueCreateInfos = &deviceQueueInfo,
            .enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
            .ppEnabledExtensionNames = deviceExtensions.data()
        };

        try {
            // Create device
            mDevice = vk::raii::Device(mPhysicalDevice, deviceInfo);
            spdlog::info("VKU created device successfully");

            // Retrieve graphics queue
            mGraphicsQueue = vk::raii::Queue(mDevice, graphicsFamilyIndex, 0);
            spdlog::info("VKU retrieved graphics queue successfully");
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("VKU error: {}", error.what());
            return false;
        }

        return true;
    }
}
