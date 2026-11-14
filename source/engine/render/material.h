#pragma once

#include "backend/backend.h"

namespace render {
    struct MaterialData {

    };

    class Material {
        // Objects
        const backend::Backend* mBackend;
        MaterialData mData;
        backend::DescriptorSet mDescriptorSet;
    public:
        bool initialize(const backend::Backend& backend);
    };
}
