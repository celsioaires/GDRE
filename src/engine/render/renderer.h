#pragma once

#include "../gui/window.h"

#include "backend/backend.h"

namespace render {
    class Renderer {
        backend::Backend mBackend;
    public:
        bool initialize(const gui::Window& window);
        void cleanup();

        void drawMesh();
    };
}
