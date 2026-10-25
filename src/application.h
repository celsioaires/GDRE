#pragma once

#include "engine.h"

namespace src {
    class Application {
        Engine mEngine;
    public:
        bool execute();
    };
}
