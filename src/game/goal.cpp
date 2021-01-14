#include "game/goal.hpp"

#include <spdlog/spdlog.h>

namespace pnkd
{

auto goal_list_t::update() -> void
{
  for (auto const &goal : *this)
  {
    if (goal.m_completed)
    {
      ++this->m_goals_completed;
      spdlog::info("Completed {} goals so far...", this->m_goals_completed);
    }
  }

  if (this->m_goals_completed == this->m_num_goals)
  {
    spdlog::info("That was the last of 'em!");
    this->m_completed = true;
  }
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

} // namespace pnkd
