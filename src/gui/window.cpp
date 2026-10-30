#include "window.h"

#include <spdlog/spdlog.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace gui {
    bool Window::initialize() {
        // Hints
        glfwWindowHint(GLFW_CLIENT_API, false); // Vulkan

        // Create window
        mHandle = glfwCreateWindow(mWidth, mHeight, "GDRE", glfwGetPrimaryMonitor(), nullptr);

        if (!mHandle) {
            // Failed
            spdlog::error("Failed to create window");
            return false;
        }

        spdlog::info("Window created successfully");

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
}
