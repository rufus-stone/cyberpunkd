#pragma once

#include <string>

#include <SFML/Graphics/RenderWindow.hpp>

#include <imgui.h>


namespace pnkd::gui
{

struct state
{
  sf::RenderWindow &app;
  ImVec2 screen_size{};
  ImVec2 app_size{};
  ImVec2 menu_bar_size{};

  std::string screenshot_dir{};
  std::string screenshot_dir_confirmation_msg{};
  bool show_screenshot_dir_confirmation = false;

  bool show_about_win = false;

  int scan_timer = 1;
  int buffer_size = 3;
};

} // namespace pnkd::gui