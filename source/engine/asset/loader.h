#pragma once

#include "gltf.h"

namespace asset {
    class Loader {
        std::vector<Gltf> mAssets;
    public:
        bool initialize();

        bool loadGltf(const std::string& path);

        // Getter
        const std::vector<Gltf>& getAssets() const { return mAssets; };
    };
}
