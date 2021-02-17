#include "utils/config.hpp"

namespace pnkd::cfg
{

config_helper::config_helper()
{
  this->m_config_path = pnkd::cfg::config_helper::locate_config();
}

config_helper::config_helper(std::string const &config_path)
{
  this->m_config_path = pnkd::cfg::config_helper::locate_config(config_path);
}

////////////////////////////////////////////////////////////////
auto config_helper::load_config() -> json
{
  auto const path = this->m_config_path;

  // Does the path actually exist/can we see it?
  if (!std::filesystem::exists(path) || !std::filesystem::is_regular_file(path))
  {
    spdlog::error("Couldn't find config file at: {}", path.string());
    return json{};
  }

  // Open the config file
  auto config = std::ifstream{path};

  try
  {
    auto config_json = json::parse(config);

    // Just in case we need to tidy up
    if (config.is_open())
    {
      config.close();
    }

    spdlog::info("Loaded config from file:\n{}", config_json.dump(2));

    this->m_json = config_json;

    return config_json;

  } catch (nlohmann::detail::parse_error const &e)
  {
    spdlog::error("Failed to parse config JSON! : {}", e.what());
    return json{};
  }
}

auto config_helper::write_config(json const &j) -> bool
{
  auto const path = this->m_config_path;

  // Check if the path exists
  if (std::filesystem::exists(path))
  {
    spdlog::info("Writing config to file:\n{}", j.dump(2));
    // Write the json to file
    auto config_file = std::ofstream{path};
    config_file << j.dump(2);
    config_file.close();

    return true;

  } else
  {
    spdlog::warn("Config file moved or deleted?");
    return false;
  }
}

auto config_helper::config_path() const -> std::filesystem::path
{
  return this->m_config_path;
}

auto config_helper::cfg_json() -> json &
{
  return this->m_json;
}

} // namespace pnkd::cfg
