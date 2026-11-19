#pragma once

#include "backend/backend.h"
#include "backend/buffer.h"
#include "backend/vertex.h"

namespace render {
    class Mesh {
        uint32_t mVertexCount = 0;
        uint32_t mIndexCount = 0;
        backend::Buffer mVertexBuffer;
        backend::Buffer mIndexBuffer;
    public:
        bool initialize(const backend::Backend& backend, const std::vector<backend::Vertex>& vertices, const std::vector<uint32_t>& indices);

        // Getters
        uint32_t getVertexCount() const { return mVertexCount; }
        uint32_t getIndexCount() const { return mIndexCount; }
        const backend::Buffer& getVertexBuffer() const { return mVertexBuffer; }
        const backend::Buffer& getIndexBuffer() const { return mIndexBuffer; }
    };
}
