#include "gui/menubar.hpp"

#include <imgui.h>

#include "gui/state.hpp"
#include "utils/config.hpp"

namespace pnkd::gui
{

auto menubar(pnkd::gui::state &g) -> void
{
  if (ImGui::BeginMainMenuBar())
  {
    if (ImGui::BeginMenu("File"))
    {
      if (ImGui::MenuItem("Save", "Ctrl+S"))
      {
        // Update the config with the state of the gui widgets
        g.cfg_helper.cfg_json()["buffer_size"] = g.buffer_size;
        g.cfg_helper.cfg_json()["sleep_for"] = g.sleep_for;
        g.cfg_helper.cfg_json()["screenshot_dir"] = g.screenshot_dir;

        // Save the updated config JSON to file
        g.cfg_helper.write_config(g.cfg_helper.cfg_json());
      }

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