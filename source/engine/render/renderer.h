#pragma once

#include <deque>

#include "../core/platform.h"

#include "backend/backend.h"

#include "mesh.h"
#include "material.h"
#include "camera.h"
#include "renderable.h"

namespace render {
    class Renderer {
        const core::Platform* mPlatform = nullptr;
        backend::Backend mBackend;

        // Data
        std::deque<Mesh> mMeshes;
        std::deque<Material> mMaterials;
        Camera mCamera;
        std::vector<Renderable> mRenderables;
    public:
        bool initialize(const core::Platform& platform);

        const Mesh* uploadMesh(const std::vector<backend::Vertex>& vertices, const std::vector<uint32_t>& indices);
        const Material* uploadMaterial(const TextureData& colorTexture);
        void uploadRenderable(const Mesh* mesh, const Material* material, const glm::mat4& transform);

        void updateScene();

        void beginDraw();
        void endDraw();

        void drawScene() const;

        void waitBackend() const;

        // Getters
        const backend::Backend& getBackend() const { return mBackend; };
    };
}
