#pragma once

#include <GLFW/glfw3.h>

#include <vulkan/vulkan.hpp>

#include <glm/glm.hpp>

namespace backend {
    // Structs
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

    // Enums
    enum PipelineType {
        ePipelineTypeGraphics,
        ePipelineTypeCompute
    };

    enum BufferType {
        eBufferTypeStaging,
        eBufferTypeVertex,
        eBufferTypeIndex,
        eBufferTypeUniform,
    };
}
