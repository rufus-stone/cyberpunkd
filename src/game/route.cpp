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

} // namespace pnkd
