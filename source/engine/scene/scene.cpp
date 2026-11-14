#include "scene.h"

namespace scene {
    bool Scene::initialize(const asset::Loader& loader, render::Renderer& renderer) {
        mRenderer = &renderer;

        if (!uploadGltfAssets(loader.getGltfAssets())) {
            return false;
        }

        return true;
    }

    bool Scene::uploadGltfAssets(const std::vector<asset::Gltf>& assets) {
        // Assets
        for (const asset::Gltf& asset : assets) {
            // Nodes
            for (const asset::Node& node : asset.getNodes()) {
                if (!uploadGltfNode(node, glm::mat4(1))) {
                    return false;
                }
            }
        }

        return true;
    }

    bool Scene::uploadGltfNode(const asset::Node& node, const glm::mat4& parentTransform) {
        // World transform
        const fastgltf::math::fmat4x4& localTransform = node.getLocalTransform();
        glm::mat4 convertedLocalTransform(1);

        for (int i = 0; i < 4; i++) {
            convertedLocalTransform[i] = {
                localTransform[i].x(),
                localTransform[i].y(),
                localTransform[i].z(),
                localTransform[i].w()
            };
        }

        const glm::mat4 worldTransform = parentTransform * convertedLocalTransform;

        // Mesh
        const asset::Mesh* mesh = node.getMesh();

        if (mesh != nullptr) {
            // Primitives
            const std::vector<asset::Primitive>& primitives = mesh->getPrimitives();

            for (const asset::Primitive& primitive : primitives) {
                // Vertices
                const std::vector<asset::Vertex>& vertices = primitive.getVertices();
                std::vector<backend::Vertex> convertedVertices(vertices.size());

                int i = 0;

                for (const asset::Vertex& vertice : vertices) {
                    convertedVertices[i] = {
                        .position = {
                            vertice.getPosition().x(), vertice.getPosition().y(), vertice.getPosition().z()
                        },
                        .color = {
                            vertice.getColor().x(), vertice.getColor().y(), vertice.getColor().z()
                        },
                    };

                    i++;
                }

                // Upload
                if (!mRenderer->uploadMesh(worldTransform, convertedVertices, primitive.getIndices())) {
                    return false;
                }
            }
        }

        // Childrens
        const std::vector<asset::Node>& childrens = node.getChildrens();

        for (const asset::Node& children : childrens) {
            if (!uploadGltfNode(children, worldTransform)) {
                return false;
            }
        }

        return true;
    }
}
