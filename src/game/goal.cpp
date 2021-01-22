#include "game/goal.hpp"

#include <spdlog/spdlog.h>

namespace pnkd
{


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

} // namespace pnkd
