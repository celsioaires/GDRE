#pragma once

namespace core {
    class Platform {
    public:
        ~Platform();

        bool initialize();

        void pollEvents();
    };
}
