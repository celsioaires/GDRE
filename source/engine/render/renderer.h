#pragma once

#include "../core/platform.h"

#include "backend/backend.h"

#include "camera.h"
#include "mesh.h"

namespace render {
    class Renderer {
        // Objects
        const core::Platform* mPlatform;
        backend::Backend mBackend;
        std::vector<Mesh> mMeshes;
        Camera mCamera;
    public:
        bool initialize(const core::Platform& platform);

        bool uploadMesh(const std::vector<backend::Vertex>& vertices, const std::vector<uint32_t>& indices);

        void updateScene();

        void beginDraw();
        void endDraw();

        void drawScene() const;

        void waitBackend() const;

        // Getters
        const backend::Backend& getBackend() const { return mBackend; };
    };
}
