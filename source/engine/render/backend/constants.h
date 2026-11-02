#pragma once

#include <vulkan/vulkan.hpp>

#include <glm/glm.hpp>

namespace backend {
    struct ConstantsData {
        vk::DeviceAddress vertexBuffer;
        glm::vec2 padding;
        glm::mat4 transformMatrix;
    };

    struct Constants {
        ConstantsData data;
        vk::ShaderStageFlags stage;
    };
}
