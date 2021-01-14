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

} // namespace pnkd
