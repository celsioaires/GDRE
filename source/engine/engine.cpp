#include "engine.h"

namespace engine {
    bool Engine::initialize() {
        if (
            !mPlatform.initialize() ||
            !mWindow.initialize() ||
            !mRenderer.initialize(mWindow) ||
            !mGui.initialize(mRenderer) ||
            !mLoader.load()
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

            update();
            draw();
        }
    }

    void Engine::update() {
        mGui.prepareWindows();
    }

    void Engine::draw() {
        mRenderer.beginDraw();

        mRenderer.drawMesh();
        mGui.drawWindows();

        mRenderer.endDraw();
    }
}
