#include "gui/config_bar.hpp"

#include <imgui-SFML.h>
#include <imgui.h>
#include <imgui_stdlib.h>

#include <spdlog/spdlog.h>

#include "gui/state.hpp"
#include "gui/utils.hpp"

#include "utils/file_utils.hpp"

namespace pnkd::gui
{

static constexpr auto screenshot_dir_helper_msg = R"(Specify the path to the directory containing your game screenshots.

If playing Cyberpunk 2077 through Steam on Windows, this will likely be:

C:\Program Files (x86)\Steam\userdata\<your_steam_ID>\760\remote\1091500\screenshots)";


auto check_and_update(pnkd::gui::state &g) -> bool
{
  g.show_screenshot_dir_confirmation = true;

  if (!g.screenshot_dir.empty())
  {
    if (pnkd::is_valid_folder(g.screenshot_dir))
    {
      spdlog::info("Folder looks good!");
      g.screenshot_dir_confirmation_msg = "Path looks good";
      return true;
    } else
    {
      spdlog::error("Nope!");
      g.screenshot_dir_confirmation_msg = "Path appears invalid";
      return false;
    }
  } else
  {
    spdlog::warn("No folder specified!");
    g.screenshot_dir_confirmation_msg = "No path specified!";
    return false;
  }
}

auto config_bar(pnkd::gui::state &g) -> void
{
  static constexpr float w = 500;
  float h = g.app_size.y;
  auto s = ImVec2{w, h};
  ImGui::SetNextWindowPos(ImVec2{0, g.menu_bar_size.y}, ImGuiCond_Always);
  ImGui::SetNextWindowSize(s, ImGuiCond_Always);

  // Start the window
  if (ImGui::Begin("Config", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus))
  {
    ImGui::Text("Configuration");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::SliderInt("Buffer size", &g.buffer_size, 1, 10);
    ImGui::SameLine();
    pnkd::gui::help_marker(R"(The buffer size of your in-game operating system)");

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Screenshot folder location:");
    if (ImGui::InputTextWithHint("Path", "/path/to/screenshot/folder", &g.screenshot_dir, ImGuiInputTextFlags_EnterReturnsTrue))
    {
      check_and_update(g);
    }
    ImGui::SameLine();
    pnkd::gui::help_marker(screenshot_dir_helper_msg);

    ImGui::Spacing();

    ImGui::Text("Refresh rate:");
    ImGui::SliderInt("Seconds", &g.scan_timer, 1, 10);
    ImGui::SameLine();
    pnkd::gui::help_marker(R"(How many seconds to wait before re-checking the screenshots directory for new images)");

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button("Breach"))
    {
      if (check_and_update(g))
      {
        // Start monitoring and trigger async puzzler
        spdlog::info("Breaching...");
      }
    }

    if (g.show_screenshot_dir_confirmation)
    {
      ImGui::SameLine();
      ImGui::Text("%s", g.screenshot_dir_confirmation_msg.data());
    }

    /*
    auto &colors = ImGui::GetStyle().Colors;
    for (std::size_t i = 0; i < ImGuiCol_COUNT; ++i).l;//
    {
      ImGui::Text("%s: %0.f, %0.f, %0.f, %0.f", enum_to_string(i).data(), colors[i].x * 255, colors[i].y * 255, colors[i].z * 255, colors[i].w * 255);
    }
    */

    //auto texture = sf::Texture{};

    ImGui::End();
  }
}

} // namespace pnkd::gui