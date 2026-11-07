#pragma once

#include "../asset/loader.h"
#include "../render/renderer.h"

namespace scene {
    class Scene {
    public:
        bool initialize(const asset::Loader& loader, render::Renderer& renderer);
    };
}
