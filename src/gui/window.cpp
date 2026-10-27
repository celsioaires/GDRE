#include "window.h"

#include <spdlog/spdlog.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace gui {
    bool Window::initialize() {
        // Hints
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Vulkan
        glfwWindowHint(GLFW_MAXIMIZED, true); // Maximized
        glfwWindowHint(GLFW_VISIBLE, false); // Hidden

        // Create GLFW window
        mHandle = glfwCreateWindow(800, 600, "GDRE", nullptr, nullptr);

        // ¿Failed?
        if (!mHandle) {
            spdlog::error("Failed to create window!");
            return false;
        }

        return true;
    }

    void Window::cleanup() {
        glfwDestroyWindow(mHandle);
    }

    void Window::show() {
        glfwShowWindow(mHandle);
    }

    bool Window::isOpen() {
        return !glfwWindowShouldClose(mHandle);
    }

    bool Window::isMinimized() {
        return glfwGetWindowAttrib(mHandle, GLFW_ICONIFIED);
    }
}
