#pragma once

#include <fastgltf/core.hpp>

namespace asset {
    class Vertex {
        fastgltf::math::fvec3 mPosition;
        fastgltf::math::fvec4 mColor;
    public:
        // Getters
        const fastgltf::math::fvec3& getPosition() const { return mPosition; }
        const fastgltf::math::fvec4& getColor() const { return mColor; }

        // Setters
        void setPosition(const fastgltf::math::fvec3& position) { mPosition = position; }
        void setColor(const fastgltf::math::fvec4& color) { mColor = color; }
    };

    class Primitive {
        std::vector<Vertex> mVertices;
        std::vector<uint32_t> mIndices;
    public:
        void load(const fastgltf::Asset& asset, const fastgltf::Primitive& primitive);

        // Getters
        const std::vector<Vertex>& getVertices() const { return mVertices; }
    };

    class Mesh {
        std::vector<Primitive> mPrimitives;
    public:
        void load(const fastgltf::Asset& asset, const fastgltf::Mesh&);

        // Getters
        const std::vector<Primitive>& getPrimitives() const { return mPrimitives ; }
    };
}
