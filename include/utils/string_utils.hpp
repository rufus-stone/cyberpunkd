#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include "game/goal.hpp"

namespace pnkd
{

static auto const whitespace = std::string{" \n\r\t\f\v"};

auto lstrip(std::string const &input) -> std::string;
auto rstrip(std::string const &input) -> std::string;
auto strip(std::string const &input) -> std::string;

auto split(std::string const &input, std::string const &delimiters = " ") -> std::vector<std::string>;

auto grid_to_string(std::vector<std::string> const &input) -> std::string;
auto goal_list_to_string(goal_list_t const &input) -> std::string;

} // namespace pnkd
