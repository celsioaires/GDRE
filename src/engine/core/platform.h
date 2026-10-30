#pragma once

namespace core {
    class Platform {
    public:
        bool initialize();
        void cleanup();

        void pollEvents();
    };
}
