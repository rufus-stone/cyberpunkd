#include <filesystem>
#include <sstream>
#include <queue>

#include <thread>
#include <chrono>

#include <spdlog/spdlog.h>
#include <docopt/docopt.h>
#include <opencv2/opencv.hpp>

#include "core/notifier.hpp"
#include "core/ocr.hpp"
#include "core/puzzler.hpp"
#include "core/watcher.hpp"
#include "core/usage.hpp"

#include "game/state.hpp"
#include "game/goal.hpp"

#include "gui/gui.hpp"
#include "utils/file_utils.hpp"
#include "utils/string_utils.hpp"


using namespace std::chrono_literals;

auto main(int argc, const char **argv) -> int
{
  auto const args = docopt::docopt(pnkd::usage, {std::next(argv), std::next(argv, argc)},
    true,                // show help if requested
    "cyberpunkd 1.1.0"); // version string

  bool const verbose = args.at("--verbose").asBool();
  bool const quiet = args.at("--quiet").asBool();

  // Can't have BOTH verbose mode AND quiet mode!
  if (verbose && quiet)
  {
    spdlog::error("Cannot enable verbose logging AND quiet mode at the same time!");
    return EXIT_FAILURE;
  }

  if (verbose)
  {
    spdlog::set_level(spdlog::level::debug);
    spdlog::debug("Enabled verbose logging...");
  } else if (quiet)
  {
    spdlog::set_level(spdlog::level::err);
    spdlog::debug("Enabled quiet logging...");
  }

  // Print the args out
  for (auto const &arg : args)
  {
    std::stringstream ss;
    ss << arg.second;

    spdlog::debug("{}: {}", arg.first, ss.str());
  }

  // Get the buffer size - return 0 on failure
  auto get_buffer_size = [&args]() -> std::size_t {
    try
    {
      std::size_t const s = static_cast<std::size_t>(args.at("<buffer_size>").asLong());
      spdlog::info("Buffer size: {}", s);
      return s;
    } catch (...)
    {
      return 0;
    }
  };

  std::size_t const buffer_size = get_buffer_size();
  if (buffer_size == 0)
  {
    spdlog::error("Buffer size must be a number greater than 0!");
    return EXIT_FAILURE;
  }

  // Get the user-specified path to the screenshots folder
  std::string const path = args.at("<path>").asString();

  // Check it's a valid path
  if (!pnkd::is_valid_folder(path))
  {
    spdlog::error("Are you sure '{}' is the path to your screenshots directory?", path);
    return EXIT_FAILURE;
  }

  // Get the user-specified path to the tessdata folder, if present
  std::string const tessdata_dir = args.at("--tessdata").isString() ? args.at("--tessdata").asString() : "tessdata";
  spdlog::debug("Using tessdata path: {}", tessdata_dir);

  // Check that the tessdata is there
  if (!pnkd::contains_tessdata(tessdata_dir))
  {
    spdlog::error("Couldn't find tessdata!");
    return EXIT_FAILURE;
  }

  // Get the user-specified sleep duration, if present - default to 1 second
  std::size_t sleep_for = args.at("--sleep").isLong() ? args.at("--sleep").asLong() : 1;

  // Start watching the screenshots folder
  auto previous_image_path = std::filesystem::path{};

  bool const headless = args.at("--headless").asBool();

  if (!headless)
  {
    // Start the gui - this will handle the watcher, etc.
    pnkd::gui::start();

  } else
  {
    // Start the watcher directly
    auto watcher = pnkd::watcher{
      path,
      tessdata_dir,
      buffer_size,
      sleep_for};

    watcher.start();
  }

  return EXIT_SUCCESS;
}
