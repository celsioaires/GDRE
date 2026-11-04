#pragma once

#include <GLFW/glfw3.h>

#include <vulkan/vulkan.hpp>

#include <glm/glm.hpp>

namespace backend {
    // Structs
    struct BackendInfo {
        GLFWwindow* window;
    };

    struct Vertex {
        glm::vec2 position;
        glm::vec2 padding;
        glm::vec3 color;
    };

    struct MeshConstants {
        vk::DeviceAddress vertexBuffer;
        glm::vec2 padding;
        glm::mat4 transformMatrix;
    };

    static inline const vk::PushConstantRange cMeshConstants{
        .stageFlags = vk::ShaderStageFlagBits::eVertex,
        .size = sizeof(MeshConstants)
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
