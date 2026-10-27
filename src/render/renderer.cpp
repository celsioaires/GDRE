#include "renderer.h"

namespace render {
    bool Renderer::initialize() {
        if (!mVKU.initialize()) {
            return false;
        }

        return true;
    }
}
