#include "utils/string_utils.hpp"

#include <algorithm>
#include <sstream>

#include <spdlog/spdlog.h>

namespace pnkd
{

auto lstrip(std::string const &input) -> std::string
{
  std::size_t const pos = input.find_first_not_of(whitespace);
  return (pos == std::string::npos) ? "" : input.substr(pos);
}

auto rstrip(std::string const &input) -> std::string
{
  std::size_t const pos = input.find_last_not_of(whitespace);

  return (pos == std::string::npos) ? "" : input.substr(0, pos + 1);
}

auto strip(std::string const &input) -> std::string
{
  return pnkd::rstrip(pnkd::lstrip(input));
}

auto split(std::string const &input, std::string const &delimiters) -> std::vector<std::string>
{
  // Divide the input up into segments by splitting around the delimiter
  auto segments = std::vector<std::string>{};

  auto start = std::begin(input);
  auto pos = std::begin(input);

  while (pos != std::end(input))
  {
    pos = std::find_first_of(start, std::end(input), std::begin(delimiters), std::end(delimiters));

    segments.emplace_back(start, pos);

    start = pos + 1;
  }

  return segments;
}


auto grid_to_string(std::vector<std::string> const &input) -> std::string
{
  // What is the grid width?
  auto const grid_width = static_cast<std::size_t>(std::sqrt(input.size()));

  // We expect grids that are perfect squares
  if (grid_width * grid_width != input.size())
  {
    spdlog::error("Invalid grid size: {}", input.size());
    return std::string{};
  } else
  {
    std::stringstream ss;

    for (std::size_t i = 0; i < input.size(); ++i)
    {
      ss << input[i] << ((i + 1) % grid_width == 0 ? "\n" : " ");
    }

    return rstrip(ss.str());
  }
}


auto goal_list_to_string(goal_list_t const &input) -> std::string
{
  std::stringstream ss;
  for (auto const &goal : input)
  {
    ss << goal.str() << '\n';
  }

  return strip(ss.str());
}

} // namespace pnkd
