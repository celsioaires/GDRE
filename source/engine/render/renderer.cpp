#include "renderer.h"

namespace render {
    bool Renderer::initialize(const core::Platform& platform) {
        mPlatform = &platform;

        // Backend
        if (!mBackend.initialize(mPlatform->getWindow().getHandle())) {
            return false;
        }

        // Camera
        if (!mCamera.initialize(mBackend)) {
            return false;
        }

        return true;
    }

    bool Renderer::uploadMesh(const std::vector<backend::Vertex>& vertices, const std::vector<uint32_t>& indices) {
        Mesh mesh;

        if (!mesh.initialize(mBackend, vertices, indices)) {
            return false;
        }

        mMeshes.push_back(std::move(mesh));

        return true;
    }

    void Renderer::updateScene() {
        // Previous frame
        mBackend.waitCommands();

        // Meshes
        for (Mesh& mesh : mMeshes) {
            mesh.updateTransform();
        }

        // Camera
        mCamera.updateData();
    }

    void Renderer::beginDraw() {
        mBackend.beginCommands();
    }

    void Renderer::endDraw() {
        mBackend.endCommands();
    }

    void Renderer::drawScene() const {
        mBackend.beginRender(mCamera.getDescriptorSet());

        for (const Mesh& mesh : mMeshes) {
            mBackend.drawIndexedVertices(
                {
                    .vertexBuffer = mesh.getVertexBuffer().getAddress(),
                    .transformMatrix = mesh.getTransformMatrix()
                },
                mesh.getIndexBuffer(),
                mesh.getIndexCount()
            );
        }

        mBackend.endRender();
    }

    void Renderer::waitBackend() const {
        mBackend.waitContext();
    }
}
