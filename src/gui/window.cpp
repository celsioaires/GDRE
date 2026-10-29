#include "window.h"

#include <spdlog/spdlog.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace gui {
    bool Window::initialize() {
        // Hints
        glfwWindowHint(GLFW_CLIENT_API, false); // Vulkan
        glfwWindowHint(GLFW_RESIZABLE, false); // No resizable

        // Create window
        mHandle = glfwCreateWindow(mWidth, mHeight, "GDRE", nullptr, nullptr);

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
