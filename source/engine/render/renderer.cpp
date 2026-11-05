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
        mBackend.beginCommands();
    }

    void Renderer::endDraw() {
        mBackend.endCommands();
    }

    void Renderer::update() {
        mCamera.updateMatrices();
        mMesh.updateTransform();
    }

    void Renderer::draw() {
        mCamera.updateDescriptor();
        mBackend.render(mCamera.getDescriptorSet(), mMesh.getConstants());
    }
}
