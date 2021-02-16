#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include <opencv2/opencv.hpp>
#include <nlohmann/json.hpp>

namespace pnkd::cfg
{

using json = nlohmann::json;

auto locate_config(std::filesystem::path const &custom_path = std::filesystem::path{}) -> std::filesystem::path;
auto load_config(std::filesystem::path const &custom_path = std::filesystem::path{}) -> json;
auto write_config(json config_json, std::filesystem::path const &custom_path = std::filesystem::path{}) -> bool;

} // namespace pnkd::cfg