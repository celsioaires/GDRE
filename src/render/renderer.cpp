#include "renderer.h"

namespace render {
    bool Renderer::initialize(const gui::Window& window) {
        backend::BackendInfo wsi{
            .window = window.getHandle(),
            .width = static_cast<uint32_t>(window.getWidth()),
            .height = static_cast<uint32_t>(window.getHeight()),
        };

        if (!mBackend.initialize(wsi)) {
            return false;
        }

        return true;
    }

    void Renderer::cleanup() {
        mBackend.cleanup();
    }

    void Renderer::test() {
        mBackend.test();
    }
}
