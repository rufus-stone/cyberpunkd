#include "game/goal.hpp"

#include <sstream>

#include <spdlog/spdlog.h>

#include "utils/string_utils.hpp"

namespace pnkd
{

////////////////////////////////////////////////////////////////
// goal_list_t
////////////////////////////////////////////////////////////////

goal_list_t::goal_list_t(std::vector<std::vector<std::string>> const &vec_of_goal_vecs)
{
  std::size_t goal_num = 0;

  for (auto const &goal : vec_of_goal_vecs)
  {
    if (goal.empty())
    {
      continue;
    }

    auto this_goal = std::queue<std::string>{};

    auto goal_str = std::string{};
    goal_str.reserve((goal.size() * 3) - 1);

    for (auto const &segment : goal)
    {
      this_goal.push(segment);
      goal_str += " " + segment;
    }

    goal_str = pnkd::strip(goal_str);

    this->emplace_back(pnkd::goal_t{this_goal, goal_str, goal_num++});
  }

  this->init();
}

auto goal_list_t::total() const -> std::size_t
{
  return this->m_num_goals;
}

auto goal_list_t::completed() const -> std::size_t
{
  return this->m_goals_completed;
}

auto goal_list_t::remaining() const -> std::size_t
{
  return this->m_goals_remaining;
}

auto goal_list_t::complete_one() -> void
{
  this->m_goals_completed++;
  this->m_goals_remaining--;
}

auto goal_list_t::fail_one() -> void
{
  this->m_goals_remaining--;
}

auto goal_list_t::init() -> void
{
  this->m_num_goals = this->size();
  this->m_goals_remaining = this->m_num_goals;
}


////////////////////////////////////////////////////////////////
// goal_t
////////////////////////////////////////////////////////////////

auto goal_t::empty() const -> bool
{
  return this->m_goal.empty();
}

auto goal_t::front() const -> std::string const &
{
  return this->m_goal.front();
}

auto goal_t::front() -> std::string &
{
  return this->m_goal.front();
}

auto goal_t::pop() -> void
{
  this->m_goal.pop();
}

auto goal_t::size() const -> std::size_t
{
  return this->m_goal.size();
}

auto goal_t::seq_len() const -> std::size_t
{
  return this->m_length;
}

auto goal_t::str() const -> std::string
{
  return this->m_str;
}

auto goal_t::moves_taken() const -> std::size_t
{
  return this->m_moves_taken;
}

auto goal_t::completed_in(std::size_t const moves_taken) -> void
{
  this->m_completed = true;
  this->m_moves_taken = moves_taken;
}

auto goal_t::num() const -> std::size_t
{
  return this->m_num;
}

} // namespace pnkd
