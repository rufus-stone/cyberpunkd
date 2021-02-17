#pragma once

#include <string>
#include <optional>

#include <nlohmann/json.hpp>

namespace pnkd::json
{
using json = nlohmann::json;

inline auto string_optional(json const &j, std::string const &key) -> std::optional<std::string>
{
  if (j.contains(key))
  {
    if (j[key].is_string())
    {
      return j[key].get<std::string>();
    } else
    {
      return std::nullopt;
    }
  } else
  {
    return std::nullopt;
  }
}

inline auto int_optional(json const &j, std::string const &key) -> std::optional<int>
{
  if (j.contains(key))
  {
    if (j[key].is_number_integer())
    {
      return j[key].get<int>();
    } else
    {
      return std::nullopt;
    }
  } else
  {
    return std::nullopt;
  }
}

} // namespace pnkd::json
