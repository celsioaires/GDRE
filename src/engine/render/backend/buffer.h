#pragma once

#include "context.h"

namespace backend {
    class Buffer {
        vk::raii::Buffer mHandle = nullptr;
    public:
        bool initialize(const Context& context);
    private:
        static inline const Context* pContext = nullptr; // const Context& context

        // Initializers
        vk::raii::Buffer initializeBuffer();
    };
}
