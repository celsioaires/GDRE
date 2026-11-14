#include "mesh.h"

#include "glm/ext/matrix_transform.hpp"

namespace render {
    bool Mesh::initialize(const backend::Backend& backend, const glm::mat4& transformMatrix, const std::vector<backend::Vertex>& vertices, const std::vector<uint32_t>& indices) {
        mTransformMatrix = transformMatrix;
        mVertexCount = vertices.size();
        mIndexCount = static_cast<uint32_t>(indices.size());

        // Vertex buffer
        if (!mVertexBuffer.initialize(
            backend.getContext(),
            backend::eBufferTypeVertex,
            vertices.size() * sizeof(backend::Vertex),
            vertices.data()
        )) {
            return false;
        }

        // Index buffer
        if (!mIndexBuffer.initialize(
            backend.getContext(),
            backend::eBufferTypeIndex,
            indices.size() * sizeof(uint32_t),
            indices.data()
        )) {
            return false;
        }

        return true;
    }
}
