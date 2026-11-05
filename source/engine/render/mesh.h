#pragma once

#include "backend/backend.h"
#include "backend/buffer.h"

namespace render {
    class Mesh {
        backend::Buffer mVertexBuffer;
        backend::MeshConstants mConstants;
    public:
        bool initialize(const backend::Backend& backend);

        void updateTransform();

        // Getters
        const backend::MeshConstants& getConstants() const { return mConstants; }
    };
}