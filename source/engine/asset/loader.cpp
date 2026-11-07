#include "loader.h"


namespace asset {
    bool Loader::initialize() {
        if (!loadGltf(cAssetsDirectory + "gltf/default.glb")) {
            return false;
        }

        return true;
    }

    bool Loader::loadGltf(const std::string& path) {
        Gltf gltf;

        if (!gltf.load(path)) {
            return false;
        }

        mGltfAssets.push_back(std::move(gltf));

        return true;
    }
}
