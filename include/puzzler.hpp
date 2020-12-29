#pragma once

#include <cstdint>
#include <vector>
#include <string>

namespace pnkd
{

class puzzler
{
private:
  std::vector<std::string> const m_grid;
  std::size_t m_grid_width;

  std::vector<std::string> const m_goals;

  std::size_t const m_buffer_size;

  std::size_t m_pos;
  std::vector<std::size_t> m_moves;

public:
  puzzler() = delete;
  puzzler(std::vector<std::string> const &grid, std::vector<std::string> const &goals, std::size_t const buffer_size);
  ~puzzler() = default;

  auto solve() -> std::vector<uint8_t>;

  auto grid_width() const -> std::size_t;
};

auto solve_paths(std::vector<std::string> const &grid, std::vector<std::string> const &goals, std::size_t const buffer_size) -> std::vector<uint8_t>;

} // namespace pnkd
