#include "inspector.h"

namespace gui {
    void Inspector::draw(const ImGuiWindowFlags flags) const {
        ImGui::Begin("Inspector", nullptr, flags);

        ImGui::End();
    }
}
