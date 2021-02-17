#pragma once

#include <atomic>

namespace pnkd::gui
{

auto cyberpunk_theme() -> void;

auto help_marker(const char *desc) -> void;

auto start(std::atomic_bool &watcher_flag) -> void;

} // namespace pnkd::gui
