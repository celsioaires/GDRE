#include <spdlog/spdlog.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

int main() {
    spdlog::info("Hello, GDRE!");

    if (!glfwInit()) {
        spdlog::error("Failed to initialize GLFW!");
        return 1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWwindow* window = glfwCreateWindow(800, 600, "GDRE", nullptr, nullptr);

    if (!window) {
        spdlog::error("Failed to create GLFW window!");
        return 1;
    }

    // Engine loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}
