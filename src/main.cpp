#include <filesystem>
#include <sstream>
#include <queue>

#include <thread>
#include <chrono>

#include <spdlog/spdlog.h>
#include <docopt/docopt.h>
#include <opencv2/opencv.hpp>

#include "core/ocr.hpp"
#include "core/puzzler.hpp"
#include "core/usage.hpp"

#include "game/state.hpp"
#include "game/goal.hpp"

#include "utils/file_utils.hpp"
#include "utils/string_utils.hpp"


using namespace std::chrono_literals;

int main(int argc, const char **argv)
{
  auto const args = docopt::docopt(pnkd::usage, {std::next(argv), std::next(argv, argc)},
    true,                // show help if requested
    "cyberpunkd 0.1.0"); // version string

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

  // Get the buffer size screenshots folder
  std::size_t const buffer_size = static_cast<std::size_t>(args.at("<buffer_size>").asLong());
  spdlog::info("Buffer size: {}", buffer_size);

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

  // Start watching the screenshots folder
  auto previous_image_path = std::filesystem::path{};

  while (true)
  {
    auto const start = std::chrono::high_resolution_clock::now();

    auto const latest_image_path = pnkd::get_path_to_latest_screenshot(path);

    if (latest_image_path == previous_image_path)
    {
      //spdlog::info("No new images yet...");
      std::this_thread::sleep_for(1s);
      continue;
    }

    spdlog::info("----------------------------");
    spdlog::info("Processing new screenshot: {}", std::filesystem::absolute(latest_image_path).string());

    // Load the latest image
    cv::Mat img = pnkd::get_latest_screenshot(path);

    // Did we successfully load an image?
    if (img.empty())
    {
      spdlog::error("Failed to load image!");
      return EXIT_FAILURE; // TODO: Handle this better than just quitting
    }

    // OCR the grid
    auto const grid = pnkd::get_grid_from_img(img, tessdata_dir);

    // Did the OCR fail?
    if (grid.empty())
    {
      spdlog::error("Failed to extract any text from grid!");
      return EXIT_FAILURE; // TODO: Handle this better than just quitting
    }

    spdlog::info("grid_text:\n\n{}\n", pnkd::grid_to_string(grid));

    // OCR the goals
    auto goal_list = pnkd::get_goal_list_from_img(img, tessdata_dir);

    // Did the OCR fail?
    if (goal_list.empty())
    {
      spdlog::error("Failed to extract any text from goals!");
      return EXIT_FAILURE; // TODO: Handle this better than just quitting
    }

    spdlog::info("goal_text:\n\n{}\n", pnkd::goal_list_to_string(goal_list));

    spdlog::info("Total goals: {}", goal_list.total());
    for (auto const &goal : goal_list)
    {
      spdlog::info("{}", goal.str());
    }

    // Create our initial game state
    auto const initial_state = pnkd::game_state_t{grid, goal_list, buffer_size};

    // Create a puzzler and solve
    auto puzzler = pnkd::puzzler{initial_state};
    auto const solutions = puzzler.solve();

    // TODO: Inform user of optimal solutions

    auto const finish = std::chrono::high_resolution_clock::now();

    auto const nanoseconds_taken = (finish - start);
    auto const milliseconds_taken = std::chrono::duration<double, std::milli>(nanoseconds_taken).count();

    spdlog::info("Solved in {:.0f}ms", milliseconds_taken);

    previous_image_path = latest_image_path;

    std::this_thread::sleep_for(500ms); // Give it half a second before checking for new screenshots, to avoid hammering the CPU
  }

  return EXIT_SUCCESS;
}
