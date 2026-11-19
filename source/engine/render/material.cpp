#include "material.h"

namespace render {
    bool Material::initialize(const backend::Backend& backend, const TextureData& colorData) {
        mBackend = &backend;

        const backend::Context& context = mBackend->getContext();

        // Descriptor set
        const backend::Descriptor& descriptor = mBackend->getDefaultDescriptor();
        const backend::DescriptorLayout& descriptorLayout = mBackend->getMaterialDescriptorLayout();

        if (!mDescriptorSet.initialize(context, descriptor.getPool(), descriptorLayout.getHandle())) {
            return false;
        }

        // Color texture
        if (!mColorTexture.load(backend, colorData)) {
            return false;
        }

        const backend::Image& colorImage = mColorTexture.getImage();

        mDescriptorSet.updateImage(0, vk::DescriptorType::eCombinedImageSampler, colorImage);

        return true;
    }
}
