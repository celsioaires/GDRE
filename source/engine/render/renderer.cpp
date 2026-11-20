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

    const Mesh* Renderer::uploadMesh(const std::vector<backend::Vertex>& vertices, const std::vector<uint32_t>& indices) {
        // Upload
        mMeshes.emplace_back();

        // Initialize
        Mesh& mesh = mMeshes.back();

        if (!mesh.initialize(mBackend, vertices, indices)) {
            mMeshes.pop_back();
            return nullptr;
        }

        return &mesh;
    }

    const Material* Renderer::uploadMaterial(const MaterialData& data, const TextureData& colorTextureData, const TextureData& metalRoughTextureData) {
        // Upload
        mMaterials.emplace_back();

        // Initialize
        Material& material = mMaterials.back();

        if (!mMaterials.back().initialize(mBackend, data, colorTextureData, metalRoughTextureData)) {
            mMaterials.pop_back();
            return nullptr;
        }

        return &material;
    }

    void Renderer::uploadRenderable(const Mesh* mesh, const Material* material, const glm::mat4& transform) {
        mRenderables.emplace_back(mesh, material, transform);
    }

    void Renderer::updateScene() {
        // Previous frame
        mBackend.waitCommands();

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

        for (const Renderable& renderable : mRenderables) {
            const Mesh* mesh = renderable.getMesh();
            const Material* material = renderable.getMaterial();

            mBackend.drawIndexedVertices(
                {
                    .vertexBuffer = mesh->getVertexBuffer().getAddress(),
                    .transformMatrix = renderable.getTransform()
                },
                material->getDescriptorSet(),
                mesh->getIndexBuffer(),
                mesh->getIndexCount()
            );
        }

        mBackend.endRender();
    }

    void Renderer::waitBackend() const {
        mBackend.waitContext();
    }
}
