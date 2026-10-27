#include "renderer.h"

namespace render {
    bool Renderer::initialize(const gui::Window& window) {
        if (!mVKU.initialize(window.getHandle())) {
            return false;
        }

        return true;
    }
}
