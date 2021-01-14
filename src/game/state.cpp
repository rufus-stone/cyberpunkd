#include "game/state.hpp"

#include <numeric>
#include <optional>
#include <spdlog/spdlog.h>

#include "game/point.hpp"
#include "utils/string_utils.hpp"

namespace pnkd
{

////////////////////////////////////////////////////////////////
game_state_t::game_state_t(grid_t const &grid, goal_list_t const &goals, std::size_t const buffer_size) : m_grid(grid), m_grid_size(grid.size()), m_goal_list(goals), m_buffer_size(buffer_size), m_pos(point_t{grid.size()}), m_direction(false), m_move_history(move_history_t{}), m_route(route_t{})
{
  std::size_t const grid_size = grid.size();

  // We expect grids that are 5x5 (25) or 6x6 (36) (or 7x7??)
  if (grid_size != 25 && grid_size != 36)
  {
    spdlog::error("[❗] Invalid grid size: {}", grid_size);
  }

  std::size_t const grid_width = static_cast<std::size_t>(std::sqrt(grid_size));
  this->m_grid_width = grid_width;

  //std::size_t const num_goals = goals.size();
  //spdlog::debug("Created game_state_t for a {}x{} ({}) grid with {} goals and a buffer size of {}", grid_width, grid_width, grid_size, num_goals, buffer_size);
}

game_state_t::game_state_t(grid_t const &grid, goal_list_t const &goals, std::size_t const buffer_size, point_t const &pos, bool const direction, move_history_t const &move_history, route_t const &route) : m_grid(grid), m_grid_size(grid.size()), m_goal_list(goals), m_buffer_size(buffer_size), m_pos(pos), m_direction(direction), m_move_history(move_history), m_route(route)
{
  std::size_t const grid_size = grid.size();

  // We expect grids that are 5x5 (25) or 6x6 (36) (or 7x7??)
  if (grid_size != 25 && grid_size != 36)
  {
    spdlog::error("[❗] Invalid grid size: {}", grid_size);
  }

  std::size_t const grid_width = static_cast<std::size_t>(std::sqrt(grid_size));
  this->m_grid_width = grid_width;

  //std::size_t const num_goals = goals.size();
  //spdlog::debug("Created game_state_t for a {}x{} ({}) grid with {} goals and a buffer size of {}", grid_width, grid_width, grid_size, num_goals, buffer_size);
}


auto game_state_t::get_goals() const -> goal_list_t const &
{
  return this->m_goal_list;
}


auto game_state_t::is_valid_move(std::size_t const pos) const -> bool
{
  // If the bit at index pos is set, then that means we've already move there, so return false
  if (this->m_move_history.test(pos))
  {
    return false;
  } else
  {
    return true;
  }
}


auto game_state_t::list_all_valid_moves() const -> std::vector<std::size_t>
{
  // How many moves have we made already?
  auto moves_taken = this->m_move_history.count();
  //spdlog::info("Already taken {} moves", moves_taken);

  if (moves_taken > this->m_buffer_size)
  {
    spdlog::error("Can't take any more moves!");
    return std::vector<std::size_t>{};
  }

  //spdlog::info("Finding all valid moves...");
  auto valid_moves = std::vector<std::size_t>{};

  // Where are we currently?
  auto const [col, row] = this->m_pos.col_row();
  //spdlog::info("Current position: {}", this->m_pos);

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
  auto goals = this->m_goal_list;

  std::string const s = this->m_grid[move];

  for (std::size_t i = 0; i < goals.size(); ++i)
  {
    // Only bother with goals we haven't finished yet
    if (!goals[i].empty())
    {
      if (s == goals[i].front())
      {
        spdlog::debug("{} at {} is the next sequence in the goal '{}'!", s, point_t{move, this->m_grid_size}, goals[i].str());
        goals[i].pop();

        // Was that the last sequence? If so, mark it as complete
        // TODO: Score later sequences higher
        if (goals[i].empty())
        {
          spdlog::info("{} was the last sequence in the goal '{}', so it's now complete! Nice!", s, goals[i].str());
          goals[i].m_completed = true;
          goals.m_goals_completed++;
        }

        goals[i].m_progress.set(i);

      } else
      {
        // If this square DIDN'T match the goal sequence, check that we haven't already started it, otherwise it's a bust
        if (goals[i].size() != goals[i].seq_len())
        {
          spdlog::debug("Looks like we had already begun goal sequence {} '{}' so have now failed it! Doh!", i, goals[i].str());
          goals[i].m_failed = true;

          // Empty the failed goal out so we don't keep checking it
          while (!goals[i].empty())
          {
            goals[i].pop();
          }

          goals.m_goals_remaining--;

          spdlog::error("goals.m_goals_remaining == {}", goals.m_goals_remaining);
        }
      }
    }
  }

  spdlog::info("After moving to {} @ {}, {} goals remain of {} ({} successfully completed)", s, pnkd::point_t{move, this->m_grid_size}, goals.m_goals_remaining, goals.m_num_goals, goals.m_goals_completed);
  // Goal counting isn't working properly... need to figure out why

  if (goals.m_goals_remaining == 0 && goals.m_goals_completed == 0)
  {
    return pnkd::goal_list_t{};
  } else
  {
    return goals;
  }
}

auto game_state_t::make_all_valid_moves() const -> std::vector<game_state_t>
{
  auto next_states = std::vector<game_state_t>{};

  // List all the valid moves we could make
  std::vector<std::size_t> const valid_moves = this->list_all_valid_moves();
  spdlog::debug("Found {} valid moves from {} @ {}", valid_moves.size(), this->m_grid[this->m_pos.pos()], this->m_pos);

  // Play each move out
  for (auto const move : valid_moves)
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

    // Have we already complete/failed all the goals? If so, stop
    if (new_goals.empty())
    {
      spdlog::info("Route taken: {}", route); //pnkd::route_to_string(route));

      return next_states;
    }

    // Create a new point_t for the new position
    auto new_pos = point_t{move, this->m_grid_size};

    // Now create a new game state based on the move we just made
    auto new_state = pnkd::game_state_t{this->m_grid, new_goals, this->m_buffer_size, new_pos, direction, move_history, route};

    next_states.push_back(new_state);
  }

  return next_states;
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

  // Have we already complete/failed all the goals? If so, stop
  if (new_goals.empty())
  {
    spdlog::info("Route taken: {}", route);

    return std::nullopt;
  }

  // Create a new point_t for the new position
  auto new_pos = point_t{move, this->m_grid_size};

  // Now create a new game state based on the move we just made
  auto new_state = pnkd::game_state_t{this->m_grid, new_goals, this->m_buffer_size, new_pos, direction, move_history, route};

  return new_state;
}


auto game_state_t::get_pos() const -> point_t
{
  return this->m_pos;
}

auto game_state_t::get_route() const -> route_t
{
  return this->m_route;
}

} // namespace pnkd
