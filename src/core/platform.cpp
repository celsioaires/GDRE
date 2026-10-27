#include "platform.h"

#include <spdlog/spdlog.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace core {
    bool Platform::initialize() {
        // Hints
        glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11); // X11

        // GLFW initialization
        if (!glfwInit()) {
            spdlog::error("Failed to initialize GLFW!");
            return false;
        }

        return true;
    }

    void Platform::cleanup() {
        glfwTerminate();
    }

    void Platform::pollEvents() {
        glfwPollEvents();
    }
}
