#include "renderer.h"

namespace render {
    bool Renderer::initialize(const gui::Window& window) {
        // Backend
        if (!mBackend.initialize(window.getHandle())) {
            return false;
        }

        // Camera
        if (!mCamera.initialize(mBackend)) {
            return false;
        }

        return true;
    }

    void Renderer::cleanup() {
        mBackend.cleanup();
    }

    void Renderer::uploadMesh(Mesh&& mesh) {
        mMeshes.push_back(std::move(mesh));
    }

    void Renderer::update() {
        // Previous frame
        mBackend.waitCommands();

        for (Mesh& mesh : mMeshes) {
            mesh.test();
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

    void Renderer::drawMeshes() {
        mBackend.beginRender(mCamera.getDescriptorSet());

        for (Mesh& mesh : mMeshes) {
            mBackend.drawVertices(
                {
                    .vertexBuffer = mesh.getVertexBuffer().getAddress(),
                    .transformMatrix = mesh.getTransformMatrix()
                },
                mesh.getVertexCount()
            );
        }

        mBackend.endRender();
    }
}
