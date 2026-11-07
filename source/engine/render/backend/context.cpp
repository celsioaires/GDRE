#define GLFW_INCLUDE_VULKAN
#define VMA_IMPLEMENTATION
#include "context.h"

#include <filesystem>

#include <spdlog/spdlog.h>

#include <vulkan/vk_enum_string_helper.h>

namespace backend {
    Context::~Context() {
        vmaDestroyAllocator(mAllocator);
    }

    bool Context::initialize(GLFWwindow* window) {
        mWindow = window;

        // Objects
        if (
            (mInstance = initializeInstance(mHandle)) == nullptr ||
#if !defined (NDEBUG)
            (mDebugMessenger = initializeDebugMessenger(mInstance)) == nullptr ||
#endif
            (mSurface = initializeSurface(mInstance, window)) == nullptr ||
            (mPhysicalDevice = initializePhysicalDevice(mInstance, mSurface, mGraphicsFamily)) == nullptr ||
            (mDevice = initializeDevice(mPhysicalDevice, mGraphicsQueue, mGraphicsFamily)) == nullptr ||
            (mAllocator = initializeAllocator(mDevice, mPhysicalDevice, mInstance)) == nullptr ||
            (mSubmitFence = initializeFence(mDevice)) == nullptr ||
            (mAcquireSemaphore = initializeSemaphore(mDevice)) == nullptr ||
            (mSubmitSemaphore = initializeSemaphore(mDevice)) == nullptr
        ) {
            return false;
        }

        return true;
    }

    void Context::submitQueue(const vk::raii::CommandBuffer& commandBuffer) const {
        // Wait stage
        const vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;

        // Submit info
        const vk::SubmitInfo info{
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &*mAcquireSemaphore,
            .pWaitDstStageMask = &waitStage,
            .commandBufferCount = 1,
            .pCommandBuffers = &*commandBuffer,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = &*mSubmitSemaphore
        };

        // Submit command buffer
        mGraphicsQueue.submit(info, *mSubmitFence);
    }

    bool Context::presentQueue(const vk::raii::SwapchainKHR& swapchain, uint32_t imageIndex) const {
        // Info
        const vk::PresentInfoKHR info{
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &*mSubmitSemaphore,
            .swapchainCount = 1,
            .pSwapchains = &*swapchain,
            .pImageIndices = &imageIndex
        };

        // Present
        const auto result = static_cast<VkResult>(mGraphicsQueue.presentKHR(info));

        if (result != VK_SUCCESS) {
            switch (result) {
                case VK_SUBOPTIMAL_KHR: {
                    spdlog::warn("Failed to present swapchain image. The swapchain will be recreated");
                    return false;
                }
                case VK_ERROR_OUT_OF_DATE_KHR: {
                    spdlog::error("Failed to present swapchain image. The swapchain will be recreated");
                    return false;
                }
                default: {
                    spdlog::warn("Failed to present swapchain image: {}", string_VkResult(result));
                }
            }
        }

        return true;
    }

    void Context::waitIdle() const {
        try {
            mDevice.waitIdle();
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("Failed to wait device idle: {}", error.what());
        }
    }

    vk::raii::Instance Context::initializeInstance(const vk::raii::Context& context) {
        // Application info
        const char* applicationName = "GDRE";
        uint32_t applicationVersion = VK_MAKE_VERSION(1, 0, 0);

        const vk::ApplicationInfo applicationInfo{
            .pApplicationName = applicationName,
            .applicationVersion = applicationVersion,
            .pEngineName = applicationName,
            .engineVersion = applicationVersion,
            .apiVersion = cApiVersion
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
        const vk::InstanceCreateInfo info{
            .pApplicationInfo = &applicationInfo,
            .enabledLayerCount = static_cast<uint32_t>(layers.size()),
            .ppEnabledLayerNames = layers.data(),
            .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
            .ppEnabledExtensionNames = extensions.data()
        };

        vk::raii::Instance handle = nullptr;

        try {
            // Create instance
            handle = vk::raii::Instance(context, info);
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
                vk::PhysicalDeviceVulkan12Features,
                vk::PhysicalDeviceVulkan13Features
            > supportedFeatures = device.getFeatures2<
                vk::PhysicalDeviceFeatures2,
                vk::PhysicalDeviceVulkan11Features,
                vk::PhysicalDeviceVulkan12Features,
                vk::PhysicalDeviceVulkan13Features
            >();

            if (
                supportedFeatures.get<vk::PhysicalDeviceVulkan11Features>().shaderDrawParameters &&
                supportedFeatures.get<vk::PhysicalDeviceVulkan12Features>().bufferDeviceAddress &&
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

        const vk::DeviceQueueCreateInfo queueInfo{
            .queueFamilyIndex = queueFamily,
            .queueCount = 1,
            .pQueuePriorities = &queuePriority
        };

        // Features
        const vk::PhysicalDeviceVulkan11Features features11{
            .shaderDrawParameters = true
        };

        const vk::PhysicalDeviceVulkan12Features features12{
            .bufferDeviceAddress = true
        };

        const vk::PhysicalDeviceVulkan13Features features13{
            .synchronization2 = true,
            .dynamicRendering = true
        };

        // Extensions
        const std::vector<const char*> extensions{ vk::KHRSwapchainExtensionName };

        // Info
        vk::DeviceCreateInfo info{
            .queueCreateInfoCount = 1,
            .pQueueCreateInfos = &queueInfo,
            .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
            .ppEnabledExtensionNames = extensions.data()
        };

        // Chain
        vk::StructureChain<
            vk::DeviceCreateInfo,
            vk::PhysicalDeviceFeatures2,
            vk::PhysicalDeviceVulkan11Features,
            vk::PhysicalDeviceVulkan12Features,
            vk::PhysicalDeviceVulkan13Features
        > chain{
            info,
            {},
            features11,
            features12,
            features13
        };

        vk::raii::Device handle = nullptr;

        try {
            // Create device
            handle = vk::raii::Device(physicalDevice, chain.get<vk::DeviceCreateInfo>());
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

    VmaAllocator Context::initializeAllocator(const vk::raii::Device& device, const vk::raii::PhysicalDevice& physicalDevice, const vk::raii::Instance& instance) {
        // Info
        VmaAllocatorCreateInfo info{
            .flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
            .physicalDevice = *physicalDevice,
            .device = *device,
            .instance = *instance
        };

        // Handle
        VmaAllocator allocator;

        if (vmaCreateAllocator(&info, &allocator) != VK_SUCCESS) {
            spdlog::error("Failed to create allocator");
            return nullptr;
        }

        return allocator;
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
        return false;
    }
}
