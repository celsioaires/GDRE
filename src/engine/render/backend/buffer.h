#pragma once

#include "context.h"

namespace backend {
    class Buffer {
        vk::raii::Buffer mHandle = nullptr;
    public:
        bool initialize(const Context& context);
    private:
        inline static const Context* sContext = nullptr; // const Context& context

        // Initializers
        vk::raii::Buffer initializeBuffer();
    };
}
