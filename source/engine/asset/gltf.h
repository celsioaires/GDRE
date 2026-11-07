#pragma once

#include <filesystem>

#include <fastgltf/core.hpp>

#include "mesh.h"

namespace asset {
    class Gltf {
        fastgltf::Asset mAsset;
        std::string mName;
        std::vector<Mesh> mMeshes;
    public:
        bool load(const std::filesystem::path& path);

        // Getters
        const std::vector<Mesh>& getMeshes() const { return mMeshes; }
    };
}
