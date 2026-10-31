#pragma once
#include <string>

namespace load {
    class Loader {
    public:
        bool load();
    private:
        static inline const std::string C_ASSETS_DIRECTORY = GDRE_ASSETS_DIRECTORY;
    };
}
