#include "gltf.h"

#include <spdlog/spdlog.h>

namespace asset {
    bool Gltf::load(const std::filesystem::path& path) {
        // File
        if (!std::filesystem::exists(path)) {
            spdlog::warn("Failed to find glTF file: {}", path.string());
            return false;
        }

        fastgltf::Expected<fastgltf::MappedGltfFile> file = fastgltf::MappedGltfFile::FromPath(path);

        if (!file) {
            spdlog::error("Failed to open glTF file: {}", fastgltf::getErrorMessage(file.error()));
            return false;
        }

        // Options
        const fastgltf::Options options =
            fastgltf::Options::AllowDouble |
            fastgltf::Options::LoadExternalBuffers |
            fastgltf::Options::LoadExternalImages |
            fastgltf::Options::GenerateMeshIndices;

        // Parser
        fastgltf::Parser parser;
        fastgltf::Expected<fastgltf::Asset> asset = parser.loadGltf(file.get(), path.parent_path(), options);

        if (!asset) {
            spdlog::error("Failed to load glTF: {}", fastgltf::getErrorMessage(asset.error()));
            return false;
        }

        // Asset
        mAsset = std::move(asset.get());

        // Meshes
        mMeshes.resize(mAsset.meshes.size());

        int i = 0;

        for (Mesh& mesh : mMeshes) {
            mesh.load(mAsset, mAsset.meshes[i]);
            i++;
        }

        // Materials

        return true;
    }
}
