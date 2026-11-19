#pragma once

#include "../asset/loader.h"
#include "../render/renderer.h"

namespace scene {
    class Scene {
        render::Renderer* mRenderer = nullptr;

        std::unordered_map<const asset::Material*, const render::Material*> mMaterials;
        std::unordered_map<const asset::Primitive*, const render::Mesh*> mMeshes;
    public:
        bool load(const asset::Loader& loader, render::Renderer& renderer);
    private:
        bool loadAssets(const std::vector<asset::Gltf>& assets);
        bool loadMeshes(const std::vector<asset::Mesh>& meshes);
        bool loadMaterials(const std::vector<asset::Material>& materials);
        bool loadRenderables(const std::vector<asset::Node>& nodes);
    };
}
