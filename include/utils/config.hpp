#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include <spdlog/spdlog.h>
#include <opencv2/opencv.hpp>
#include <nlohmann/json.hpp>

#include "sago/platform_folders.h"

namespace pnkd::cfg
{

using json = nlohmann::json;

using namespace std::string_view_literals;

// A default config
constexpr auto default_config = R"###({"screenshot_dir":null, "tessdata_dir":"tessdata", "buffer_size":3, "sleep_for":1})###"sv;

/*
Config JSON fields in use (name / type / purpose):

screenshot_dir / string / path to directory containing game screenshots
tessdata_dir / string / path to directory containing tessdata for OCR
buffer_size / integer / size of in-game cyberware OS (basically how many moves we can make)
sleep_for / integer / how many seconds to wait between checking the screenshots folder for new images
*/

class config_helper
{
private:
  std::filesystem::path m_config_path;
  json m_json;

public:
  config_helper();
  explicit config_helper(std::string const &config_path);

  auto config_path() const -> std::filesystem::path;
  auto cfg_json() -> json &;

  auto load_config() -> json;
  auto write_config(json const &j) -> bool;


  static auto locate_config(std::filesystem::path const &custom_path = std::filesystem::path{}) -> std::filesystem::path
  {
    auto config_path = std::filesystem::path{};

    // Are we using a custom config location?
    if (!custom_path.empty())
    {
      config_path = custom_path;
    } else
    {
      // Try to find the default location for a cyberpunkd config file for this platform
      try
      {
        config_path = sago::getConfigHome(); // ~/.config on Linux,
      } catch (...)
      {
        spdlog::error("Failed to find config location!");
        return std::filesystem::path{};
      }

      config_path = config_path / "cyberpunkd" / "config.json";
    }

    spdlog::info("Looking for config file at: {}", config_path.string());

    // Check if the path exists
    if (!std::filesystem::exists(config_path))
    {
      spdlog::warn("Cannot find config file at: {} ... Attempting to create one...", config_path.string());

      // Create a cyberpunkd directory if it does not already exist
      try
      {
        std::filesystem::create_directories(config_path.parent_path());
      } catch (std::filesystem::filesystem_error const &e)
      {
        spdlog::error(e.what());
        return std::filesystem::path{};
      }

      // Write the default config to file
      auto const default_config_json = json::parse(default_config);
      auto config_file = std::ofstream{config_path};
      config_file << default_config_json.dump(2);
      config_file.close();
    }

    return config_path;
  }
};

} // namespace pnkd::cfg