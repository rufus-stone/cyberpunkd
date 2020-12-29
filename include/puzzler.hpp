#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <queue>

namespace pnkd
{

class puzzler
{

  using goal_list_t = std::vector<std::queue<std::string>>;

private:
  std::vector<std::string> const m_grid;
  std::size_t const m_grid_size;
  std::size_t m_grid_width;

  std::vector<std::queue<std::string>> const m_goals;

  std::size_t const m_buffer_size;

  std::size_t m_pos;
  std::vector<std::size_t> m_moves;

public:
  puzzler() = delete;
  puzzler(std::vector<std::string> const &grid, goal_list_t const &goals, std::size_t const buffer_size);
  ~puzzler() = default;

  auto grid() const -> std::vector<std::string> const &;
  auto grid_width() const -> std::size_t;
  auto goals() const -> std::vector<std::string> const &;
  auto num_goals() const -> std::size_t;
  auto buffer_size() const -> std::size_t;

  auto solve() const -> std::vector<std::size_t>;
};

} // namespace pnkd
