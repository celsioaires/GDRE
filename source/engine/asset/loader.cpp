#include "loader.h"


namespace asset {
    bool Loader::initialize() {
        // Assets
        const std::string& directory = GDRE_ASSETS_DIRECTORY;

        if (!loadGltf(directory + "gltf/structure.glb")) {
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
