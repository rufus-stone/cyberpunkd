#include "game/puzzler.hpp"

#include <cmath>
#include <bitset>
#include <numeric>
#include <sstream>
#include <utility>

#include <spdlog/spdlog.h>

#include "game/goal.hpp"
#include "utils/prng.hpp"
#include "utils/string_utils.hpp"
#include "game/point.hpp"
#include "game/state.hpp"

namespace pnkd
{


puzzler::puzzler(game_state_t const &game_state)
{
  auto q = std::queue<game_state_t>{};
  q.push(game_state);
  this->m_game_states = q;
}


auto puzzler::solve() -> void
{
  auto new_states = std::queue<game_state_t>{};

  int i = 0;

  while (!this->m_game_states.empty() && this->should_continue)
  {
    spdlog::info("\n\n----------------------\nIteration {}\n----------------------\n\n", i++);

    // Get the state at the front of the queue
    auto game_state = this->m_game_states.front();

    // Find all the valid next moves for this game state
    auto const next_moves = game_state.list_all_valid_moves();

    // Make all the valid moves and generate a new list of game states
    auto const next_game_states = game_state.make_all_valid_moves();

    spdlog::debug("Generated {} new game states", next_game_states.size());

    // Check if any of these game states failed/completed all the goals
    for (auto &state : next_game_states)
    {
      auto gs = state.get_goals();

      //spdlog::info("{} done!", gs.m_goals_completed);

      for (auto const &g : gs)
      {
        if (g.m_completed && !g.m_failed)
        {
          //spdlog::info("Goal complete! {}", g.str());
          spdlog::info("Route taken: {}", pnkd::route_to_string(state.get_route()));
        }
      }
    }

    // Add these new game states to the list of states for the next loop
    for (auto &state : next_game_states)
    {
      this->m_game_states.push(state);
    }

    // Now check how well it did

    // Pop this state now that we've dealt with it
    this->m_game_states.pop();
  }
}

} // namespace pnkd
