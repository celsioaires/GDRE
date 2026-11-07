#pragma once

#include "gltf.h"

namespace asset {
    class Loader {
        const std::string cAssetsDirectory = GDRE_ASSETS_DIRECTORY;
        std::vector<Gltf> mGltfAssets;
    public:
        bool initialize();

        bool loadGltf(const std::string& path);

        // Getter
        const std::vector<Gltf>& getGltfAssets() const { return mGltfAssets; };
    };
}
