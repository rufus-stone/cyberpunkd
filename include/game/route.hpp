#pragma once

#include <string>
#include <vector>

#include <spdlog/fmt/ostr.h> // must be included for printing this type with spdlog

namespace pnkd
{

struct route_t : public std::vector<std::size_t>
{
  [[nodiscard]] auto str() const -> std::string;

  [[nodiscard]] auto first_n(std::size_t const n) const -> route_t;

  template<typename OStream>
  friend auto operator<<(OStream &os, route_t const &r) -> OStream &
  {
    return os << r.str();
  }
};

} // namespace pnkd
