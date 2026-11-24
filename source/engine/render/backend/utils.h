#pragma once

#include <GLFW/glfw3.h>

#include <vulkan/vulkan.hpp>

#include <glm/glm.hpp>

namespace backend {
    struct BackendInfo {
        GLFWwindow* window;
    };

    struct PushConstantsData {
        vk::DeviceAddress vertexBuffer;
        glm::vec2 padding;
        glm::mat4 transformMatrix;
    };

    struct PushConstants {
        PushConstantsData data;
        vk::ShaderStageFlags stage;
    };
}
