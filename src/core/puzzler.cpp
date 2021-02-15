#include "core/puzzler.hpp"

#include <cmath>
#include <bitset>
#include <numeric>
#include <sstream>
#include <utility>
#include <map>

#include <spdlog/spdlog.h>

#include "game/goal.hpp"
#include "game/point.hpp"
#include "game/state.hpp"

#include "utils/string_utils.hpp"

namespace pnkd
{


puzzler::puzzler(game_state_t const &game_state)
{
  auto q = std::queue<game_state_t>{};
  q.push(game_state);
  this->m_game_states = q;
  this->m_total_goals = game_state.goals().total();
}


auto puzzler::calculate_all_routes() -> void
{
  int iters = 0;

  // How many possible goal combos are there at most?
  std::size_t const pos_combos = std::pow(2, this->m_total_goals) - 1;

  while (!this->m_game_states.empty())
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
            spdlog::debug("Candidate: {} of {} goal(s) in {} moves", next_game_state->goals().completed(), next_game_state->goals().total(), next_game_state->moves_taken());

            this->m_candidates.push_back(next_game_state.value());
          }
        }
      }
    }

    // Pop this state now that we've dealt with it
    this->m_game_states.pop();

    // Have we found candidate solutions to all the possible combos of target sequences? If so, we can't improve further so break out of the loop
    if (this->m_candidates.size() == pos_combos)
    {
      spdlog::info("All goal combos found!"); // THIS DOESN'T WORK - need to check optimal routes not candidate routes
      break;
    }
  }
}

auto puzzler::pick_best_routes() -> std::map<std::size_t, game_state_t>
{
  auto optimal_solutions = std::map<std::size_t, game_state_t>{};

  spdlog::info("Generated {} candidate route(s)", this->m_candidates.size());
  for (auto const &candidate : this->m_candidates)
  {
    auto gb = std::bitset<goal_t::max_goals>{};

    // Score each candidate - longer goal sequences and fewer moves are better
    auto const &goals = candidate.goals();

    for (std::size_t i = 0; i < goals.total(); ++i)
    {
      if (goals[i].m_completed)
      {
        gb.set(i);
      }
    }

    //spdlog::debug("{} - {} of {} goals in {} moves: {}", candidate.id(), candidate.goals().completed(), candidate.goals().total(), candidate.moves_taken(), candidate.route());

    auto goal_combo = static_cast<std::size_t>(gb.to_ulong());

    // Is this the first example of this combo of completed goals?
    if (optimal_solutions.find(goal_combo) == std::end(optimal_solutions))
    {
      optimal_solutions[goal_combo] = candidate;
    } else
    {
      // If not, check if this candidate is better than the previous best
      auto const current_best = optimal_solutions.at(goal_combo);
      std::size_t curr_best_scoring_moves = std::max_element(std::begin(current_best.goals()), std::end(current_best.goals()), [](auto const &lhs, auto const &rhs) { return lhs.moves_taken() < rhs.moves_taken(); })->moves_taken();
      std::size_t candidate_scoring_moves = std::max_element(std::begin(candidate.goals()), std::end(candidate.goals()), [](auto const &lhs, auto const &rhs) { return lhs.moves_taken() < rhs.moves_taken(); })->moves_taken();

      //spdlog::debug("This candidate scored it's {} goal(s) in {} moves: {} (full route: {})", candidate.goals().completed(), candidate_scoring_moves, candidate.route().first_n(candidate_scoring_moves), candidate.route());

      if (candidate_scoring_moves < curr_best_scoring_moves)
      {
        optimal_solutions[goal_combo] = candidate;
      }
    }
  }

  spdlog::info("Refined {} candidate(s) down to {} optimal solution(s)", this->m_candidates.size(), optimal_solutions.size());

  return optimal_solutions;
}


auto puzzler::solve() -> std::map<std::size_t, game_state_t>
{
  this->calculate_all_routes();

  return this->pick_best_routes();
}

} // namespace pnkd
