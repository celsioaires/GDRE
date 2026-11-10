#pragma once

#include <GLFW/glfw3.h>

namespace gui {
    class Window {
        GLFWwindow* mHandle;
    public:
        ~Window();

        bool initialize();

        bool isOpen() const;
        bool isMinimized() const;

        // Getters
        GLFWwindow* getHandle() const { return mHandle; }
    private:
        // Callbacks
        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    };
}
