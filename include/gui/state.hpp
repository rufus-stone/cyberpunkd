#pragma once

#include <filesystem>
#include <string>
#include <atomic>

#include <SFML/Graphics/RenderWindow.hpp>

#include <imgui.h>
#include <nlohmann/json.hpp>

#include "utils/config.hpp"

#include <mutex>
#include <string>
#include <atomic>

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

  std::string tessdata_dir{};
  std::string tessdata_dir_confirmation_msg{};
  bool show_tessdata_dir_confirmation = false;

  bool show_about_win = false;

  int sleep_for = 1;
  int buffer_size = 3;

  pnkd::cfg::config_helper cfg_helper{};

  std::atomic_bool start_watcher = false;
  std::atomic_bool stop_watcher = false;

  bool breach_button_pushed = false;
  std::string breach_button_text = "Breach";
  ImVec4 inactive_button = ImVec4(0.26F, 0.59F, 0.98F, 0.40F); // Default blue
  ImVec4 inactive_button_hovered = ImVec4(0.26F, 0.59F, 0.98F, 1.00F);
  ImVec4 inactive_button_active = ImVec4(0.06F, 0.53F, 0.98F, 1.00F);
  ImVec4 active_button = static_cast<ImVec4>(ImColor{255, 0, 60, 128});         // Red
  ImVec4 active_button_hovered = static_cast<ImVec4>(ImColor{255, 0, 60, 200}); // Red
  ImVec4 active_button_active = static_cast<ImVec4>(ImColor{255, 0, 60});       // Red

  std::mutex mtx{}; // For thread-safe access to the stuff the watcher needs

  auto set_screenshot_dir(std::string const &s) -> void
  {
    std::lock_guard<std::mutex> lg{mtx};
    screenshot_dir = s;
  }

  auto get_screenshot_dir() -> std::string
  {
    std::lock_guard<std::mutex> lg{mtx};
    return screenshot_dir;
  }

  auto set_tessdata_dir(std::string const &s) -> void
  {
    std::lock_guard<std::mutex> lg{mtx};
    tessdata_dir = s;
  }

  auto get_tessdata_dir() -> std::string
  {
    std::lock_guard<std::mutex> lg{mtx};
    return tessdata_dir;
  }

  auto set_sleep_for(int s) -> void
  {
    std::lock_guard<std::mutex> lg{mtx};
    sleep_for = s;
  }

  auto get_sleep_for() -> int
  {
    std::lock_guard<std::mutex> lg{mtx};
    return sleep_for;
  }

  auto set_buffer_size(int s) -> void
  {
    std::lock_guard<std::mutex> lg{mtx};
    buffer_size = s;
  }

  auto get_buffer_size() -> int
  {
    std::lock_guard<std::mutex> lg{mtx};
    return buffer_size;
  }
};

} // namespace pnkd::gui