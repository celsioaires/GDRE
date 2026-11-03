#pragma once

#include <imgui.h>

namespace gui {
    class Hierarchy {
    public:
        void draw(const ImGuiWindowFlags flags) const;
    };
}
