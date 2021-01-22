#include "game/route.hpp"

#include <sstream>

namespace pnkd
{

auto route_t::str() const -> std::string
{
  auto ss = std::stringstream{};

  if (this->empty())
  {
    return ss.str();
  }

  ss << (*this)[0];

  for (std::size_t i = 1; i < this->size(); ++i)
  {
    ss << " -> " << (*this)[i];
  }

  return ss.str();
}

auto route_t::first_n(std::size_t const n) const -> route_t
{
  if (n >= this->size())
  {
    return *this;
  } else
  {
    auto tmp = route_t{};
    std::copy(std::begin(*this), std::begin(*this) + n, std::back_inserter(tmp));
    return tmp;
  }
}

} // namespace pnkd
