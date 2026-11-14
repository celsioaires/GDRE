#pragma once

#include <filesystem>

#include <fastgltf/core.hpp>

namespace asset {
    class Vertex {
        fastgltf::math::fvec3 mPosition;
        fastgltf::math::fvec3 mNormal;
        fastgltf::math::fvec2 mUv;
        fastgltf::math::fvec4 mColor;
    public:
        // Getters
        const fastgltf::math::fvec3& getPosition() const { return mPosition; }
        const fastgltf::math::fvec4& getColor() const { return mColor; }

        // Setters
        void setPosition(const fastgltf::math::fvec3& position) { mPosition = position; }
        void setNormal(const fastgltf::math::fvec3& normal) { mNormal = normal; }
        void setUv(const fastgltf::math::fvec2& uv) { mUv = uv; }
        void setColor(const fastgltf::math::fvec4& color) { mColor = color; }
    };

    class Primitive {
        std::vector<Vertex> mVertices;
        std::vector<uint32_t> mIndices;
    public:
        void load(const fastgltf::Asset& asset, const fastgltf::Primitive& primitive);

        // Getters
        const std::vector<Vertex>& getVertices() const { return mVertices; }
        const std::vector<uint32_t>& getIndices() const { return mIndices; }
    };

    class Mesh {
        std::vector<Primitive> mPrimitives;
    public:
        void load(const fastgltf::Asset& asset, const fastgltf::Mesh& mesh);

        // Getters
        const std::vector<Primitive>& getPrimitives() const { return mPrimitives ; }
    };

    class Material {
    public:
        void load(const fastgltf::Asset& asset, const fastgltf::Material& material);
    };

    class Node {
        std::string mName;
        fastgltf::math::fmat4x4 mLocalTransform;
        const Mesh* mMesh;
        std::vector<Node> mChildrens;

    public:
        void load(const fastgltf::Asset& asset, const fastgltf::Node& node, const std::vector<Mesh>& meshes);

        // Getters
        const fastgltf::math::fmat4x4& getLocalTransform() const { return mLocalTransform; };
        const Mesh* getMesh() const { return mMesh; };
        const std::vector<Node>& getChildrens() const { return mChildrens; }
    };

    class Gltf {
        fastgltf::Asset mAsset;

        std::vector<Mesh> mMeshes;
        std::vector<Node> mNodes;
        std::vector<Material> mMaterials;
    public:
        bool load(const std::filesystem::path& path);

        // Getters
        const std::vector<Mesh>& getMeshes() const { return mMeshes; }
        const std::vector<Node>& getNodes() const { return mNodes; }
        const std::vector<Material>& getMaterials() const { return mMaterials; }
    private:
        void loadData();

        void loadNodes();
    };
}
