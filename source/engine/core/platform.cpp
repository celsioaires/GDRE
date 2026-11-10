#include "platform.h"

#include <spdlog/spdlog.h>

#include <GLFW/glfw3.h>

namespace core {
    Platform::~Platform() {
        glfwTerminate();
    }

    bool Platform::initialize() {
        // Hints
        glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11); // X11

        // GLFW initialization
        if (!glfwInit()) {
            spdlog::error("Failed to initialize GLFW");
            return false;
        }

        spdlog::info("GLFW initialized successfully");

        return true;
    }

    void Platform::pollEvents() {
        glfwPollEvents();
    }
}
