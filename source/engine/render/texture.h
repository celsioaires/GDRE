#pragma once

#include "backend/backend.h"

namespace render {
    struct TextureData {
        uint32_t width = 1;
        uint32_t height = 1;
        void* pixels = nullptr;
    };

    class Texture {
        backend::Image mImage;
    public:
        bool load(const backend::Backend& backend, const TextureData& data);

        // Getters
        const backend::Image& getImage() const { return mImage; };
    };
}
