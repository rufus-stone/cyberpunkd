#include "gui/about.hpp"

#include <imgui.h>

#include "gui/state.hpp"

namespace pnkd::gui
{

static constexpr auto about_msg =
  R"(Solution finder for the Cyberpunk 2077 Breach Protocol mini-game.

Because the real hacking mini-game is hacking the hacking mini-game.)";

void centre_aligned_text(std::string const &txt)
{
  float const font_size = ImGui::GetFontSize() * txt.size() / 2;

  ImGui::SameLine(ImGui::GetWindowSize().x / 2 - font_size + (font_size / 2));

  ImGui::Text("%s", txt.c_str());
}

auto about_dialog(pnkd::gui::state &g) -> void
{
  // Set up window position/size/shape
  if (g.show_about_win)
  {
    static constexpr float w = 500;
    static constexpr float h = 200;
    auto s = ImVec2{w, h};
    ImGui::SetNextWindowPos(ImVec2{(g.app_size.x / 2) - (w / 2), (g.app_size.y / 2) - (h / 2)}, ImGuiCond_Always);
    ImGui::SetNextWindowSize(s, ImGuiCond_Always);

    // Start the window
    if (ImGui::Begin("About", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove))
    {
      centre_aligned_text("cyberpunkd 1.1.0");

      ImGui::Dummy({0, 25});

      ImGui::TextWrapped(about_msg);

      ImGui::Dummy({0, 25});

      auto button_width = s.x * 0.9F;
      auto indent = (s.x / 2) - (button_width / 2) - 8; // -6 because of the safety space???
      ImGui::Indent(indent);
      if (ImGui::Button("Close", ImVec2{button_width, 0}))
      {
        g.show_about_win = false;
      }

      ImGui::End();
    }
  }
}

} // namespace pnkd::gui