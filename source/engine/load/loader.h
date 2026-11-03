#pragma once
#include <string>

namespace load {
    class Loader {
        const std::string cAssetsDirectory = GDRE_ASSETS_DIRECTORY;
    public:
        bool load();
    };
}
