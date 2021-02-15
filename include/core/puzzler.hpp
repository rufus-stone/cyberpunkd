#pragma once

#include <queue>
#include <vector>
#include <map>

#include "game/state.hpp"

namespace pnkd
{

class puzzler
{

private:
  std::queue<game_state_t> m_game_states;
  std::vector<game_state_t> m_candidates;
  std::size_t m_total_goals;

public:
  puzzler() = delete;
  explicit puzzler(game_state_t const &game_state);

  auto calculate_all_routes() -> void;
  auto pick_best_routes() -> std::map<std::size_t, game_state_t>;
  auto solve() -> std::map<std::size_t, game_state_t>;
};

} // namespace pnkd
