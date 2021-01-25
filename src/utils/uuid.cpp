#include "utils/uuid.hpp"

#include <random>
#include <string>
#include <string_view>


namespace pnkd::prng
{

auto bytes(std::size_t quantity) -> std::string
{
  std::string output;
  output.reserve(quantity);

  std::mt19937 random_engine{std::random_device{}()};

  std::uniform_int_distribution<int> distribution(std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max()); // MSVC doesn't like std::uniform_int_distribution<uint8_t>

  // Generate quantity-worth of random bytes and add to the output string.
  for (std::size_t n = 0; n < quantity; ++n)
  {
    output.push_back(static_cast<char>(distribution(random_engine)));
  }

  return output;
}

} // namespace pnkd::prng


namespace pnkd::hex
{

auto encode(std::string_view input) -> std::string
{
  std::string output;
  output.reserve(input.size() * 2);

  for (auto const &ch : input)
  {
    output.push_back(hex_alphabet[(ch & 0xF0) >> 4]);
    output.push_back(hex_alphabet[(ch & 0x0F)]);
  }

  return output;
}

} // namespace pnkd::hex


namespace pnkd::uuid
{

static constexpr std::size_t uuid_size = 16;

auto generate() -> std::string
{
  auto uuid = pnkd::hex::encode(pnkd::prng::bytes(uuid_size));

  uuid.insert(8, 1, '-');
  uuid.insert(13, 1, '-');
  uuid.insert(18, 1, '-');
  uuid.insert(23, 1, '-');

  return uuid;
}

} // namespace pnkd::uuid
