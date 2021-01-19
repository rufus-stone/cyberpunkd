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

  int iters = 0;

  while (!this->m_game_states.empty() && this->should_continue)
  {
    spdlog::debug("\n----------------------\nIteration {}\n----------------------", iters++);

    // Get the state at the front of the queue
    auto game_state = this->m_game_states.front();
    spdlog::debug("Evaluating state id {} (parent {})", game_state.id(), game_state.parent_id());

    // Have we taken all our moves in this state?
    if (game_state.moves_taken() >= game_state.buffer_size())
    {
      // Add it to the list of candidates if we've scored any goals
      if (game_state.goals().completed() > 0)
      {
        this->m_candidates.push_back(game_state);
      }

    } else
    {
      // List all the valid moves we could make
      std::vector<std::size_t> const valid_moves = game_state.list_all_valid_moves();
      spdlog::debug("Found {} valid moves from {} @ {}", valid_moves.size(), game_state.grid()[game_state.pos().pos()], game_state.pos());

      // Play each move out
      for (std::size_t const move : valid_moves)
      {
        spdlog::debug("++++++++++++++++++++++");
        // Make the move and score the goals
        auto next_game_state = game_state.make_move(move);

        if (next_game_state)
        {
          next_game_state->set_parent_id(game_state.id());
          this->m_game_states.push(next_game_state.value());

          // Was the move worth adding to the list of candidates?
          if (next_game_state->goals().remaining() == 0 && next_game_state->goals().completed() > 0)
          {
            spdlog::debug("Candidate: {} of {} goals in {} moves", next_game_state->goals().completed(), next_game_state->goals().total(), next_game_state->moves_taken());

            this->m_candidates.push_back(next_game_state.value());
          }
        }
      }
    }

    // Pop this state now that we've dealt with it
    this->m_game_states.pop();
  }

  spdlog::info("Generated {} candidate routes", this->m_candidates.size());
  for (auto const &candidate : this->m_candidates)
  {
    // Score each candidate - the fewer moves the better
    // TODO: figure out a good way to score these
    double score = static_cast<double>(candidate.goals().completed()) / static_cast<double>(candidate.moves_taken());

    spdlog::info("{} - ({:.3f}) {} of {} in {} moves: {}", candidate.id(), score, candidate.goals().completed(), candidate.goals().total(), candidate.moves_taken(), candidate.route());
  }
}

} // namespace pnkd
