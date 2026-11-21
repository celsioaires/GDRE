#pragma once

#include <glm/glm.hpp>

namespace backend {
    struct Vertex {
        glm::vec3 position;
        float _padding0;
        glm::vec3 normal;
        float _padding1;
        glm::vec4 tangent;
        glm::vec2 uv;
        glm::vec2 _padding2;
        glm::vec4 color;
    };
}
