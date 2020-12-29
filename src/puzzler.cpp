#include "puzzler.hpp"

#include <cmath>

#include <spdlog/spdlog.h>

#include "prng.hpp"

namespace pnkd
{

puzzler::puzzler(std::vector<std::string> const &grid, std::vector<std::string> const &goals, std::size_t const buffer_size) : m_grid(grid), m_goals(goals), m_buffer_size(buffer_size)
{
  std::size_t const grid_size = grid.size();
  std::size_t const num_goals = goals.size();

  // We expect grids that are 5x5 (25) or 6x6 (36) (or 7x7??)
  if (grid_size != 25 && grid_size != 36)
  {
    spdlog::error("[❗] Invalid grid size: {}", grid_size);
  }

  std::size_t const grid_width = static_cast<std::size_t>(std::sqrt(grid_size));
  this->m_grid_width = grid_width;

  spdlog::info("Created puzzler for {}x{} grid with {} goals and a buffer size of {}", grid_width, grid_width, num_goals, buffer_size);
}

auto puzzler::grid_width() const -> std::size_t
{
  return this->m_grid.size();
}

auto puzzler::solve() -> std::vector<uint8_t>
{
  auto output = std::vector<uint8_t>{};

  //auto path = std::vector<uint8_t>{};

  // Pick a random starting position on the top row
  std::size_t pos = pnkd::prng::number_between<std::size_t>(0, this->m_grid_width - 1);

  spdlog::info("Starting position {} ({})", pos, this->m_grid[pos]);
  // We can only take buffer_size moves
  for (std::size_t i = 0; i < this->m_buffer_size; ++i)
  {
    // Make a move
  }

  return output;
}


auto solve_paths(std::vector<std::string> const &grid, std::vector<std::string> const &goals, std::size_t const buffer_size) -> std::vector<uint8_t>
{
  auto output = std::vector<uint8_t>{};

  // Create a puzzler
  auto puzzler = pnkd::puzzler{grid, goals, buffer_size};

  // Solve the puzzle
  return puzzler.solve();
}

} // namespace pnkd
