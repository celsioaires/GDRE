#pragma once

#include <GLFW/glfw3.h>

#if defined (USE_CPP20_MODULES)
    import vulkan_hpp;
#else
#include <vulkan/vulkan_raii.hpp>
#endif

#include <vma/vk_mem_alloc.h>

namespace backend {
    class Context {
        // Objects
        GLFWwindow* mWindow;
        vk::raii::Context mHandle;
        vk::raii::Instance mInstance = nullptr;
        vk::raii::DebugUtilsMessengerEXT mDebugMessenger = nullptr;
        vk::raii::SurfaceKHR mSurface = nullptr;
        vk::raii::PhysicalDevice mPhysicalDevice = nullptr;
        uint32_t mGraphicsFamily = 0;
        vk::raii::Device mDevice = nullptr;
        vk::raii::Queue mGraphicsQueue = nullptr;
        VmaAllocator mAllocator;
        vk::raii::Fence mSubmitFence = nullptr;
        vk::raii::Semaphore mAcquireSemaphore = nullptr;
        vk::raii::Semaphore mSubmitSemaphore = nullptr;
    public:
        static inline const uint32_t cApiVersion = vk::ApiVersion14;

        ~Context();

        bool initialize(GLFWwindow* window);

        void submitQueue(const vk::raii::CommandBuffer& commandBuffer) const;
        bool presentQueue(const vk::raii::SwapchainKHR& swapchain, uint32_t imageIndex) const;

        void waitIdle() const;

        // Getters
        GLFWwindow* getWindow() const { return mWindow; }
        const vk::raii::Instance& getInstance() const { return mInstance; }
        const vk::raii::SurfaceKHR& getSurface() const { return mSurface; }
        const vk::raii::PhysicalDevice& getPhysicalDevice() const { return mPhysicalDevice; }
        uint32_t getGraphicsFamily() const { return mGraphicsFamily; }
        const vk::raii::Device& getDevice() const { return mDevice; }
        const vk::raii::Queue& getGraphicsQueue() const { return mGraphicsQueue; }
        const VmaAllocator getAllocator() const { return mAllocator; }
        const vk::raii::Fence& getSubmitFence() const { return mSubmitFence; }
        const vk::raii::Semaphore& getAcquireSemaphore() const { return mAcquireSemaphore; }
        const vk::raii::Semaphore& getSubmitSemaphore() const { return mSubmitSemaphore; }
    private:
        // Initializers
        bool initializeInstance();
        bool initializeDebugMessenger();
        bool initializeSurface();
        bool initializePhysicalDevice();
        bool initializeDevice();
        bool initializeAllocator();
        bool initializeFence();
        bool initializeSemaphore(vk::raii::Semaphore& semaphore);


        // Callback
        static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(
            vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
            vk::DebugUtilsMessageTypeFlagsEXT type,
            const vk::DebugUtilsMessengerCallbackDataEXT* callbackData,
            void* userData
        );
    };
}
