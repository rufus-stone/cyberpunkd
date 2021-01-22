#pragma once

#include <utility>
#include <string>

#include <spdlog/spdlog.h>

#include <spdlog/fmt/ostr.h> // must be included for printing this type with spdlog

namespace pnkd
{

class point_t
{
private:
  std::size_t m_pos;
  std::size_t m_grid_size;
  std::size_t m_grid_width;
  bool m_valid;

public:
  point_t();
  point_t(std::size_t const grid_size);
  point_t(std::size_t const pos, std::size_t const grid_size);
  ~point_t() = default;

  auto pos() const -> std::size_t;
  auto x() const -> std::size_t;
  auto y() const -> std::size_t;
  auto row() const -> std::size_t;
  auto col() const -> std::size_t;
  auto x_y() const -> std::pair<std::size_t, std::size_t>;
  auto col_row() const -> std::pair<std::size_t, std::size_t>;
  auto str() const -> std::string;

  operator bool() const
  {
    return this->m_valid;
  }

  template<typename OStream>
  friend OStream &operator<<(OStream &os, point_t const &p)
  {
    return os << p.str();
  }


  static auto pos_to_xy(std::size_t const p, std::size_t const grid_width) -> std::pair<std::size_t, std::size_t>
  {
    if (grid_width == 0)
    {
      return std::pair<std::size_t, std::size_t>{};
    }

    std::size_t const col = p % grid_width;
    std::size_t const row = p / grid_width;

    return std::make_pair(col, row);
  }

  static auto xy_to_pos(std::size_t const x, std::size_t const y, std::size_t const grid_width) -> std::size_t
  {
    if (grid_width == 0)
    {
      return std::size_t{};
    }

    std::size_t p = (grid_width * y) + x;
    return p;
  };

  static auto xy_to_pos(std::pair<std::size_t, std::size_t> xy, std::size_t const grid_width) -> std::size_t
  {
    if (grid_width == 0)
    {
      return std::size_t{};
    }

    std::size_t p = (grid_width * xy.second) + xy.first;
    return p;
  };

  static auto col_row_to_pos(std::size_t const col, std::size_t const row, std::size_t const grid_width) -> std::size_t
  {
    if (grid_width == 0)
    {
      return std::size_t{};
    }

    std::size_t p = (grid_width * row) + col;
    return p;
  };
};

} // namespace pnkd
