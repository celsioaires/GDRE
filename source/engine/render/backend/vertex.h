#pragma once

#include <glm/glm.hpp>

namespace backend {
    struct Vertex {
        glm::vec2 position;
        glm::vec2 padding;
        glm::vec3 color;
    };
}
