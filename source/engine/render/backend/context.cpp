#include "context.h"

#include <filesystem>

#include <spdlog/spdlog.h>

namespace backend {
    bool Context::initialize(GLFWwindow* window) {
        if (
            (mInstance = initializeInstance(mHandle)) == nullptr ||
#if !defined (NDEBUG)
            (mDebugMessenger = initializeDebugMessenger(mInstance)) == nullptr ||
#endif
            (mSurface = initializeSurface(mInstance, window)) == nullptr ||
            (mPhysicalDevice = initializePhysicalDevice(mInstance, mSurface, mGraphicsFamily)) == nullptr ||
            (mDevice = initializeDevice(mPhysicalDevice, mGraphicsQueue, mGraphicsFamily)) == nullptr ||
            (mSubmitFence = initializeFence(mDevice)) == nullptr ||
            (mAcquireSemaphore = initializeSemaphore(mDevice)) == nullptr ||
            (mSubmitSemaphore = initializeSemaphore(mDevice)) == nullptr
        ) {
            return false;
        }

        return true;
    }

    vk::raii::Instance Context::initializeInstance(const vk::raii::Context& context) {
        // Application info
        const vk::ApplicationInfo applicationInfo{
            .pApplicationName = "GDRE",
            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
            .pEngineName = "GDRE",
            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
            .apiVersion = vk::ApiVersion14
        };

        // Extensions
        std::vector<const char*> extensions;

        uint32_t glfwExtensionCount;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        for (uint32_t i = 0; i < glfwExtensionCount; i++) {
            extensions.push_back(glfwExtensions[i]);
        }

#if !defined (NDEBUG)
        extensions.push_back(vk::EXTDebugUtilsExtensionName);
#endif

        // Layers
        std::vector<const char*> layers;

#if !defined (NDEBUG)
        layers.push_back("VK_LAYER_KHRONOS_validation");
#endif

        // Info
        const vk::InstanceCreateInfo createInfo{
            .pApplicationInfo = &applicationInfo,
            .enabledLayerCount = static_cast<uint32_t>(layers.size()),
            .ppEnabledLayerNames = layers.data(),
            .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
            .ppEnabledExtensionNames = extensions.data()
        };

        vk::raii::Instance handle = nullptr;

        try {
            // Create instance
            handle = vk::raii::Instance(context, createInfo);
            spdlog::info("Instance created successfully");
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("Failed to create instance: {}", error.what());
            return nullptr;
        }

        return handle;
    }

    vk::raii::DebugUtilsMessengerEXT Context::initializeDebugMessenger(const vk::raii::Instance& instance) {
        // Info
        const vk::DebugUtilsMessengerCreateInfoEXT createInfo{
            .messageSeverity =
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
            .messageType =
                vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
            .pfnUserCallback = debugCallback
        };

        vk::raii::DebugUtilsMessengerEXT handle = nullptr;

        try {
            // Create debug messenger
            handle = vk::raii::DebugUtilsMessengerEXT(instance, createInfo);
            spdlog::info("Debug messenger created successfully");
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("Failed to create debug messenger: {}", error.what());
            return nullptr;
        }

        return handle;
    }

    vk::raii::SurfaceKHR Context::initializeSurface(const vk::raii::Instance& instance, GLFWwindow* window) {
        // Surface
        VkSurfaceKHR surface;

        if (glfwCreateWindowSurface(*instance, window, nullptr, &surface) != VK_SUCCESS) {
            spdlog::error("Failed to create surface");
            return nullptr;
        }

        vk::raii::SurfaceKHR handle = nullptr;

        try {
            // Create surface
            handle = vk::raii::SurfaceKHR(instance, surface);
            spdlog::info("Surface created successfully");
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("Failed to create surface: {}", error.what());
            return nullptr;
        }

        return handle;
    }

    vk::raii::PhysicalDevice Context::initializePhysicalDevice(const vk::raii::Instance &instance, const vk::raii::SurfaceKHR& surface, uint32_t& graphicsFamily) {
        vk::raii::PhysicalDevice handle = nullptr;

        // Physical devices
        const std::vector<vk::raii::PhysicalDevice> devices = instance.enumeratePhysicalDevices();

        // Iterate
        for (const vk::raii::PhysicalDevice& device : devices) {
            bool vulkan14Support, featuresSupport, swapchainSupport, graphicsQueueSupport;

            // Vulkan 1.4 support
            if (device.getProperties().apiVersion >= vk::ApiVersion14) {
                vulkan14Support = true;
            }

            // Features support
            const vk::StructureChain<
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
                supportedFeatures.get<vk::PhysicalDeviceVulkan13Features>().synchronization2 &&
                supportedFeatures.get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering
            ) {
                featuresSupport = true;
            }

            // Swapchain extension support
            const std::vector<vk::ExtensionProperties> extensionProperties = device.enumerateDeviceExtensionProperties();

            for (const vk::ExtensionProperties& extensionProperty : extensionProperties) {
                if (strcmp(extensionProperty.extensionName, vk::KHRSwapchainExtensionName) == 0) {
                    swapchainSupport = true;
                }
            }

            // Graphics queue support
            const std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

            int i = 0;

            for (const vk::QueueFamilyProperties& queueFamily : queueFamilies) {
                if (
                    queueFamily.queueFlags & vk::QueueFlagBits::eGraphics &&
                    device.getSurfaceSupportKHR(i, *surface) // ¿Surface present support?
                ) {
                    graphicsQueueSupport = true;
                    graphicsFamily = i;
                }

                i++;
            }

            // ¿Support?
            if (vulkan14Support && featuresSupport && swapchainSupport && graphicsQueueSupport) {
                handle = device;
                spdlog::info("Physical device selected successfully");
                break;
            }
        }

        // ¿Failed?
        if (handle == nullptr) {
            spdlog::error("Failed to select physical device");
            return nullptr;
        }

        return handle;
    }

    vk::raii::Device Context::initializeDevice(const vk::raii::PhysicalDevice& physicalDevice, vk::raii::Queue& queue, uint32_t queueFamily) {
        // Queue
        const float queuePriority = 0.5f;

        const vk::DeviceQueueCreateInfo deviceQueueInfo{
            .queueFamilyIndex = queueFamily,
            .queueCount = 1,
            .pQueuePriorities = &queuePriority
        };

        // Features
        const vk::StructureChain<
            vk::PhysicalDeviceFeatures2,
            vk::PhysicalDeviceVulkan11Features,
            vk::PhysicalDeviceVulkan13Features
        > deviceFeatures{
            {},
            { .shaderDrawParameters = true },
            {
                .synchronization2 = true,
                .dynamicRendering = true
            }
        };

        // Extensions
        const std::vector<const char*> deviceExtensions{ vk::KHRSwapchainExtensionName };

        // Info
        vk::DeviceCreateInfo createInfo{
            .pNext = &deviceFeatures.get<vk::PhysicalDeviceFeatures2>(),
            .queueCreateInfoCount = 1,
            .pQueueCreateInfos = &deviceQueueInfo,
            .enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
            .ppEnabledExtensionNames = deviceExtensions.data()
        };

        vk::raii::Device handle = nullptr;

        try {
            // Create device
            handle = vk::raii::Device(physicalDevice, createInfo);
            spdlog::info("Device created successfully");

            // Retrieve queue
            queue = vk::raii::Queue(handle, queueFamily, 0);
            spdlog::info("Retrieved queue successfully");
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("Failed to create device: {}", error.what());
            return nullptr;
        }

        return handle;
    }

    vk::raii::Fence Context::initializeFence(const vk::raii::Device& device) {
        // Info
        const vk::FenceCreateInfo createInfo{
            .flags = vk::FenceCreateFlags::BitsType::eSignaled
        };

        vk::raii::Fence handle = nullptr;

        try {
            // Create fence
            handle = vk::raii::Fence(device, createInfo);
            spdlog::info("Fence created successfully");
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("Failed to create fence: {}", error.what());
            return nullptr;
        }

        return handle;
    }

    vk::raii::Semaphore Context::initializeSemaphore(const vk::raii::Device& device) {
        // Info
        const vk::SemaphoreCreateInfo info{};

        vk::raii::Semaphore handle = nullptr;

        try {
            // Create semaphore
            handle = vk::raii::Semaphore(device, info);
            spdlog::info("Semaphore created successfully");
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("Failed to create semaphore: {}", error.what());
            return nullptr;
        }

        return handle;
    }

    VKAPI_ATTR vk::Bool32 VKAPI_CALL Context::debugCallback(
        vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
        vk::DebugUtilsMessageTypeFlagsEXT type,
        const vk::DebugUtilsMessengerCallbackDataEXT* callbackData,
        void* userData
    ) {
        spdlog::error("Validation error:\n{}", callbackData->pMessage);
        return vk::False;
    }
}
