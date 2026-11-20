#include "scene.h"

namespace scene {
    bool Scene::load(const asset::Loader& loader, render::Renderer& renderer) {
        mRenderer = &renderer;

        // Data
        if (!loadAssets(loader.getAssets())) {
            return false;
        }

        return true;
    }

    bool Scene::loadAssets(const std::vector<asset::Gltf>& assets) {
        for (const asset::Gltf& asset : assets) {
            if (
                !loadMeshes(asset.getMeshes()) ||
                !loadMaterials(asset.getMaterials()) ||
                !loadRenderables(asset.getNodes())
            ) {
                return false;
            }
        }

        return true;
    }

    bool Scene::loadMeshes(const std::vector<asset::Mesh>& meshes) {
        for (const asset::Mesh& mesh : meshes) {
            // Primitives
            const std::vector<asset::Primitive>& primitives = mesh.getPrimitives();

            for (const asset::Primitive& primitive : primitives) {
                // Vertices
                const std::vector<asset::Vertex>& vertices = primitive.getVertices();
                std::vector<backend::Vertex> backendVertices;

                for (const asset::Vertex& vertex : vertices) {
                    // Attribs
                    const fastgltf::math::fvec3& position = vertex.getPosition();
                    const fastgltf::math::fvec4& color = vertex.getColor();
                    const fastgltf::math::fvec2& uv = vertex.getUv();

                    backendVertices.push_back({
                        .position = { position.x(), position.y(), position.z() },
                        .color = { color.x(), color.y(), color.z() },
                        .uv = { uv.x(), uv.y() }
                    });
                }

                // Upload
                const render::Mesh* uploadedMesh = mRenderer->uploadMesh(backendVertices, primitive.getIndices());

                if (uploadedMesh == nullptr) {
                    return false;
                }

                mMeshes[&primitive] = uploadedMesh;
            }
        }

        return true;
    }

    bool Scene::loadMaterials(const std::vector<asset::Material>& materials) {
        for (const asset::Material& material : materials) {
            // Data
            const asset::MaterialData& data = material.getData();

            const fastgltf::math::fvec4& colorFactors = data.getColorFactors();
            const fastgltf::math::fvec4& metalRoughFactors = data.getMetalRoughFactors();

            render::MaterialData renderData{
                .colorFactors = {
                    colorFactors.x(),
                    colorFactors.y(),
                    colorFactors.z(),
                    colorFactors.w()
                },
                .metalRoughFactors = {
                    metalRoughFactors.x(),
                    metalRoughFactors.y(),
                    0, 0
                }
            };

            // Color texture
            const asset::Texture* colorTexture = material.getColorTexture();

            uint32_t colorPixel = 0xFFFFFFFF;

            render::TextureData colorTextureData{ .pixels = &colorPixel };

            if (colorTexture != nullptr && colorTexture->getPixels() != nullptr) {
                colorTextureData = {
                    .width = colorTexture->getWidth(),
                    .height = colorTexture->getHeight(),
                    .pixels = colorTexture->getPixels()
                };
            }

            // Metal rough texture
            const asset::Texture* metalRoughTexture = material.getColorTexture();

            uint32_t metalRoughPixel = 0xFF00FF00;

            render::TextureData metalRoughTextureData{ .pixels = &metalRoughPixel };

            if (metalRoughTexture != nullptr && metalRoughTexture->getPixels() != nullptr) {
                colorTextureData = {
                    .width = metalRoughTexture->getWidth(),
                    .height = metalRoughTexture->getHeight(),
                    .pixels = metalRoughTexture->getPixels()
                };
            }

            // Upload
            const render::Material* uploadedMaterial =
                mRenderer->uploadMaterial(renderData, colorTextureData, metalRoughTextureData);

            if (uploadedMaterial == nullptr) {
                return false;
            }

             mMaterials[&material] = uploadedMaterial;
        }

        return true;
    }

    bool Scene::loadRenderables(const std::vector<asset::Node>& nodes) {
        for (const asset::Node& node : nodes) {
            // World transform
            glm::mat4 worldTransform(1);

            const asset::Node* parentNode = &node;

            while (parentNode != nullptr) {
                // Hierarchy transform
                const fastgltf::math::fmat4x4& localTransform = parentNode->getLocalTransform();
                glm::mat4 backendLocalTransform(1);

                for (int i = 0; i < 4; i++) {
                    backendLocalTransform[i] = {
                        localTransform[i].x(),
                        localTransform[i].y(),
                        localTransform[i].z(),
                        localTransform[i].w()
                    };
                }

                worldTransform = backendLocalTransform * worldTransform;
                parentNode = parentNode->getParent();
            }

            // Mesh
            const asset::Mesh* mesh = node.getMesh();

            if (mesh == nullptr) {
                continue;
            }

            // Primitives
            const std::vector<asset::Primitive>& primitives = mesh->getPrimitives();

            for (const asset::Primitive& primitive : primitives) {
                // Find GPU mesh
                auto backendMesh = mMeshes.find(&primitive);

                if (backendMesh == mMeshes.end()) {
                    continue;
                }

                // Find GPU material
                auto backendMaterial = mMaterials.find(primitive.getMaterial());

                if (backendMaterial == mMaterials.end()) {
                    continue;
                }

                // Upload
                mRenderer->uploadRenderable(backendMesh->second, backendMaterial->second, worldTransform);
            }
        }

        return true;
    }
}
