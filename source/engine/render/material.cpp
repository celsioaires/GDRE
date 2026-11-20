#include "material.h"

namespace render {
    bool Material::initialize(const backend::Backend& backend, const MaterialData& data, const TextureData& colorTextureData) {
        mBackend = &backend;

        const backend::Context& context = mBackend->getContext();

        // Descriptor set
        const backend::Descriptor& descriptor = mBackend->getDefaultDescriptor();
        const backend::DescriptorLayout& descriptorLayout = mBackend->getMaterialDescriptorLayout();

        if (!mDescriptorSet.initialize(
            context,
            descriptor.getPool(),
            descriptorLayout.getHandle()
        )) {
            return false;
        }

        // Uniform buffer
        if (!mUniformBuffer.initialize(
            context,
            backend::eBufferTypeUniform,
            sizeof(data),
            &data
        )) {
            return false;
        }

        // Color texture
        if (!mColorTexture.load(backend, colorTextureData)) {
            return false;
        }

        // Update descriptor
        updateDesriptorSet();

        return true;
    }

    void Material::updateDesriptorSet() {
        // Uniform buffer
        mDescriptorSet.updateBuffer(
            0,
            vk::DescriptorType::eUniformBuffer,
            mUniformBuffer.getHandle()
        );

        // Sampler
        mDescriptorSet.updateImage(
            1,
            vk::DescriptorType::eCombinedImageSampler,
            mColorTexture.getImage()
        );
    }
}
