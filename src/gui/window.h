#pragma once

struct GLFWwindow;

namespace gui {
    class Window {
        GLFWwindow* mHandle = nullptr;
    public:
        bool initialize();
        void cleanup();

        void show();

        bool isOpen();
        bool isMinimized();

        GLFWwindow* getHandle() const { return mHandle; }
    };
}
