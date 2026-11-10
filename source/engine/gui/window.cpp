#include "window.h"

#include <spdlog/spdlog.h>

namespace gui {
    Window::~Window() {
        glfwDestroyWindow(mHandle);
    }

    bool Window::initialize() {
        // Hints
        glfwWindowHint(GLFW_CLIENT_API, false); // Vulkan

        // Create window
        mHandle = glfwCreateWindow(800, 600, "GDRE", nullptr, nullptr);

        if (!mHandle) {
            // Failed
            spdlog::error("Failed to create window");
            return false;
        }

        spdlog::info("Window created successfully");

        glfwSetKeyCallback(mHandle, keyCallback);

        return true;
    }

    bool Window::isOpen() const {
        return !glfwWindowShouldClose(mHandle);
    }

    bool Window::isMinimized() const {
        return glfwGetWindowAttrib(mHandle, GLFW_ICONIFIED);
    }

    void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
    }
}
