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
  std::string m_tessdata_path;
  ocr_t m_ocr;
  std::size_t m_buffer_size;
  std::size_t m_sleep_for;

  bool m_should_watch = true;
  std::filesystem::path m_previous_image_path{};

public:
  explicit watcher(std::string const &screenshots_path, std::string const &tessdata_path, std::size_t buffer_size, std::size_t sleep_for);

  auto do_once() -> void;
  auto start() -> bool;
  auto stop() -> void;

  auto set_screenshots_path(std::string const &s) -> void;
  auto set_tessdata_path(std::string const &s) -> void;
  auto set_buffer_size(std::size_t s) -> void;
  auto set_sleep_for(std::size_t s) -> void;
  auto forget_previous_image() -> void;
};

} // namespace pnkd
