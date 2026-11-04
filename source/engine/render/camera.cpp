#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace render {
    bool Camera::initialize(const backend::Backend& backend) {
        const backend::Context& context = backend.getContext();

        // Window size
        int windowWidth, windowHeight;
        glfwGetWindowSize(context.getWindow(), &windowWidth, &windowHeight);

        // Data
        mData = {
            .viewMatrix = glm::lookAt(glm::vec3(0, 0, 2), glm::vec3(0), glm::vec3(0, 1, 0)),
            .projectionMatrix = glm::perspective(
                glm::radians(70.0f), static_cast<float>(windowWidth) / windowHeight, 0.1f, 1000.0f
            )
        };

        // Uniform buffer
        if (!mUniformBuffer.initialize(
            context,
            backend::eBufferTypeUniform,
            sizeof(mData),
            &mData
        )) {
            return false;
        }

        return true;
    }
}
