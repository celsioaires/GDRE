#include "inspector.h"

namespace core {
    void Inspector::draw(const ImGuiWindowFlags flags) const {
        ImGui::Begin("Inspector", nullptr, flags);

        ImGui::End();
    }
}
