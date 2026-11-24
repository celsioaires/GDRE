#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace render {
    bool Camera::initialize(const backend::Backend& backend) {
        mBackend = &backend;

        const backend::Context& context = mBackend->getContext();

        // Descriptor set
        const backend::DescriptorPool& descriptorPool = mBackend->getDefaultDescriptorPool();
        const backend::DescriptorLayout& descriptorLayout = mBackend->getCameraDescriptorLayout();

        if (!mDescriptorSet.initialize(context, descriptorPool, descriptorLayout)) {
            return false;
        }

        // Uniform buffer
        if (!mUniformBuffer.initialize(context, backend::eBufferTypeUniform, sizeof(mData), nullptr)) {
            return false;
        }

        updateData();

        return true;
    }

void Camera::updateData() {
    static glm::vec3 position(30, 0, 85);
    static float yaw = -110.0f;
    static float pitch = 0.0f;

    static double lastX = 0, lastY = 0;
    static bool firstMouse = true;

    auto window = mBackend->getContext().getWindow();

    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    // Mouse
    double x, y;
    glfwGetCursorPos(window, &x, &y);

    if (firstMouse) {
        lastX = x;
        lastY = y;
        firstMouse = false;
    }

    float sensitivity = 0.1f;

    yaw += (x - lastX) * sensitivity;
    pitch -= (y - lastY) * sensitivity;

    lastX = x;
    lastY = y;

    pitch = glm::clamp(pitch, -89.0f, 89.0f);

    // Direction
    glm::vec3 direction(
        cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
        sin(glm::radians(pitch)),
        sin(glm::radians(yaw)) * cos(glm::radians(pitch))
    );

    glm::vec3 forward = glm::normalize(direction);
    glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));

    // Keyboard
    float speed = 0.2f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) position += forward * speed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) position -= forward * speed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) position -= right * speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) position += right * speed;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) position.y += speed;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) position.y -= speed;

    // Data
    mData = {
        .viewMatrix = glm::lookAt(position, position + forward, glm::vec3(0, 1, 0)),
        .projectionMatrix = glm::perspective(
            glm::radians(70.0f),
            static_cast<float>(windowWidth) / windowHeight,
            0.1f,
            1000.0f
        )
    };

    mData.projectionMatrix[1][1] *= -1;

    // Uniform buffer
    mUniformBuffer.mapMemory(&mData);

    // Descriptor set
    mDescriptorSet.updateBuffer(
        0,
        vk::DescriptorType::eUniformBuffer,
        mUniformBuffer.getHandle()
    );
}

}
