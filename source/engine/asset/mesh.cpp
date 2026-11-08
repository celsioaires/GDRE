#include "mesh.h"

#include <spdlog/spdlog.h>

#include <glm/glm.hpp>

#include <fastgltf/tools.hpp>

namespace asset {
    void Primitive::load(const fastgltf::Asset& asset, const fastgltf::Primitive& primitive) {
        size_t initialVertex = mVertices.size();

        // Position attribute
        const std::ranges::borrowed_iterator_t<const std::vector<fastgltf::Attribute>&> positionAttribute =
            primitive.findAttribute("POSITION");

        if (positionAttribute == primitive.attributes.end()) {
            return;
        }

        // Position accesor
        const fastgltf::Accessor& positionAccessor = asset.accessors[positionAttribute->accessorIndex];

        mVertices.resize(mVertices.size() + positionAccessor.count);

        std::vector<Vertex> vertices(positionAccessor.count);

        // Position
        fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
            asset,
            positionAccessor,
            [&](fastgltf::math::fvec3 position, size_t index) { vertices[initialVertex + index].setPosition(position); }
        );

        // Color attribute
        const std::ranges::borrowed_iterator_t<const std::vector<fastgltf::Attribute>&> colorAttribute =
            primitive.findAttribute("COLOR_0");

        // Color
        if (colorAttribute != primitive.attributes.end()) {
            fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec4>(
                asset,
                asset.accessors[colorAttribute->accessorIndex],
                [&](fastgltf::math::fvec4 color, size_t index) { vertices[initialVertex + index].setColor(color); }
            );
        }

        if (primitive.indicesAccessor.has_value()) {
            const auto& indexAccessor =
                asset.accessors[*primitive.indicesAccessor];

            fastgltf::iterateAccessorWithIndex<uint32_t>(
                asset,
                indexAccessor,
                [&](uint32_t index, size_t) { mVertices.push_back(vertices[index]); }
            );
        } else {
            mVertices.insert(mVertices.end(), vertices.begin(), vertices.end());
        }
    }

    void Mesh::load(const fastgltf::Asset& asset, const fastgltf::Mesh& mesh) {
        for (const fastgltf::Primitive& fprimitive : mesh.primitives) {
            Primitive primitive;
            
            primitive.load(asset, fprimitive);

            mPrimitives.push_back(std::move(primitive));
        }
    }
}
