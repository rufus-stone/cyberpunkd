#include "game/puzzler.hpp"

#include <cmath>
#include <bitset>
#include <numeric>
#include <sstream>
#include <utility>
#include <map>

#include <spdlog/spdlog.h>

#include "game/goal.hpp"
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
  this->m_total_goals = game_state.goals().total();
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

  auto top_picks = std::map<std::size_t, game_state_t>{};

  spdlog::info("Generated {} candidate routes", this->m_candidates.size());
  for (auto const &candidate : this->m_candidates)
  {
    auto gb = std::bitset<goal_t::max_goals>{};

    // Score each candidate - longer goal sequences and fewer moves are better
    auto const goals = candidate.goals();

    for (std::size_t i = 0; i < goals.total(); ++i)
    {
      if (goals[i].m_completed)
      {
        gb.set(i);
      }
    }

    //spdlog::debug("{} - {} of {} goals in {} moves: {}", candidate.id(), candidate.goals().completed(), candidate.goals().total(), candidate.moves_taken(), candidate.route());

    std::size_t goal_combo = static_cast<std::size_t>(gb.to_ulong());

    // Is this the first example of this combo of completed goals?
    if (top_picks.find(goal_combo) == std::end(top_picks))
    {
      top_picks[goal_combo] = candidate;
    } else
    {
      // If not, check if this candidate is better than the previous best
      auto const current_best = top_picks.at(goal_combo);
      std::size_t curr_best_scoring_moves = std::max_element(std::begin(current_best.goals()), std::end(current_best.goals()), [](auto const &lhs, auto const &rhs) { return lhs.moves_taken() < rhs.moves_taken(); })->moves_taken();
      std::size_t candidate_scoring_moves = std::max_element(std::begin(candidate.goals()), std::end(candidate.goals()), [](auto const &lhs, auto const &rhs) { return lhs.moves_taken() < rhs.moves_taken(); })->moves_taken();

      //spdlog::debug("This candidate scored it's {} goal(s) in {} moves: {} (full route: {})", candidate.goals().completed(), candidate_scoring_moves, candidate.route().first_n(candidate_scoring_moves), candidate.route());

      //if (candidate.moves_taken() < current_best.moves_taken())
      if (candidate_scoring_moves < curr_best_scoring_moves)
      {
        top_picks[goal_combo] = candidate;
      }
    }
  }

  spdlog::info("Refined {} candidates to {} best routes:", this->m_candidates.size(), top_picks.size());

  for (auto const &[combo, pick] : top_picks)
  {
    auto const goals = pick.goals();

    auto goal_vec = std::vector<std::string>{};

    for (auto const &goal : goals)
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


    std::size_t scoring_moves = std::max_element(std::begin(pick.goals()), std::end(pick.goals()), [](auto const &lhs, auto const &rhs) { return lhs.moves_taken() < rhs.moves_taken(); })->moves_taken();
    spdlog::info("{} - {} of {} goals ({}) in {} moves: {}", pick.id(), pick.goals().completed(), pick.goals().total(), ss.str(), scoring_moves, pick.route().first_n(scoring_moves));
  }
}

} // namespace pnkd
