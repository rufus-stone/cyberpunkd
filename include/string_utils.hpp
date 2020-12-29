#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace pnkd
{

static auto const whitespace = std::string{" \n\r\t\f\v"};

auto lstrip(std::string const &input) -> std::string;
auto rstrip(std::string const &input) -> std::string;
auto strip(std::string const &input) -> std::string;

auto split(std::string const &input, std::string const &delimiters = " ") -> std::vector<std::string>;

} // namespace pnkd
