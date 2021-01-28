#pragma once

#include <map>

#include "game/state.hpp"

namespace pnkd
{

auto show_solutions(std::map<std::size_t, game_state_t> const &solutions) -> void;

} // namespace pnkd
