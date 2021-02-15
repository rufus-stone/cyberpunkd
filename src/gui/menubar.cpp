#include "gui/menubar.hpp"

#include <imgui.h>

#include "gui/state.hpp"

namespace pnkd::gui
{

auto menubar(pnkd::gui::state &g) -> void
{
  if (ImGui::BeginMainMenuBar())
  {
    if (ImGui::BeginMenu("File"))
    {
      if (ImGui::MenuItem("Exit", "Ctrl+Q"))
      {
        g.app.close();
      }

      ImGui::EndMenu();
    }

    ImGui::Separator();

    if (ImGui::BeginMenu("Help"))
    {
      if (ImGui::MenuItem("About"))
      {
        g.show_about_win = true;
      }

      ImGui::EndMenu();
    }

    // Update the layout state with the size of this UI element
    g.menu_bar_size = ImGui::GetWindowSize();

    ImGui::EndMainMenuBar();
  }
}

} // namespace pnkd::gui