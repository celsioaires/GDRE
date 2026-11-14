#pragma once

#include "backend/backend.h"
#include "backend/buffer.h"
#include "backend/vertex.h"

namespace render {
    class Mesh {
        glm::mat4 mTransformMatrix;
        uint32_t mVertexCount;
        uint32_t mIndexCount;
        backend::Buffer mVertexBuffer;
        backend::Buffer mIndexBuffer;
    public:
        bool initialize(const backend::Backend& backend, const glm::mat4& transformMatrix, const std::vector<backend::Vertex>& vertices, const std::vector<uint32_t>& indices);

        // Getters
        glm::mat4 getTransformMatrix() const { return mTransformMatrix; }
        uint32_t getVertexCount() const { return mVertexCount; }
        uint32_t getIndexCount() const { return mIndexCount; }
        const backend::Buffer& getVertexBuffer() const { return mVertexBuffer; }
        const backend::Buffer& getIndexBuffer() const { return mIndexBuffer; }

        // Setters
        void setTransformMatrix(const glm::mat4& matrix) { mTransformMatrix = matrix; }
    };
}