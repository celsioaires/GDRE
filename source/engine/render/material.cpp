#include "material.h"

namespace render {
    bool Material::initialize(const backend::Backend& backend, const MaterialData& data, const TextureData& colorTextureData, const TextureData& metalRoughTextureData) {
        mBackend = &backend;

        const backend::Context& context = mBackend->getContext();

        // Descriptor set
        const backend::DescriptorPool& descriptorPool = mBackend->getDefaultDescriptorPool();
        const backend::DescriptorLayout& descriptorLayout = mBackend->getMaterialDescriptorLayout();

        if (!mDescriptorSet.initialize(
            context,
            descriptorPool,
            descriptorLayout
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

        // Metal rough texture
        if (!mMetalRoughTexture.load(backend, metalRoughTextureData)) {
            return false;
        }

        // Update descriptor
        updateDesriptorSet();

        return true;
    }

    void Material::updateDesriptorSet() {
        // Data
        mDescriptorSet.updateBuffer(
            0,
            vk::DescriptorType::eUniformBuffer,
            mUniformBuffer.getHandle()
        );

        // Color texture
        mDescriptorSet.updateImage(
            1,
            vk::DescriptorType::eCombinedImageSampler,
            mColorTexture.getImage()
        );

        // Metal rough texture
        mDescriptorSet.updateImage(
            2,
            vk::DescriptorType::eCombinedImageSampler,
            mMetalRoughTexture.getImage()
        );
    }
}
