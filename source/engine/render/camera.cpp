#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace render {
    bool Camera::initialize(const backend::Backend& backend) {
        mBackend = &backend;

        // Descriptor set
        const backend::Descriptor& descriptor = mBackend->getDefaultDescriptor();
        const backend::DescriptorLayout& descriptorLayout = mBackend->getDefaultDescriptorLayout();

        mDescriptorSet.handle = descriptor.allocateSet(descriptorLayout.getHandle());

        if (mDescriptorSet.handle == nullptr) {
            return false;
        }

        // Uniform buffer
        if (!mUniformBuffer.initialize(
            mBackend->getContext(),
            backend::eBufferTypeUniform,
            sizeof(mData),
            nullptr
        )) {
            return false;
        }

        updateData();

        return true;
    }

    void Camera::updateData() {
        // Window size
        int windowWidth, windowHeight;
        glfwGetWindowSize(mBackend->getContext().getWindow(), &windowWidth, &windowHeight);

        // Data
        mData = {
            .viewMatrix = glm::lookAt(glm::vec3(0, 0, 2), glm::vec3(0), glm::vec3(0, 1, 0)),
            .projectionMatrix = glm::perspective(
                glm::radians(70.0f), static_cast<float>(windowWidth) / windowHeight, 0.1f, 1000.0f
            )
        };

        // Uniform buffer
        mUniformBuffer.mapMemory(&mData);

        // Descriptor set
        const backend::Descriptor& descriptor = mBackend->getDefaultDescriptor();

        descriptor.updateSet(mDescriptorSet.handle, 0, vk::DescriptorType::eUniformBuffer, mUniformBuffer.getHandle());
    }
}
