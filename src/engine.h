#pragma once

#include "core/platform.h"
#include "gui/window.h"
#include "gui/gui.h"

namespace src {
    class Engine {
        core::Platform mPlatform;
        gui::Window mWindow;
        gui::Gui mGui;
    public:
        bool initialize();
        void cleanup();

        void run();
    };
}
