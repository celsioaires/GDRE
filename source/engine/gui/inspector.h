#pragma once

#include <imgui.h>

namespace core {
    class Inspector {
    public:
        void draw(const ImGuiWindowFlags flags) const;
    };
}
