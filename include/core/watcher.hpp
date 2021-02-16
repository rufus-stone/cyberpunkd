#pragma once

#include <filesystem>
#include <string>

#include "core/ocr.hpp"

namespace pnkd
{

class watcher
{
private:
  std::string m_screenshots_path;
  ocr_t m_ocr;
  std::size_t m_buffer_size;
  std::size_t m_sleep_for;

  bool m_should_watch = true;
  std::filesystem::path m_previous_image_path{};

public:
  explicit watcher(std::string const &screenshots_path, std::string const &tessdata_dir, std::size_t buffer_size, std::size_t sleep_for);

  auto start() -> bool;
  auto stop() -> void;
};

} // namespace pnkd
