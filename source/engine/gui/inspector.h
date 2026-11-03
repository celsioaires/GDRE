#pragma once

#include <imgui.h>

namespace gui {
    class Inspector {
    public:
        void draw(const ImGuiWindowFlags flags) const;
    };
}
