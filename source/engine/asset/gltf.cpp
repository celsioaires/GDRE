#include "gltf.h"

#include <spdlog/spdlog.h>

#include <fastgltf/tools.hpp>

namespace asset {
    void Primitive::load(const fastgltf::Asset& asset, const fastgltf::Primitive& primitive) {
        size_t initialVertex = mVertices.size();

        // Position attribute
        const auto positionAttribute = primitive.findAttribute("POSITION");

        if (positionAttribute == primitive.attributes.end()) {
            return;
        }

        // Position accesor
        const fastgltf::Accessor& positionAccessor = asset.accessors[positionAttribute->accessorIndex];

        const size_t vertexOffset = mVertices.size();

        mVertices.resize(vertexOffset + positionAccessor.count);

        // Positions
        fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
            asset,
            positionAccessor,
            [&](fastgltf::math::fvec3 position, size_t index) {
                mVertices[initialVertex + index].setPosition(position);
            }
        );

        // Normal attribute
        const auto normalAttribute = primitive.findAttribute("NORMAL");

        // Normals
        if (normalAttribute != primitive.attributes.end()) {
            fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
                asset,
                asset.accessors[normalAttribute->accessorIndex],
                [&](fastgltf::math::fvec3 normal, size_t index) {
                    mVertices[initialVertex + index].setNormal(normal);
                }
            );
        }

        // Uv attribute
        const auto uvAttribute = primitive.findAttribute("TEXCOORD_0");

        // Uvs
        if (uvAttribute != primitive.attributes.end()) {
            fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(
                asset,
                asset.accessors[uvAttribute->accessorIndex],
                [&](fastgltf::math::fvec2 uv, size_t index) {
                    mVertices[initialVertex + index].setUv(uv);
                }
            );
        }
        
        // Color attribute
        const auto colorAttribute = primitive.findAttribute("COLOR_0");

        // Colors
        if (colorAttribute != primitive.attributes.end()) {
            fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec4>(
                asset,
                asset.accessors[colorAttribute->accessorIndex],
                [&](fastgltf::math::fvec4 color, size_t index) {
                    mVertices[initialVertex + index].setColor(color);
                }
            );
        }

        // Indices
        if (primitive.indicesAccessor.has_value()) {
            fastgltf::iterateAccessor<uint32_t>(
                asset,
                asset.accessors[*primitive.indicesAccessor],
                [&](uint32_t index) {
                    mIndices.push_back(static_cast<uint32_t>(vertexOffset + index));
                }
            );
        }
    }

    void Mesh::load(const fastgltf::Asset& asset, const fastgltf::Mesh& mesh) {
        // Primitives
        for (const fastgltf::Primitive& fprimitive : mesh.primitives) {
            Primitive primitive;
            primitive.load(asset, fprimitive);

            mPrimitives.push_back(std::move(primitive));
        }
    }

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
