#pragma once

#include "backend/backend.h"
#include "backend/buffer.h"

namespace render {
    struct CameraData {
        glm::mat4 viewMatrix;
        glm::mat4 projectionMatrix;
    };

    class Camera {
        const backend::Backend* mBackend;
        CameraData mData;
        backend::DescriptorSet mDescriptorSet;
        backend::Buffer mUniformBuffer;
    public:
        bool initialize(const backend::Backend& backend);

        void updateData();

        const backend::DescriptorSet& getDescriptorSet() const { return mDescriptorSet; }
    };
}
