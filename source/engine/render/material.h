#pragma once

#include "backend/backend.h"

#include "texture.h"

namespace render {
    struct MaterialData {
        glm::vec4 colorFactors;
        glm::vec4 metalRoughFactors;
        glm::vec4 padding0;
    };

    class Material {
        const backend::Backend* mBackend = nullptr;

        // Data
        MaterialData mData;
        backend::DescriptorSet mDescriptorSet;
        backend::Buffer mUniformBuffer;
        Texture mColorTexture;
        Texture mMetalRoughTexture;
    public:
        bool initialize(const backend::Backend& backend, const MaterialData& data, const TextureData& colorTextureData, const TextureData& metalRoughTextureData);

        // Getters
        const backend::DescriptorSet& getDescriptorSet() const { return mDescriptorSet; }
    private:
        void updateDesriptorSet();
    };
}
