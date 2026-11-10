#pragma once

#include "core/platform.h"
#include "gui/window.h"
#include "gui/gui.h"
#include "asset/loader.h"
#include "render/renderer.h"
#include "scene/scene.h"

namespace engine {
    class Engine {
        core::Platform mPlatform;
        gui::Window mWindow;
        asset::Loader mLoader;
        render::Renderer mRenderer;
        scene::Scene mScene;
        gui::Gui mGui;
    public:
        bool initialize();
        void shutdown();

        void run();
    private:
        void update();
        void draw();
    };
}
