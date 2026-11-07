#include "mesh.h"

#include "glm/ext/matrix_transform.hpp"

namespace render {
    bool Mesh::initialize(const backend::Backend& backend, const std::vector<backend::Vertex>& vertices) {
        mTransformMatrix = glm::mat4(1);
        mVertexCount = static_cast<uint32_t>(vertices.size());

        // Vertex buffer
        if (!mVertexBuffer.initialize(
            backend.getContext(),
            backend::eBufferTypeVertex,
            vertices.size() * sizeof(backend::Vertex),
            vertices.data()
        )) {
            return false;
        }

        return true;
    }

    void Mesh::test() {
        mTransformMatrix = glm::rotate(glm::mat4(1), static_cast<float>(glfwGetTime()), glm::vec3(1, 1, 1));
    }
}
