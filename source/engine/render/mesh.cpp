#include "mesh.h"

namespace render {
    bool Mesh::initialize(const backend::Backend& backend) {
        // Vertices
        const std::vector<backend::Vertex> triangleVertices{
            { .position{ 0.0, -0.5 }, .color{ 1, 0, 0 } },
            { .position{ 0.5, 0.5 }, .color{ 0, 1, 0 } },
            { .position{ -0.5, 0.5 }, .color{ 0, 0, 1 } }
        };

        // Vertex buffer
        if (!mVertexBuffer.initialize(
            backend.getContext(),
            backend::eBufferTypeVertex,
            triangleVertices.size() * sizeof(backend::Vertex),
            triangleVertices.data()
        )) {
            return false;
        }

        // Constants
        mConstants = {
            .vertexBuffer = mVertexBuffer.getAddress(),
            .transformMatrix = glm::mat4(1),
        };

        return true;
    }
}
