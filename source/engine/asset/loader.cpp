#include "loader.h"


namespace asset {
    bool Loader::initialize() {
        // Assets
        if (!loadGltf(cAssetsDirectory + "gltf/structure.glb")) {
            return false;
        }

        return true;
    }

    bool Loader::loadGltf(const std::string& path) {
        Gltf gltf;

        if (!gltf.load(path)) {
            return false;
        }

        mAssets.push_back(std::move(gltf));

        return true;
    }
}
