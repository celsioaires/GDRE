#include "texture.h"

namespace render {
    bool Texture::load(const backend::Backend& backend, const TextureData& data) {
        uint32_t width = data.width;
        uint32_t height = data.height;

        // Image
        if (!mImage.initialize(
            backend.getContext(),
            backend::eImageTypeSampler,
            { width, height }
        )) {
            return false;
        }

        // Staging buffer
        backend::Buffer stagingBuffer;

        if (!stagingBuffer.initialize(
            backend.getContext(),
            backend::eBufferTypeStaging,
            width * height * 4
        )) {
            return false;
        }

        if (data.pixels != nullptr) {
            if (!stagingBuffer.mapMemory(data.pixels)) {
                return false;
            }
        }

        // Commads
        backend.beginImmediateCommands();

        const backend::Command& command = backend.getImmediateCommand();

        command.copyBufferToImage(stagingBuffer, mImage, { width, height});

        backend.endImmediateCommands();

        return true;
    }
}
