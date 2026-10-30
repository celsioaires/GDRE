#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace gui {
    class Window {
        GLFWwindow* mHandle = nullptr;
        int mWidth = 800;
        int mHeight = 600;
    public:
        bool initialize();
        void cleanup();

        bool isOpen();
        bool isMinimized();

        // Getters
        GLFWwindow* getHandle() const { return mHandle; }
        int getWidth() const { return mWidth; }
        int getHeight() const { return mHeight; }
    };
}
