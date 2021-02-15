#pragma once

#include <utility>
#include <string>
#include <vector>
#include <bitset>
#include <queue>
#include <optional>

#include "game/point.hpp"
#include "game/goal.hpp"
#include "game/route.hpp"

#include "utils/uuid.hpp"

namespace pnkd
{

class game_state_t
{
  static constexpr std::size_t max_grid_size = 36;
  static constexpr std::size_t max_goals = 5;

  using grid_t = std::vector<std::string>;
  using move_history_t = std::bitset<max_grid_size>;

private:
  grid_t m_grid;
  std::size_t m_grid_size{};
  std::size_t m_grid_width{};

  goal_list_t m_goal_list;

  std::size_t m_buffer_size{};

  point_t m_pos;
  bool m_direction{};

  move_history_t m_move_history;
  route_t m_route;

  bool m_complete = true;

  std::string m_id = pnkd::uuid::generate();
  std::string m_parent_id;

public:
  game_state_t() = default;
  game_state_t(grid_t const &grid, goal_list_t const &goals, std::size_t const buffer_size);
  game_state_t(grid_t const &grid, goal_list_t const &goals, std::size_t const buffer_size, point_t const &pos, bool const direction, move_history_t const &move_history, route_t const &route);

  auto set_parent_id(std::string const &id) -> void;

  [[nodiscard]] auto pos() const -> point_t const &;
  [[nodiscard]] auto route() const -> route_t const &;
  [[nodiscard]] auto grid() const -> grid_t const &;
  [[nodiscard]] auto goals() const -> goal_list_t const &;
  [[nodiscard]] auto moves_taken() const -> std::size_t;
  [[nodiscard]] auto buffer_size() const -> std::size_t;
  [[nodiscard]] auto id() const -> std::string const &;
  [[nodiscard]] auto parent_id() const -> std::string const &;

  [[nodiscard]] auto is_valid_move(std::size_t const pos) const -> bool;
  [[nodiscard]] auto list_all_valid_moves() const -> std::vector<std::size_t>;
  [[nodiscard]] auto make_move(std::size_t const move) const -> std::optional<game_state_t>;
  [[nodiscard]] auto score_goals(std::size_t const move) const -> goal_list_t;
};


} // namespace pnkd
