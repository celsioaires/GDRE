#pragma once

#include "backend/backend.h"

namespace render {
    struct CameraData {
        glm::mat4 viewMatrix;
        glm::mat4 projectionMatrix;
    };

    class Camera {
        // Objects
        const backend::Backend* mBackend;
        CameraData mData;
        backend::DescriptorSet mDescriptorSet;
        backend::Buffer mUniformBuffer;
    public:
        bool initialize(const backend::Backend& backend);

        void updateData();

        // Getters
        const backend::DescriptorSet& getDescriptorSet() const { return mDescriptorSet; }
    };
}
