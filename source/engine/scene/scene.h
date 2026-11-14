#pragma once

#include "../asset/loader.h"
#include "../render/renderer.h"

namespace scene {
    class Scene {
        render::Renderer* mRenderer;
    public:
        bool initialize(const asset::Loader& loader, render::Renderer& renderer);
    private:
        bool uploadGltfAssets(const std::vector<asset::Gltf>& assets);
        bool uploadGltfNode(const asset::Node& node, const glm::mat4& parentTransform);
    };
}
