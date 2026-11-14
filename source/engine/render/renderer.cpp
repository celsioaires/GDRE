#include "renderer.h"

#include "spdlog/spdlog.h"

namespace render {
    bool Renderer::initialize(const core::Platform& platform) {
        mPlatform = &platform;

        // Backend
        if (!mBackend.initialize(mPlatform->getWindow().getHandle())) {
            return false;
        }

        // Meshes
        mMeshes.reserve(1000);

        // Camera
        if (!mCamera.initialize(mBackend)) {
            return false;
        }

        return true;
    }

    bool Renderer::uploadMesh(const glm::mat4& transformMatrix, const std::vector<backend::Vertex>& vertices, const std::vector<uint32_t>& indices) {
        spdlog::info(
        "Upload position: {}, {}, {}",
        transformMatrix[3].x,
        transformMatrix[3].y,
        transformMatrix[3].z
    );


        // Upload
        mMeshes.emplace_back();

        // Initialize
        Mesh& mesh = mMeshes.back();

        if (!mesh.initialize(mBackend, transformMatrix, vertices, indices)) {
            mMeshes.pop_back();
            return false;
        }

        return true;
    }

    bool Renderer::uploadMaterial() {
        // Upload
        mMaterials.emplace_back();

        // Initialize
        if (!mMaterials.back().initialize(mBackend)) {
            mMaterials.pop_back();
            return false;
        }

        return true;
    }

    void Renderer::updateScene() {
        // Previous frame
        mBackend.waitCommands();

        // Meshes
        for (Mesh& mesh : mMeshes) {
            
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
