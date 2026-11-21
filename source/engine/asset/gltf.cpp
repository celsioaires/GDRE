#define STB_IMAGE_IMPLEMENTATION

#include "gltf.h"

#include <spdlog/spdlog.h>

#include <fastgltf/tools.hpp>

namespace asset {
    bool Texture::load(const fastgltf::Asset& asset, const fastgltf::Image& image) {
        // Handle
        std::visit(
            fastgltf::visitor {
                [&](const fastgltf::sources::URI& source) {
                    const std::string path(source.uri->path().begin(), source.uri->path().end());

                    mPixels = stbi_load(path.c_str(), &mWidth, &mHeight, &mChannels, STBI_rgb_alpha);
                },
                [&](const fastgltf::sources::Vector& vector) {
                    mPixels = stbi_load_from_memory(
                        reinterpret_cast<stbi_uc const*>(vector.bytes.data()),
                        static_cast<int>(vector.bytes.size()),
                        &mWidth,
                        &mHeight,
                        &mChannels,
                        STBI_rgb_alpha
                    );
                },
                [&](const fastgltf::sources::Array& array) {
                    mPixels = stbi_load_from_memory(
                        reinterpret_cast<const stbi_uc*>(array.bytes.data()),
                        static_cast<int>(array.bytes.size()),
                        &mWidth,
                        &mHeight,
                        &mChannels,
                        STBI_rgb_alpha
                    );
                },
                [&](const fastgltf::sources::BufferView& view) {
                    const fastgltf::BufferView& bufferView = asset.bufferViews[view.bufferViewIndex];
                    const fastgltf::Buffer& buffer = asset.buffers[bufferView.bufferIndex];

                    std::visit(
                        fastgltf::visitor{
                            [&](const fastgltf::sources::Vector& vector) {
                                mPixels = stbi_load_from_memory(
                                    reinterpret_cast<stbi_uc const*>(vector.bytes.data() + bufferView.byteOffset),
                                    static_cast<int>(bufferView.byteLength),
                                    &mWidth,
                                    &mHeight,
                                    &mChannels,
                                    STBI_rgb_alpha
                                );
                            },
                            [&](const fastgltf::sources::Array& array) {
                                const size_t offset = bufferView.byteOffset;
                                const size_t size = bufferView.byteLength;

                                if (offset > array.bytes.size() ||
                                    size > array.bytes.size() - offset) {
                                    spdlog::error("BufferView out of bounds");
                                    return;
                                }

                                mPixels = stbi_load_from_memory(
                                    reinterpret_cast<const stbi_uc*>(array.bytes.data() + offset),
                                    static_cast<int>(size),
                                    &mWidth,
                                    &mHeight,
                                    &mChannels,
                                    STBI_rgb_alpha
                                );
                            },
                            [&](const auto& source) {
                                spdlog::error("Unsupported buffer source: {}", typeid(source).name());
                            }
                        },
                        buffer.data
                    );
                },
                [&](auto& source) {
                    spdlog::error("Unsupported image source: {}", typeid(source).name());
                }
            },
            image.data
        );

        if (mPixels == nullptr) {
            spdlog::error("Failed to load glTF texture");
            return false;
        }

        return true;
    }

    void Material::load(const fastgltf::Material& material, const std::vector<Texture>& textures) {
        // Data
        mData.setColorFactor({
            material.pbrData.baseColorFactor[0],
            material.pbrData.baseColorFactor[1],
            material.pbrData.baseColorFactor[2],
            material.pbrData.baseColorFactor[3],
        });

        mData.setMetalRoughFactor({
            material.pbrData.metallicFactor,
            material.pbrData.roughnessFactor
        });

        // Color texture
        std::optional<fastgltf::TextureInfo> colorTexture = material.pbrData.baseColorTexture;

        if (colorTexture.has_value()) {
            mColorTexture = &textures[colorTexture->textureIndex];
        }

        // Metal rough texture
        std::optional<fastgltf::TextureInfo> metalRoughTexture = material.pbrData.metallicRoughnessTexture;

        if (metalRoughTexture.has_value()) {
            mMetalRoughTexture = &textures[metalRoughTexture->textureIndex];
        }
    }

    void Primitive::load(const fastgltf::Asset& asset, const fastgltf::Primitive& primitive, const Material* material) {
        size_t initialVertex = mVertices.size();

        // Position attribute
        const auto positionAttrib = primitive.findAttribute("POSITION");

        if (positionAttrib == primitive.attributes.end()) {
            return;
        }

        // Position accesor
        const fastgltf::Accessor& positionAccessor = asset.accessors[positionAttrib->accessorIndex];

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

        // Normal attrib
        const auto normalAttrib = primitive.findAttribute("NORMAL");

        // Normals
        if (normalAttrib != primitive.attributes.end()) {
            fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
                asset,
                asset.accessors[normalAttrib->accessorIndex],
                [&](fastgltf::math::fvec3 normal, size_t index) {
                    mVertices[initialVertex + index].setNormal(normal);
                }
            );
        }

        // Tangent attrib
        const auto tangentAttrib = primitive.findAttribute("TANGENT");

        // Tangent
        if (tangentAttrib != primitive.attributes.end()) {
            fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec4>(
                asset,
                asset.accessors[tangentAttrib->accessorIndex],
                [&](fastgltf::math::fvec4 tangent, size_t index) {
                    mVertices[initialVertex + index].setTangent(tangent);
                }
            );
        }
        // Uv attribute
        const auto uvAttrib = primitive.findAttribute("TEXCOORD_0");

        // Uvs
        if (uvAttrib != primitive.attributes.end()) {
            fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(
                asset,
                asset.accessors[uvAttrib->accessorIndex],
                [&](fastgltf::math::fvec2 uv, size_t index) {
                    mVertices[initialVertex + index].setUv(uv);
                }
            );
        }

        // Color attribute
        const auto colorAttrib = primitive.findAttribute("COLOR_0");

        // Colors
        if (colorAttrib != primitive.attributes.end()) {
            fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec4>(
                asset,
                asset.accessors[colorAttrib->accessorIndex],
                [&](fastgltf::math::fvec4 color, size_t index) {
                    mVertices[initialVertex + index].setColor(color);
                }
            );
        }
        else {
            for (size_t i = 0; i < mVertices.size(); i++) {
                mVertices[initialVertex + i].setColor({ 1.0f });
            }
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

        mMaterial = material;
    }

    void Mesh::load(const fastgltf::Asset& asset, const fastgltf::Mesh& mesh, const std::vector<Material>& materials) {
        // Primitives
        for (const fastgltf::Primitive& fprimitive : mesh.primitives) {
            Primitive primitive;

            const Material* material = nullptr;

            if (fprimitive.materialIndex.has_value()) {
                material = &materials[fprimitive.materialIndex.value()];
            }

            primitive.load(asset, fprimitive, material);

            mPrimitives.push_back(std::move(primitive));
        }
    }

    void Node::load(const fastgltf::Node& node, const std::vector<Mesh>& meshes, std::vector<Node>& nodes) {
        mName = node.name;
        mLocalTransform = fastgltf::getLocalTransformMatrix(node);

        // Mesh
        if (node.meshIndex.has_value()) {
            mMesh = &meshes[*node.meshIndex];
        }

        // Childrens
        mChildrens.reserve(node.children.size());

        for (size_t childIndex : node.children) {
            Node* children = &nodes[childIndex];
            children->mParent = this;

            mChildrens.push_back(children);
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

        // Data
        if (!loadData()) {
            return false;
        }

        return true;
    }

    bool Gltf::loadData() {
        size_t i;

        // Textures
        mTextures.resize(mAsset.textures.size());

        i = 0;

        for (Texture& texture : mTextures) {
            const fastgltf::Texture& ftexture = mAsset.textures[i];
            std::optional<size_t> imageIndex = ftexture.imageIndex;

            if (imageIndex.has_value()) {
                if (!texture.load(mAsset, mAsset.images[imageIndex.value()])) {
                    return false;
                }
            }

            i++;
        }

        // Materials
        mMaterials.resize(mAsset.materials.size());

        i = 0;

        for (Material& material : mMaterials) {
            material.load(mAsset.materials[i], mTextures);
            i++;
        }

        // Meshes
        mMeshes.resize(mAsset.meshes.size());

        i = 0;

        for (Mesh& mesh : mMeshes) {
            mesh.load(mAsset, mAsset.meshes[i], mMaterials);
            i++;
        }

        // Nodes
        mNodes.resize(mAsset.nodes.size());

        i = 0;

        for (Node& node : mNodes) {
            node.load(mAsset.nodes[i], mMeshes, mNodes);
            i++;
        }

        // Root nodes
        mRootNodes.reserve(mNodes.size());

        for (Node& node : mNodes) {
            if (node.getParent() == nullptr) {
                mRootNodes.push_back(&node);
            }
        }

        return true;
    }
}
