#include "gui.h"

#include <imgui.h>

namespace gui {
    void Gui::initialize() {
        IMGUI_CHECKVERSION();

        ImGui::CreateContext();
        ImGui::StyleColorsClassic();
    }

    void Gui::cleanup() {
        ImGui::DestroyContext();
    }
}
