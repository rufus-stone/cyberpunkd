#include "core/notifier.hpp"

#include <string>
#include <vector>
#include <sstream>

#include <spdlog/spdlog.h>

#include "utils/string_utils.hpp"

namespace pnkd
{

auto show_solutions(std::map<std::size_t, game_state_t> const &solutions) -> void
{
  for (auto const &[combo, solution] : solutions)
  {
    auto goal_vec = std::vector<std::string>{};

    for (auto const &goal : solution.goals())
    {
      if (goal.m_completed)
      {
        std::stringstream ss;
        ss << goal.m_num << ": " << goal.str();
        goal_vec.push_back(ss.str());
      }
    }

    std::stringstream ss;
    if (goal_vec.size() == 1)
    {
      ss << "[" << goal_vec[0] << "]";
    } else
    {
      ss << "[" << goal_vec[0];
      for (std::size_t i = 1; i < goal_vec.size(); ++i)
      {
        ss << ", " << goal_vec[i];
      }

      ss << "]";
    }

    std::size_t scoring_moves = std::max_element(std::begin(solution.goals()), std::end(solution.goals()), [](auto const &lhs, auto const &rhs) { return lhs.moves_taken() < rhs.moves_taken(); })->moves_taken();

    //spdlog::info("{:2d}: {} - {} of {} goals ({}) in {} moves:", combo, solution.id(), solution.goals().completed(), solution.goals().total(), ss.str(), scoring_moves);
    spdlog::info("Solve {} of {} target sequences ({}) in {} moves:", solution.goals().completed(), solution.goals().total(), ss.str(), scoring_moves);
    spdlog::info("    {}\n", solution.route().first_n(scoring_moves));

    /*
    auto grid_copy = solution.grid();

    auto scoring_route = solution.route().first_n(scoring_moves);
    std::size_t n = 1;
    for (auto const &move : scoring_route)
    {
      grid_copy[move] = fmt::format("{:2d}", n++);
    }

    spdlog::info("\n\n{}\n", pnkd::grid_to_string(grid_copy));
    */
  }
}

} // namespace pnkd
