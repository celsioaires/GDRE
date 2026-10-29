#include "application.h"

#include <spdlog/spdlog.h>

namespace src {
    bool Application::execute() {
        spdlog::set_pattern("[%H:%M:%S] [%l] %v");

        // Initialization
        spdlog::info("Initializing GDRE...");

        if (!mEngine.initialize()) {
            return false;
        }

        // Running
        spdlog::info("Running GDRE...");

        mEngine.run();

        // Shutdown
        spdlog::info("Shutting down GDRE...");

        mEngine.cleanup();

        return true;
    }
}
