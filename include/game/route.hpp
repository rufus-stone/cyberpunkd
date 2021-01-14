#pragma once

#include <string>
#include <vector>

#include <spdlog/fmt/ostr.h> // must be included for printing this type with spdlog

namespace pnkd
{

struct route_t : public std::vector<std::size_t>
{
  auto str() const -> std::string;

  template<typename OStream>
  friend OStream &operator<<(OStream &os, route_t const &r)
  {
    return os << r.str();
  }
};

} // namespace pnkd
