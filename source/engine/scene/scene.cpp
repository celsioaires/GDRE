#include "scene.h"

#include "glm/gtc/matrix_transform.hpp"

namespace scene {
    bool Scene::initialize(const asset::Loader& loader, render::Renderer& renderer) {
        const std::vector<asset::Gltf>& gltfAssets = loader.getGltfAssets();

        for (const asset::Gltf& asset : gltfAssets) {
            const std::vector<asset::Mesh>& meshes = asset.getMeshes();

            for (const asset::Mesh& mesh : meshes) {
                const std::vector<asset::Primitive>& primitives = mesh.getPrimitives();

                for (const asset::Primitive& primitive : primitives) {
                    const std::vector<asset::Vertex>& vertices = primitive.getVertices();

                    // Convert vertices
                    std::vector<backend::Vertex> convertedVertices(vertices.size());

                    int i = 0;

                    for (const asset::Vertex& vertice : vertices) {
                        convertedVertices[i] = {
                            .position = { vertice.getPosition().x(), vertice.getPosition().y(), vertice.getPosition().z() },
                            .color = { vertice.getColor().x(), vertice.getColor().y(), vertice.getColor().z() },
                        };

                        i++;
                    }

                    // Upload mesh
                    render::Mesh mesh;

                    if (!mesh.initialize(renderer.getBackend(), convertedVertices)) {
                        return false;
                    }

                    renderer.uploadMesh(std::move(mesh));
                }
            }
        }

        return true;
    }
}
