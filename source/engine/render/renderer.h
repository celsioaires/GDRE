#pragma once

#include "../core/platform.h"

#include "backend/backend.h"

#include "mesh.h"
#include "material.h"
#include "camera.h"

namespace render {
    class Renderer {
        // Objects
        const core::Platform* mPlatform;
        backend::Backend mBackend;
        std::vector<Mesh> mMeshes;
        std::vector<Material> mMaterials;
        Camera mCamera;
    public:
        bool initialize(const core::Platform& platform);

        bool uploadMesh(const glm::mat4& transformMatrix, const std::vector<backend::Vertex>& vertices, const std::vector<uint32_t>& indices);
        bool uploadMaterial();

        void updateScene();

        void beginDraw();
        void endDraw();

        void drawScene() const;

        void waitBackend() const;

        // Getters
        const backend::Backend& getBackend() const { return mBackend; };
    };
}
