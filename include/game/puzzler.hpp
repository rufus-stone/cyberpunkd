#pragma once

#include <queue>

#include "game/state.hpp"

namespace pnkd
{

class puzzler
{

private:
  std::queue<game_state_t> m_game_states;
  bool should_continue = true;

public:
  puzzler() = delete;
  puzzler(game_state_t const &game_state);
  ~puzzler() = default;

  auto play_one() -> std::queue<game_state_t>;
  auto solve() -> void;
};

} // namespace pnkd
