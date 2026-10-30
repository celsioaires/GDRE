#pragma once

#include "engine/engine.h"

namespace src {
    class Application {
        engine::Engine mEngine;
    public:
        bool execute();
    };
}
