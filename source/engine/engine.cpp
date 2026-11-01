#include "engine.h"

namespace engine {
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
        mRenderer.cleanup();
        mGui.cleanup();
        mWindow.cleanup();
        mPlatform.cleanup();
    }

    void Engine::run() {
        while (mWindow.isOpen()) {
            mPlatform.pollEvents();

            if (mWindow.isMinimized()) {
                continue;
            }

            mRenderer.drawMesh();
        }
    }
}
