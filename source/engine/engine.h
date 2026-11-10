#pragma once

#include "core/platform.h"
#include "gui/gui.h"
#include "asset/loader.h"
#include "render/renderer.h"
#include "scene/scene.h"

namespace engine {
    class Engine {
        core::Platform mPlatform;
        asset::Loader mLoader;
        render::Renderer mRenderer;
        scene::Scene mScene;
        core::Gui mGui;
    public:
        bool initialize();
        void shutdown();

        void run();
    private:
        void update();
        void draw();
    };
}
