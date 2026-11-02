#include "renderer.h"

namespace render {
    bool Renderer::initialize(const gui::Window& window) {
        const backend::BackendInfo info{
            .window = window.getHandle(),
            .width = static_cast<uint32_t>(window.getWidth()),
            .height = static_cast<uint32_t>(window.getHeight()),
        };

        if (!mBackend.initialize(info)) {
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

    void Renderer::drawMesh() {
        mBackend.renderCube();
    }
}
