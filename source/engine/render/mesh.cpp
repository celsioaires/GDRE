#include "mesh.h"

#include "glm/ext/matrix_transform.hpp"

namespace render {
    bool Mesh::initialize(const backend::Backend& backend, const std::vector<backend::Vertex>& vertices, const std::vector<uint32_t>& indices) {
        mTransformMatrix = glm::mat4(1);

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

    void Mesh::updateTransform() {
        mTransformMatrix = glm::rotate(glm::mat4(1), static_cast<float>(glfwGetTime()), { 0, 1, 0 });
    }
}
