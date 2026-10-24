#include "loader.h"

#include <spdlog/spdlog.h>

#include <fastgltf/core.hpp>

namespace load {
    bool Loader::load() {
        fastgltf::Expected<fastgltf::GltfDataBuffer> data = fastgltf::GltfDataBuffer::FromPath("assets/gltf/meshes.glb");

        if (data.error() != fastgltf::Error::None) {
            spdlog::error("Failed to read glTF asset: {}!", fastgltf::getErrorMessage(data.error()));
            return false;
        }

        return true;
    }
}
