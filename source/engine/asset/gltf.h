#pragma once

#include <filesystem>

#include <stb_image.h>
#include <fastgltf/core.hpp>

namespace asset {
    class Vertex {
        fastgltf::math::fvec3 mPosition;
        fastgltf::math::fvec3 mNormal;
        fastgltf::math::fvec4 mTangent;
        fastgltf::math::fvec2 mUv;
        fastgltf::math::fvec4 mColor;
    public:
        // Getters
        const fastgltf::math::fvec3& getPosition() const { return mPosition; }
        const fastgltf::math::fvec3& getNormal() const { return mNormal; }
        const fastgltf::math::fvec4& getTangent() const { return mTangent; }
        const fastgltf::math::fvec2& getUv() const { return mUv; }
        const fastgltf::math::fvec4& getColor() const { return mColor; }

        // Setters
        void setPosition(const fastgltf::math::fvec3& position) { mPosition = position; }
        void setNormal(const fastgltf::math::fvec3& normal) { mNormal = normal; }
        void setTangent(const fastgltf::math::fvec4& tangent) { mTangent = tangent; }
        void setUv(const fastgltf::math::fvec2& uv) { mUv = uv; }
        void setColor(const fastgltf::math::fvec4& color) { mColor = color; }
    };

    class MaterialData {
        fastgltf::math::fvec4 mColorFactor;
        fastgltf::math::fvec4 mMetalRoughFactor;
    public:
        // Getters
        const fastgltf::math::fvec4& getColorFactor() const { return mColorFactor; }
        const fastgltf::math::fvec4& getMetalRoughFactor() const { return mMetalRoughFactor; }

        // Setters
        void setColorFactor(const fastgltf::math::fvec4& colorFactor) { mColorFactor = colorFactor; }
        void setMetalRoughFactor(const fastgltf::math::fvec4& metalRoughFactor) { mMetalRoughFactor = metalRoughFactor; }
    };

    class Texture {
        int mWidth = 0;
        int mHeight = 0;
        int mChannels = 0;
        stbi_uc* mPixels = nullptr;
    public:
        bool load(const fastgltf::Asset& asset, const fastgltf::Image& image);

        // Getters
        uint32_t getWidth() const { return mWidth; }
        uint32_t getHeight() const { return mHeight; }
        stbi_uc* getPixels() const { return mPixels; }
    };

    class Material {
        MaterialData mData;
        const Texture* mColorTexture = nullptr;
        const Texture* mMetalRoughTexture = nullptr;
    public:
        void load(const fastgltf::Material& material, const std::vector<Texture>& textures);

        // Getters
        const MaterialData& getData() const { return mData; }
        const Texture* getColorTexture() const { return mColorTexture; }
    };

    class Primitive {
        std::vector<Vertex> mVertices;
        std::vector<uint32_t> mIndices;
        const Material* mMaterial = nullptr;
    public:
        void load(const fastgltf::Asset& asset, const fastgltf::Primitive& primitive, const Material* material);

        // Getters
        const std::vector<Vertex>& getVertices() const { return mVertices; }
        const std::vector<uint32_t>& getIndices() const { return mIndices; }
        const Material* getMaterial() const { return mMaterial; }
    };

    class Mesh {
        std::vector<Primitive> mPrimitives;
    public:
        void load(const fastgltf::Asset& asset, const fastgltf::Mesh& mesh, const std::vector<Material>& materials);

        // Getters
        const std::vector<Primitive>& getPrimitives() const { return mPrimitives ; }
    };

    class Node {
        std::string mName;
        fastgltf::math::fmat4x4 mLocalTransform;
        const Mesh* mMesh = nullptr;
        std::vector<Node*> mChildrens;
        const Node* mParent = nullptr;
    public:
        void load(const fastgltf::Node& node, const std::vector<Mesh>& meshes, std::vector<Node>& nodes);

        // Getters
        const fastgltf::math::fmat4x4& getLocalTransform() const { return mLocalTransform; };
        const Mesh* getMesh() const { return mMesh; };
        const std::vector<Node*>& getChildrens() const { return mChildrens; }
        const Node* getParent() const { return mParent; }
    };

    class Gltf {
        fastgltf::Asset mAsset;

        // Data
        std::vector<Texture> mTextures;
        std::vector<Material> mMaterials;
        std::vector<Mesh> mMeshes;
        std::vector<Node> mNodes;
        std::vector<Node*> mRootNodes;
    public:
        bool load(const std::filesystem::path& path);

        // Getters
        const std::vector<Material>& getMaterials() const { return mMaterials; }
        const std::vector<Mesh>& getMeshes() const { return mMeshes; }
        const std::vector<Node>& getNodes() const { return mNodes; }
        const std::vector<Node*>& getRootNodes() const { return mRootNodes; }
    private:
        bool loadData();
    };
}
