#pragma once

#include "window.h"

namespace core {
    class Platform {
        Window mWindow;
    public:
        ~Platform();

        bool initialize();

        void pollEvents();

        // Getters
        const Window& getWindow() const { return mWindow; }
    };
}
