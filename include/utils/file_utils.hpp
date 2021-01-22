#pragma once

#include <filesystem>

#include <opencv2/opencv.hpp>

namespace pnkd
{

// Generic
auto is_valid_folder(std::filesystem::path const &path) -> bool;

// Screenshots
auto has_screenshots_present(std::filesystem::path const &path) -> bool;
auto get_path_to_latest_screenshot(std::filesystem::path const &dir) -> std::filesystem::path;
auto get_latest_screenshot(std::filesystem::path const &path) -> cv::Mat;
auto get_all_screenshots(std::filesystem::path const &path) -> std::vector<cv::Mat>;

// Tesseract
auto contains_tessdata(std::filesystem::path const &dir) -> bool;

} // namespace pnkd