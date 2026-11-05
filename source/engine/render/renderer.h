#pragma once

#include "../gui/window.h"

#include "backend/backend.h"

#include "camera.h"
#include "mesh.h"

namespace render {
    class Renderer {
        backend::Backend mBackend;
        Camera mCamera;
        Mesh mMesh;
    public:
        bool initialize(const gui::Window& window);
        void cleanup();

        void beginDraw();
        void endDraw();

        void update();
        void draw();

        // Getters
        const backend::Backend& getBackend() const { return mBackend; };
    };
}
