#include "application.h"

#include <spdlog/spdlog.h>

namespace src {
    bool Application::execute() {
        spdlog::info("Initializing GDRE...");

        if (!mEngine.initialize()) {
            return false;
        }

        spdlog::info("Running GDRE...");

        mEngine.run();

        spdlog::info("Shutting down GDRE...");

        mEngine.cleanup();

        return true;
    }
}
