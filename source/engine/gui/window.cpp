#include "window.h"

#include <spdlog/spdlog.h>

namespace gui {
    bool Window::initialize() {
        // Hints
        glfwWindowHint(GLFW_CLIENT_API, false); // Vulkan

        // Create window
        mHandle = glfwCreateWindow(mWidth, mHeight, "GDRE", nullptr, nullptr);

        if (!mHandle) {
            // Failed
            spdlog::error("Failed to create window");
            return false;
        }

        spdlog::info("Window created successfully");

        glfwSetWindowUserPointer(mHandle, this);

        glfwSetFramebufferSizeCallback(mHandle, framebufferSizeCallback);

        return true;
    }

    void Window::cleanup() {
        glfwDestroyWindow(mHandle);
    }

    bool Window::isOpen() {
        return !glfwWindowShouldClose(mHandle);
    }

    bool Window::isMinimized() {
        return glfwGetWindowAttrib(mHandle, GLFW_ICONIFIED);
    }

    void Window::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
        Window* pWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

        if (pWindow != nullptr) {
            pWindow->setWidth(width);
            pWindow->setHeight(height);
        }
    }
}
