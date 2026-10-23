#include <spdlog/spdlog.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

int main() {
    spdlog::info("Hello, GDRE!");

    if (!glfwInit()) {
        spdlog::error("Failed to initialize GLFW!");
    }

    glfwTerminate();

    return 0;
}
