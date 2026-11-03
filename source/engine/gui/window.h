#pragma once

#include <GLFW/glfw3.h>

namespace gui {
    class Window {
        GLFWwindow* mHandle;
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

        // Setters
        void setWidth(const int size) { mWidth = size; }
        void setHeight(const int size) { mHeight = size; }
    private:
        // Callbacks
        static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    };
}
