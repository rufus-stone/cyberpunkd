#include "file_utils.hpp"

#include <spdlog/spdlog.h>
#include <opencv2/opencv.hpp>

namespace pnkd
{

auto is_valid_screenshot_folder(std::filesystem::path const &dir) -> bool
{
  spdlog::debug("Checking screenshot folder: {}", dir.string());

  // Does the path exist?
  if (!std::filesystem::exists(dir))
  {
    spdlog::error("[❗] Couldn't find path: {}", dir.string());
    return false;
  }

  // Is it a directory?
  if (!std::filesystem::is_directory(dir))
  {
    spdlog::error("[❗] Path is not a directory: {}", dir.string());
    return false;
  }

  spdlog::debug("[👍] Screenshot folder looks valid");

  return true;
}

auto has_screenshots_present(std::filesystem::path const &dir) -> bool
{
  return !std::filesystem::is_empty(dir);
}

auto get_path_to_latest_screenshot(std::filesystem::path const &dir) -> std::filesystem::path
{

  // Lambda to check for image file extensions
  auto const has_image_extension = [](std::filesystem::path const &file) -> bool {
    static constexpr auto image_extensions = std::array{".jpg", ".jpeg", ".png"};

    std::string const extension = file.extension().string();
    return (!extension.empty() && std::find(std::begin(image_extensions), std::end(image_extensions), extension) != std::end(image_extensions));
  };

  // List all the regular files in the directory
  auto files = std::vector<std::filesystem::path>{};
  for (auto const &entry : std::filesystem::directory_iterator(dir))
  {
    if (std::filesystem::is_regular_file(entry) && has_image_extension(entry))
    {
      files.push_back(entry.path());
    }
  }

  // Did we find any files?
  if (files.empty())
  {
    spdlog::error("[❗] No files found: {}", dir.string());
    return std::filesystem::path{};
  }

  // Sort the files by name descending in order to find the most recent log file (CS:GO log files include the date at the start of the filename)
  std::sort(std::begin(files), std::end(files), std::greater<>());
  spdlog::debug("Most recent image file: {} ", files[0].string());

  return files[0];
}

auto get_latest_screenshot(std::filesystem::path const &dir) -> cv::Mat
{
  // Did the user pass a valid path to a folder?
  if (!pnkd::is_valid_screenshot_folder(dir))
  {
    spdlog::error("[❗] Specified folder path appears invalid! : {}", dir.string());
    return cv::Mat{};
  }

  // Is the folder empty?
  if (!pnkd::has_screenshots_present(dir))
  {
    spdlog::warn("[⚠️] Screenshot folder is empty : {}", dir.string());
    return cv::Mat{};
  }

  // Get the path to the latest screenshot
  auto const img_path = pnkd::get_path_to_latest_screenshot(dir);

  if (!img_path.empty())
  {
    spdlog::debug("Loading image file...");
    return cv::imread(img_path.string(), cv::IMREAD_COLOR);
  } else
  {
    spdlog::error("[❗] Failed to find screenshot file!");
    return cv::Mat{};
  }
}

} // namespace pnkd
