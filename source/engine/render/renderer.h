#pragma once

#include "../gui/window.h"

#include "backend/backend.h"

#include "camera.h"
#include "mesh.h"

namespace render {
    class Renderer {
        backend::Backend mBackend;
        std::vector<Mesh> mMeshes;
        Camera mCamera;
    public:
        bool initialize(const gui::Window& window);
        void cleanup();

        bool uploadMesh(const std::vector<backend::Vertex>& vertices);

        void update();

        void beginDraw();
        void endDraw();

        void drawMeshes();

        // Getters
        const backend::Backend& getBackend() const { return mBackend; };
    };
}
