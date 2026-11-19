#include "scene.h"
#include <iostream>
namespace scene {
    bool Scene::load(const asset::Loader& loader, render::Renderer& renderer) {
        mRenderer = &renderer;

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
                    const fastgltf::math::fvec3& position = vertex.getPosition();
                    const fastgltf::math::fvec4& color = vertex.getColor();
                    const fastgltf::math::fvec2& uv = vertex.getUv();

                    backendVertices.push_back({
                        .position = { position.x(), position.y(), position.z() },
                        .color = { color.x(), color.y(), color.z() },
                        .uv = { uv.x(), uv.y() }
                    });
                }

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
            // Color Texture
            const asset::Texture* colorTexture = material.getColorTexture();

            if (colorTexture == nullptr) {
                continue;
            }

            const render::TextureData colorTextureData{
                .width = colorTexture->getWidth(),
                .height = colorTexture->getHeight(),
                .pixels = colorTexture->getPixels()
            };

            const render::Material* uploadedMaterial = mRenderer->uploadMaterial(colorTextureData);

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

            const asset::Mesh* mesh = node.getMesh();

            if (mesh == nullptr) {
                continue;
            }

            const std::vector<asset::Primitive>& primitives = mesh->getPrimitives();

            for (const asset::Primitive& primitive : primitives) {
                // Mesh
                auto backendMesh = mMeshes.find(&primitive);

                if (backendMesh == mMeshes.end()) {
                    continue;
                }

                // Material
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
