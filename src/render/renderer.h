#pragma once

#include "vku/vku.h"

namespace render {
    class Renderer {
        vku::VKU mVKU;
    public:
        bool initialize();
    };
}
