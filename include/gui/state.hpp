#pragma once

#include <filesystem>
#include <string>
#include <atomic>

#include <SFML/Graphics/RenderWindow.hpp>

#include <imgui.h>
#include <nlohmann/json.hpp>

#include "utils/config.hpp"

namespace pnkd::gui
{

using json = nlohmann::json;

struct state
{
  sf::RenderWindow &app;
  ImVec2 screen_size{};
  ImVec2 app_size{};
  ImVec2 menu_bar_size{};

  std::string screenshot_dir{};
  std::string screenshot_dir_confirmation_msg{};
  bool show_screenshot_dir_confirmation = false;

  std::string tessdata_dir_confirmation_msg{};
  bool show_tessdata_dir_confirmation = false;

  bool show_about_win = false;

  int sleep_for = 1;
  int buffer_size = 3;

  //json cfg{};
  pnkd::cfg::config_helper cfg_helper{};
  //std::filesystem::path cfg_file_path{};

  std::atomic_bool *watcher_flag = nullptr;

  bool breach_button_pushed = false;
  std::string breach_button_text = "Breach";
  //ImVec4 breach_colour =
  ImVec4 stop_breach_colour = static_cast<ImVec4>(ImColor{255, 0, 60}); // Red
};

} // namespace pnkd::gui