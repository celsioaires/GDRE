#include <spdlog/spdlog.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

int main() {
    spdlog::info("Hello, GDRE!");

    // glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);

    if (!glfwInit()) {
        spdlog::error("Failed to initialize GLFW!");
        return 1;
    }

    // Hints
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Vulkan
    glfwWindowHint(GLFW_MAXIMIZED, true); // Maximized
    glfwWindowHint(GLFW_VISIBLE, false); // Hidden

    int width{1};
    int height{1};

    GLFWwindow* window = glfwCreateWindow(width, height, "GDRE", nullptr, nullptr);

    if (!window) {
        spdlog::error("Failed to create GLFW window!");
        return 1;
    }

    glm::mat4 matrix(1);

    glfwShowWindow(window);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED)) {
            continue;
        }

        // spdlog::info("Matrix: {}", glm::to_string(matrix));

        // glfwGetWindowSize(window, &width, &height);

        // spdlog::info("Size: {}x{}", width, height);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
