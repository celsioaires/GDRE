#include "context.h"

#include <spdlog/spdlog.h>

namespace backend {
#if !defined (NDEBUG)
    static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(
        vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
        vk::DebugUtilsMessageTypeFlagsEXT type,
        const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData
    ) {
        spdlog::error("Validation error: {}", pCallbackData->pMessage);
        return vk::False;
    }
#endif

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
        std::vector<const char*> instanceExtensions;

        uint32_t glfwExtensionCount;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        for (uint32_t i = 0; i < glfwExtensionCount; i++) {
            instanceExtensions.push_back(glfwExtensions[i]);
        }

#if !defined (NDEBUG)
        instanceExtensions.push_back(vk::EXTDebugUtilsExtensionName);
#endif

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
            .enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size()),
            .ppEnabledExtensionNames = instanceExtensions.data()
        };

        try {
            // Create instance
            mInstance = vk::raii::Instance(mHandle, instanceInfo);
            spdlog::info("Instance created successfully");
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("Failed to create instance: {}", error.what());
            return false;
        }

#if !defined (NDEBUG)
        // Debug messenger info
        vk::DebugUtilsMessengerCreateInfoEXT debugMessengerInfo{
            .messageSeverity =
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
            .messageType =
                vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
            .pfnUserCallback = debugCallback
        };

        try {
            // Create debug messenger
            mDebugMessenger = vk::raii::DebugUtilsMessengerEXT(mInstance, debugMessengerInfo);
            spdlog::info("Debug messenger created successfully");
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("Failed to create debug messenger: {}", error.what());
            return false;
        }
#endif

        // Surface
        VkSurfaceKHR surface;

        if (glfwCreateWindowSurface(*mInstance, window, nullptr, &surface) != VK_SUCCESS) {
            spdlog::error("Failed to create GLFW surface");
            return false;
        }

        try {
            // Create surface
            mSurface = vk::raii::SurfaceKHR(mInstance, surface);
            spdlog::info("Surface created successfully");
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("Failed to create surface: {}", error.what());
            return false;
        }

        // Physical devices
        std::vector<vk::raii::PhysicalDevice> physicalDevices = mInstance.enumeratePhysicalDevices();

        // Iterate
        for (const vk::raii::PhysicalDevice& device : physicalDevices) {
            bool vulkan14Support, featuresSupport, swapchainSupport, graphicsQueueSupport;

            // Vulkan 1.4 support
            if (device.getProperties().apiVersion >= vk::ApiVersion14) {
                vulkan14Support = true;
            }

            // Features support (shaderDrawParameters, dynamic rendering)
            vk::StructureChain<
                vk::PhysicalDeviceFeatures2,
                vk::PhysicalDeviceVulkan11Features,
                vk::PhysicalDeviceVulkan13Features
            > supportedFeatures = device.getFeatures2<
                vk::PhysicalDeviceFeatures2,
                vk::PhysicalDeviceVulkan11Features,
                vk::PhysicalDeviceVulkan13Features
            >();

            if (
                supportedFeatures.get<vk::PhysicalDeviceVulkan11Features>().shaderDrawParameters &&
                supportedFeatures.get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering
            ) {
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
                    mGraphicsFamily = i;
                }

                i++;
            }

            // ¿Support?
            if (vulkan14Support && featuresSupport && swapchainSupport && graphicsQueueSupport) {
                mPhysicalDevice = device;
                spdlog::info("Physical device selected successfully");
                break;
            }
        }

        // ¿Failed?
        if (mPhysicalDevice == nullptr) {
            spdlog::error("Failed to select physical device");
            return false;
        }

        // Device queue
        float queuePriority = 0.5f;

        vk::DeviceQueueCreateInfo deviceQueueInfo{
            .queueFamilyIndex = mGraphicsFamily,
            .queueCount = 1,
            .pQueuePriorities = &queuePriority
        };

        // Device features (shaderDrawParameters, dynamic rendering)
        vk::StructureChain<
            vk::PhysicalDeviceFeatures2,
            vk::PhysicalDeviceVulkan11Features,
            vk::PhysicalDeviceVulkan13Features
        > deviceFeatures = {
            {},
            { .shaderDrawParameters = true },
            { .dynamicRendering = true },
        };

        // Device extensions
        std::vector<const char*> deviceExtensions{ vk::KHRSwapchainExtensionName };

        // Device info
        vk::DeviceCreateInfo deviceInfo{
            .pNext = &deviceFeatures.get<vk::PhysicalDeviceFeatures2>(),
            .queueCreateInfoCount = 1,
            .pQueueCreateInfos = &deviceQueueInfo,
            .enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
            .ppEnabledExtensionNames = deviceExtensions.data()
        };

        try {
            // Create device
            mDevice = vk::raii::Device(mPhysicalDevice, deviceInfo);
            spdlog::info("Device created successfully");

            // Retrieve graphics queue
            mGraphicsQueue = vk::raii::Queue(mDevice, mGraphicsFamily, 0);
            spdlog::info("Retrieved graphics queue successfully");
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("Failed to create device: {}", error.what());
            return false;
        }

        return true;
    }
}
