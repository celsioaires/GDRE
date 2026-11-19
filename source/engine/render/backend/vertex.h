#pragma once

#include <glm/glm.hpp>

namespace backend {
    struct Vertex {
        glm::vec3 position;
        float padding0;
        glm::vec3 color;
        float padding1;
        glm::vec2 uv;
        glm::vec2 padding2;
    };
}
