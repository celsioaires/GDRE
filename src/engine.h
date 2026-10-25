#pragma once

#include "core/platform.h"
#include "gui/window.h"
#include "gui/gui.h"
#include "load/loader.h"
#include "render/renderer.h"

namespace src {
    class Engine {
        core::Platform mPlatform;
        gui::Window mWindow;
        gui::Gui mGui;
        load::Loader mLoader;
        render::Renderer mRenderer;
    public:
        bool initialize();
        void cleanup();

        void run();
    };
}
