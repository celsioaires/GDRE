#pragma once

#include "backend/backend.h"
#include "backend/buffer.h"
#include "backend/vertex.h"

namespace render {
    class Mesh {
        glm::mat4 mTransformMatrix;
        uint32_t mVertexCount;
        backend::Buffer mVertexBuffer;
    public:
        bool initialize(const backend::Backend& backend, const std::vector<backend::Vertex>& vertices);

        void test();

        // Getters
        glm::mat4 getTransformMatrix() const { return mTransformMatrix; }
        uint32_t getVertexCount() const { return mVertexCount; }
        const backend::Buffer& getVertexBuffer() const { return mVertexBuffer; }

        // Setters
        void setTransformMatrix(const glm::mat4& matrix) { mTransformMatrix = matrix; }
    };
}