#include "engine.h"

namespace engine {
    bool Engine::initialize() {
        if (
            !mPlatform.initialize() ||
            !mLoader.initialize() ||
            !mRenderer.initialize(mPlatform) ||
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
        const core::Window& window = mPlatform.getWindow();

        window.maximize();

        while (window.isOpen()) {
            mPlatform.pollEvents();

            if (window.isMinimized()) {
                continue;
            }

            update();
            draw();
        }
    }

    void Engine::update() {
        mGui.prepareWindows();
        mRenderer.updateScene();
    }

    void Engine::draw() {
        mRenderer.beginDraw();

        mRenderer.drawScene();
        mGui.drawWindows();

        mRenderer.endDraw();
    }
}
