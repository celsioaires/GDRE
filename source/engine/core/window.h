#pragma once

#include <GLFW/glfw3.h>

namespace core {
    class Window {
        GLFWwindow* mHandle;
    public:
        ~Window();

        bool initialize();

        void maximize() const;

        bool isOpen() const;
        bool isMinimized() const;

        // Getters
        GLFWwindow* getHandle() const { return mHandle; }
    private:
        // Callbacks
        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    };
}
