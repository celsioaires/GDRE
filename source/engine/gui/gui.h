#pragma once

#include "../render/renderer.h"

namespace gui {
    class Gui {
        const backend::Backend* mBackend = nullptr;
    public:
        bool initialize(const render::Renderer& renderer);
        void cleanup();

        void drawPanels();
        void renderPanels();
    };
}
