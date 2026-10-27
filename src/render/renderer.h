#pragma once

#include "../gui/window.h"

#include "vku/vku.h"

namespace render {
    class Renderer {
        vku::VKU mVKU;
    public:
        bool initialize(const gui::Window& window);
    };
}
