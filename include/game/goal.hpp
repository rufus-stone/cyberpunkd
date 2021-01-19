#pragma once

#include <string>
#include <vector>
#include <bitset>
#include <queue>

namespace pnkd
{

struct goal_t;

class goal_list_t : public std::vector<goal_t>
{
private:
  std::size_t m_num_goals = this->size();
  std::size_t m_goals_completed = 0;
  std::size_t m_goals_remaining = m_num_goals;

public:
  auto total() const -> std::size_t;
  auto completed() const -> std::size_t;
  auto remaining() const -> std::size_t;

  auto complete_one() -> void;
  auto fail_one() -> void;

  auto init() -> void;
};

struct goal_t
{
  static constexpr std::size_t max_goals = 5;

  std::queue<std::string> m_goal = std::queue<std::string>{};
  std::string m_str = "";
  std::size_t m_num = 0;
  std::size_t m_length = m_goal.size();
  std::bitset<max_goals> m_progress = std::bitset<max_goals>{};
  bool m_completed = false;
  bool m_failed = false;

  auto empty() const -> bool;
  auto front() const -> std::string const &;
  auto front() -> std::string &;
  auto pop() -> void;
  auto size() const -> std::size_t;
  auto seq_len() const -> std::size_t;
  auto str() const -> std::string;
};

} // namespace pnkd
