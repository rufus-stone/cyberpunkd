#pragma once

#include <string>

namespace pnkd
{

static auto const whitespace = std::string{" \n\r\t\f\v"};

std::string lstrip(std::string const &input)
{
  std::size_t const pos = input.find_first_not_of(whitespace);
  return (pos == std::string::npos) ? "" : input.substr(pos);
}

std::string rstrip(std::string const &input)
{
  std::size_t const pos = input.find_last_not_of(whitespace);

  return (pos == std::string::npos) ? "" : input.substr(0, pos + 1);
}

std::string strip(std::string const &input)
{
  return rstrip(lstrip(input));
}

} // namespace pnkd
