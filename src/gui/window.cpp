#include "window.h"

#include <spdlog/spdlog.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace gui {
    bool Window::initialize() {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_MAXIMIZED, true);
        glfwWindowHint(GLFW_VISIBLE, false);

        mHandle = glfwCreateWindow(1, 1, "GDRE", nullptr, nullptr);

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
