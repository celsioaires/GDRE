#pragma once

#include <glm/glm.hpp>

namespace backend {
    struct Vertex {
        glm::vec3 position;
        float padding0;
        glm::vec3 color;
        float padding1;
    };
}
