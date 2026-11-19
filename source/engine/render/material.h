#pragma once

#include "backend/backend.h"

#include "texture.h"

namespace render {
    class Material {
        // Objects
        const backend::Backend* mBackend = nullptr;
        backend::DescriptorSet mDescriptorSet;
        Texture mColorTexture;
    public:
        bool initialize(const backend::Backend& backend, const TextureData& colorData);

        // Getters
        const backend::DescriptorSet& getDescriptorSet() const { return mDescriptorSet; }
    };
}
