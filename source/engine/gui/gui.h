#pragma once

#include "../render/renderer.h"

#include "hierarchy.h"
#include "inspector.h"

namespace gui {
    class Gui {
        const backend::Backend* mBackend = nullptr;
        Hierarchy mHierarchy;
        Inspector mInspector;
    public:
        bool initialize(const render::Renderer& renderer);
        void cleanup();

        void prepareWindows() const;
        void drawWindows() const;
    };
}
