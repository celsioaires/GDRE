#include "engine.h"

namespace src {
    bool Engine::initialize() {
        if (
            !mPlatform.initialize() ||
            !mWindow.initialize()
        ) {
            return false;
        }

        mGui.initialize();

        if (
            !mLoader.load() ||
            !mRenderer.initialize(mWindow)
        ) {
            return false;
        }

        return true;
    }

    void Engine::cleanup() {
        mGui.cleanup();
        mWindow.cleanup();
        mPlatform.cleanup();
    }

    void Engine::run() {
        mWindow.show();

        while (mWindow.isOpen()) {
            mPlatform.pollEvents();

            if (mWindow.isMinimized()) {
                continue;
            }
        }
    }
}
