#include <spdlog/spdlog.h>

#include "engine.h"

namespace src {
    static Engine sEngine;

    bool execute() {
        spdlog::info("Initializing GDRE...");

        Engine& e = sEngine;

        if (!e.initialize()) {
            return false;
        }

        spdlog::info("Running GDRE...");

        e.run();

        spdlog::info("Shutting down GDRE...");

        e.cleanup();

        return true;
    }
}

int main() {
    if (!src::execute()) {
        return 1;
    }

    return 0;
}
