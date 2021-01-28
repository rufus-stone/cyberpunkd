#include "game/state.hpp"

#include <cmath>
#include <numeric>
#include <optional>
#include <spdlog/spdlog.h>

#include "game/point.hpp"
#include "utils/string_utils.hpp"

namespace pnkd
{

auto const is_perfect_square = [](std::size_t const n) {
  // Ignore empty grids
  if (n == 0)
  {
    return false;
  }

  std::size_t const sqrt = std::sqrt(n);

  return (sqrt * sqrt == n);
};


game_state_t::game_state_t(grid_t const &grid, goal_list_t const &goals, std::size_t const buffer_size) : m_grid(grid), m_grid_size(grid.size()), m_goal_list(goals), m_buffer_size(buffer_size), m_pos(point_t{grid.size()}), m_direction(false) //, m_move_history(move_history_t{}), m_route(route_t{})
{
  std::size_t const grid_size = grid.size();

  // We expect grids that are perfect squares
  if (!is_perfect_square(grid_size))
  {
    spdlog::error("Invalid grid size: {}", grid_size);
  }

  this->m_grid_width = static_cast<std::size_t>(std::sqrt(grid_size));

  //spdlog::debug("Created game_state_t for a {}x{} ({}) grid with {} goals and a buffer size of {}", grid_width, grid_width, grid_size, goals.size(), buffer_size);
}


game_state_t::game_state_t(grid_t const &grid, goal_list_t const &goals, std::size_t const buffer_size, point_t const &pos, bool const direction, move_history_t const &move_history, route_t const &route) : m_grid(grid), m_grid_size(grid.size()), m_goal_list(goals), m_buffer_size(buffer_size), m_pos(pos), m_direction(direction), m_move_history(move_history), m_route(route)
{
  std::size_t const grid_size = grid.size();

  // We expect grids that are perfect squares
  if (!is_perfect_square(grid_size))
  {
    spdlog::error("Invalid grid size: {}", grid_size);
  }

  this->m_grid_width = static_cast<std::size_t>(std::sqrt(grid_size));

  //spdlog::debug("Created game_state_t for a {}x{} ({}) grid with {} goals and a buffer size of {}", grid_width, grid_width, grid_size, goals.size(), buffer_size);
}


auto game_state_t::is_valid_move(std::size_t const pos) const -> bool
{
  // If the bit at index pos is set, then that means we've already move there, so return false
  return !this->m_move_history.test(pos);
}


auto game_state_t::list_all_valid_moves() const -> std::vector<std::size_t>
{
  // How many moves have we made already?
  auto const moves_taken = this->moves_taken(); //this->m_move_history.count();

  // Have we already made all our moves?
  if (moves_taken >= this->m_buffer_size)
  {
    spdlog::debug("Can't take any more moves!");
    return std::vector<std::size_t>{};
  }

  auto valid_moves = std::vector<std::size_t>{};

  // Where are we currently?
  auto const [col, row] = this->m_pos.col_row();

  // Check all possible positions in the same row or column
  for (std::size_t i = 0; i < this->m_grid_width; ++i)
  {
    std::size_t next_pos = 0;

    if (this->m_direction)
    {
      //spdlog::info("Moving vertically - col: {} row: {} g_w: {}, ", col, i, this->m_grid_width);
      next_pos = pnkd::point_t::xy_to_pos(col, i, this->m_grid_width);
    } else
    {
      //spdlog::info("Moving horizontally - col: {} row: {} g_w: {}, ", col, i, this->m_grid_width);
      next_pos = pnkd::point_t::xy_to_pos(i, row, this->m_grid_width);
    }

    if (this->is_valid_move(next_pos))
    {
      valid_moves.push_back(next_pos);
    }
  }

  return valid_moves;
}


auto game_state_t::score_goals(std::size_t const move) const -> goal_list_t
{
  auto goal_list = this->m_goal_list;

  std::string const s = this->m_grid[move];

  for (std::size_t i = 0; i < goal_list.size(); ++i)
  {
    // Only bother with goals we haven't finished yet
    if (!goal_list[i].empty())
    {
      if (s == goal_list[i].front())
      {
        spdlog::debug("{} @ {} is the next sequence in the goal '{}'! {} of {} done", s, point_t{move, this->m_grid_size}, goal_list[i].str(), goal_list[i].seq_len() - (goal_list[i].size() - 1), goal_list[i].seq_len());
        goal_list[i].pop();

        // Was that the last sequence? If so, mark it as complete
        if (goal_list[i].empty())
        {
          spdlog::debug("{} was the last sequence in the goal '{}', so it's now complete! Nice!", s, goal_list[i].str());
          //goal_list[i].m_completed = true;
          goal_list[i].completed_in(this->moves_taken() + 1); // +1 because at this point the current move is still being evaluated, so the number returned by this->moves_taken() is 1 behind
          goal_list.complete_one();
        }

        goal_list[i].m_progress.set(i);

      } else
      {
        // If this square DIDN'T match the goal sequence, check that we haven't already started it, otherwise it's a bust
        if (goal_list[i].size() != goal_list[i].seq_len())
        {
          spdlog::debug("Looks like we had already begun goal sequence {} '{}' so have now failed it! Doh!", i, goal_list[i].str());
          goal_list[i].m_failed = true;

          // Empty the failed goal out so we don't keep checking it
          while (!goal_list[i].empty())
          {
            goal_list[i].pop();
          }

          goal_list.fail_one();

          spdlog::debug("{} goals remaining", goal_list.remaining());
        }
      }
    }
  }

  spdlog::debug("After moving to {} @ {}, {} goals remain of {} ({} successfully completed)", s, pnkd::point_t{move, this->m_grid_size}, goal_list.remaining(), goal_list.total(), goal_list.completed());

  return goal_list;
}


auto game_state_t::make_move(std::size_t const move) const -> std::optional<game_state_t>
{
  // Get copies of the current state variables
  auto move_history = this->m_move_history;
  auto route = this->m_route;
  bool direction = this->m_direction;

  // Make the move
  move_history.set(move); // Mark this position as moved into
  route.push_back(move);  // Add the move to the route history
  direction = !direction; // Toggle the vertical direction flag

  // Now check if it progressed any goals
  auto new_goals = this->score_goals(move);

  // Have we failed all the goals?
  if (new_goals.remaining() == 0 && new_goals.completed() == 0)
  {
    spdlog::debug("All goals failed! Route taken: {}", route);

    return std::nullopt;
  }

  // Create a new point_t for the new position
  auto new_pos = point_t{move, this->m_grid_size};

  // Now create a new game state based on the move we just made
  auto new_state = pnkd::game_state_t{this->m_grid, new_goals, this->m_buffer_size, new_pos, direction, move_history, route};

  return new_state;
}


auto game_state_t::pos() const -> point_t const &
{
  return this->m_pos;
}

auto game_state_t::route() const -> route_t const &
{
  return this->m_route;
}

auto game_state_t::grid() const -> grid_t const &
{
  return this->m_grid;
}

auto game_state_t::goals() const -> goal_list_t const &
{
  return this->m_goal_list;
}

auto game_state_t::moves_taken() const -> std::size_t
{
  return this->m_move_history.count();
}

auto game_state_t::buffer_size() const -> std::size_t
{
  return this->m_buffer_size;
}

auto game_state_t::id() const -> std::string const &
{
  return this->m_id;
}

auto game_state_t::parent_id() const -> std::string const &
{
  return this->m_parent_id;
}

auto game_state_t::set_parent_id(std::string const &id) -> void
{
  this->m_parent_id = id;
}

} // namespace pnkd
