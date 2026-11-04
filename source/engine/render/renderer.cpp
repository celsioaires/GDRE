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

        // Mesh
        if (!mMesh.initialize(mBackend)) {
            return false;
        }

        return true;
    }

    void Renderer::cleanup() {
        mBackend.cleanup();
    }

    void Renderer::beginDraw() {
        mBackend.beginRender();
    }

    void Renderer::endDraw() {
        mBackend.endRender();
    }

    void Renderer::drawTest() {
        mBackend.renderMesh(mMesh.getConstants());
    }
}
