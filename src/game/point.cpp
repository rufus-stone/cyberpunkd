#include "game/point.hpp"

#include <cmath>
#include <sstream>

#include <spdlog/spdlog.h>

namespace pnkd
{

point_t::point_t() : m_pos(0), m_grid_size(25)
{
}

point_t::point_t(std::size_t const grid_size) : m_pos(0), m_grid_size(grid_size)
{
  std::size_t const grid_width = static_cast<std::size_t>(std::sqrt(grid_size));

  // We expect grids that are perfect squares
  if (grid_width * grid_width != grid_size)
  {
    spdlog::error("Invalid grid size: {}", grid_size);
    this->m_grid_width = 0;
    this->m_valid = false;
  } else
  {
    this->m_grid_width = grid_width;
    this->m_valid = true;
  }
}

point_t::point_t(std::size_t const pos, std::size_t const grid_size) : m_pos(pos), m_grid_size(grid_size)
{
  std::size_t const grid_width = static_cast<std::size_t>(std::sqrt(grid_size));

  // We expect grids that are perfect squares
  if (grid_width * grid_width != grid_size)
  {
    spdlog::error("Invalid grid size: {}", grid_size);
    this->m_grid_width = 0;
    this->m_valid = false;
  } else
  {
    this->m_grid_width = grid_width;
    this->m_valid = true;
  }

  //spdlog::info("New point {} on grid size: {} ({} x {})", pos, grid_size, grid_width, grid_width);
}

auto point_t::pos() const -> std::size_t
{
  return this->m_pos;
}

auto point_t::x() const -> std::size_t
{
  auto xy = pnkd::point_t::pos_to_xy(this->m_pos, this->m_grid_width);
  return xy.first;
}

auto point_t::y() const -> std::size_t
{
  auto xy = pnkd::point_t::pos_to_xy(this->m_pos, this->m_grid_width);
  return xy.second;
}

auto point_t::row() const -> std::size_t
{
  return this->y();
}

auto point_t::col() const -> std::size_t
{
  return this->x();
}

auto point_t::x_y() const -> std::pair<std::size_t, std::size_t>
{
  return pnkd::point_t::pos_to_xy(this->m_pos, this->m_grid_width);
}

auto point_t::col_row() const -> std::pair<std::size_t, std::size_t>
{
  return pnkd::point_t::pos_to_xy(this->m_pos, this->m_grid_width);
}

auto point_t::str() const -> std::string
{
  auto ss = std::stringstream{};

  if (this->m_valid)
  {
    ss << "(" << this->pos() << " [" << this->x() << "," << this->y() << "])";
  } else
  {
    ss << "(" << this->pos() << " [?,?])";
  }

  return ss.str();
}


} // namespace pnkd
