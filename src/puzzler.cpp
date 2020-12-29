#include "puzzler.hpp"

#include <cmath>
#include <bitset>
#include <numeric>
#include <sstream>

#include <spdlog/spdlog.h>

#include "prng.hpp"
#include "string_utils.hpp"

namespace pnkd
{

static constexpr std::size_t max_grid_size = 36;

puzzler::puzzler(std::vector<std::string> const &grid, goal_list_t const &goals, std::size_t const buffer_size) : m_grid(grid), m_grid_size(grid.size()), m_goals(goals), m_buffer_size(buffer_size)
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

  spdlog::debug("Created puzzler for a {}x{} ({}) grid with {} goals and a buffer size of {}", grid_width, grid_width, grid_size, num_goals, buffer_size);
}

auto puzzler::solve() const -> std::vector<std::size_t>
{
  auto path = std::vector<std::size_t>{};

  std::size_t const max_pos = this->m_grid_width - 1;
  std::size_t const max_moves = this->m_buffer_size;

  std::size_t hits = 0;
  std::size_t completions = 0;

  bool vertical = false; // Toggle this back and forth to keep track of whether we're moving in rows or columns

  // Keep a modifiable copy of the goals so we can keep track of wins
  goal_list_t goals = this->m_goals;

  // Also remember how long each goal sequence was so we know if we've begun it when scoring
  auto goal_lengths = std::vector<std::size_t>{};
  for (auto const &goal : goals)
  {
    goal_lengths.push_back(goal.size());
  }

  // Let's use a bitset to keep track of squares we've already moved into
  auto moves = std::bitset<max_grid_size>{};

  // clang-format off
  // Quick lambda to check if all the goals are complete/failed
  auto goals_remain = [&goals]() -> bool
  {
    for (auto const &goal : goals)
    {
      if (!goal.empty())
      {
        return true;
      }
    }

    spdlog::debug("All the goals are either complete or failed!");
    return false;
  };

  // Lambda to convert a grid index to an x / y position
  auto x_y_at_pos = [this](std::size_t const p) -> std::pair<std::size_t, std::size_t>
  {
    std::size_t const col = p % this->m_grid_width;
    std::size_t const row = p / this->m_grid_width;
    //spdlog::debug("{} -> {},{}", p, col, row);

    return std::make_pair(col, row);
  };

  // Lambda to convert an x / y position to a grid index
  auto pos_at_x_y = [this](std::size_t const col, std::size_t const row) -> std::size_t
  {
    std::size_t p = (this->m_grid_width * row) + col;
    //spdlog::debug("{},{} -> {}", col, row, p);
    return p;
  };

  // Lambda to check if the proposed next move is valid
  auto is_valid_move = [this, &moves](std::size_t const pos) -> bool
  {
    // If the bit at index pos is set, then that means we've already move there, so return false
    if (moves.test(pos))
    {
      //spdlog::debug("Already been here: {} ({})", pos, this->m_grid[pos]);
      return false;
    } else
    {
      //spdlog::debug("{} ({}) looks valid", pos, this->m_grid[pos]);
      return true;
    }
  };

  // Lambda to check if the current square fits the next sequence of a goal
  // As soon as we begin a goal sequence, we must finish to score it
  auto score_against_goals = [this, &goals, &hits, &completions, &goal_lengths](std::size_t const pos) -> std::size_t
  {
    std::string const s = this->m_grid[pos];
    spdlog::debug(s);

    std::size_t points = 0;

    for (std::size_t i = 0; i < goals.size(); ++i)
    {
      // Only bother with goals we haven't finished yet
      if (!goals[i].empty())
      {
        if (s == goals[i].front())
        {
          spdlog::debug("{} is the next sequence in the queue!", s);
          goals[i].pop();
          ++points;

          // Was that the last sequence? If so, mark it as complete
          // TODO: Score later sequences higher
          if (goals[i].empty())
          {
            spdlog::debug("{} was the last sequence in the queue! Nice!", s);
            ++completions;
          }
        } else
        {
          // If this square DIDN'T match the goal sequence, check that we haven't already started it, otherwise it's a bust
          if (goals[i].size() != goal_lengths[i])
          {
            spdlog::debug("Looks like we had already begun goal sequence {}! Doh!", i);

            // Empty the failed goal out so we don't keep checking it
            while (!goals[i].empty())
            {
              goals[i].pop();
            }
          }
        }
        
      }
    }

    return points;
  };
  // clang-format on

  std::size_t pos = 0; // Set this to 0 so that our first move is looking in the top row

  // Keep making moves until we run out of moves or fulfill (or fail!) all the goals
  while (moves.count() < max_moves && goals_remain())
  {
    spdlog::debug("-------------------------");
    // Where are we currently?
    auto const [col, row] = x_y_at_pos(pos);

    // Pick a next move
    auto choice = pnkd::prng::number_between<std::size_t>(0, max_pos);

    if (vertical)
    {
      pos = pos_at_x_y(col, choice);
    } else
    {
      pos = pos_at_x_y(choice, row);
    }

    if (is_valid_move(pos))
    {
      vertical = !vertical; // Toggle the vertical direction flag
      moves.set(pos);       // Mark this position as moved into
      //spdlog::debug("Moved to position {} ({})", pos, this->m_grid[pos]);
      //spdlog::debug("Taken {} moves of {}", moves.count(), max_moves);
      spdlog::debug(moves.to_string());

      // Now check if it scores us anything
      std::size_t points = score_against_goals(pos);
      if (points > 0)
      {
        spdlog::debug("That move scored us {} points!", points);
        hits += points;
      } else
      {
        spdlog::debug("That move scored us nothing...");
      }

      // Update the move history with the move we just took
      path.push_back(pos);
    }
  }

  auto path_string = std::string{};
  for (std::size_t const move : path)
  {
    auto const [col, row] = x_y_at_pos(move);
    std::stringstream ss;
    ss << "(" << row << "," << col << ") ";
    path_string += ss.str();
  }

  path_string = pnkd::strip(path_string);

  spdlog::info("The path we took was: {}", path_string);
  spdlog::info("It scored us {} points and {} completions in {} moves", hits, completions, moves.count());

  return path;
}

} // namespace pnkd
