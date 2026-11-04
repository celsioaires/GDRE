#pragma once

#include "backend/backend.h"
#include "backend/buffer.h"

namespace render {
    struct CameraData {
        glm::mat4 viewMatrix;
        glm::mat4 projectionMatrix;
    };

    class Camera {
        CameraData mData;
        backend::Buffer mUniformBuffer;
    public:
        bool initialize(const backend::Backend& backend);
    };
}
