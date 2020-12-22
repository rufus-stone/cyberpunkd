#pragma once

#include <filesystem>

#include <opencv2/opencv.hpp>

namespace pnkd
{

auto is_valid_screenshot_folder(std::filesystem::path const &path) -> bool;
auto has_screenshots_present(std::filesystem::path const &path) -> bool;
auto get_path_to_latest_screenshot(std::filesystem::path const &dir) -> std::filesystem::path;
auto get_latest_screenshot(std::filesystem::path const &path) -> cv::Mat;

} // namespace pnkd