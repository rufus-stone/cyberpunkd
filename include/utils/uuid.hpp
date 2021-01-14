#pragma once

#include <string>
#include <string_view>


namespace pnkd::prng
{

auto bytes(std::size_t quantity) -> std::string;

} // namespace pnkd::prng


namespace pnkd::hex
{

using namespace std::string_view_literals;

constexpr auto hex_alphabet = "0123456789ABCDEF"sv;

auto encode(std::string_view input) -> std::string;

} // namespace pnkd::hex


namespace pnkd::uuid
{

auto generate() -> std::string;

} // namespace pnkd::uuid
