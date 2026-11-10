#include "engine.h"

namespace engine {
    bool Engine::initialize() {
        if (
            !mPlatform.initialize() ||
            !mWindow.initialize() ||
            !mLoader.initialize() ||
            !mRenderer.initialize(mWindow) ||
            !mScene.initialize(mLoader, mRenderer) ||
            !mGui.initialize(mRenderer)
        ) {
            return false;
        }

        return true;
    }

    void Engine::shutdown() {
        mRenderer.waitBackend();
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

        mRenderer.update();
    }

    void Engine::draw() {
        mRenderer.beginDraw();

        mRenderer.drawMeshes();
        mGui.drawWindows();

        mRenderer.endDraw();
    }
}
