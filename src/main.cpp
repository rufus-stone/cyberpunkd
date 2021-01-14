#include <sstream>
#include <queue>

#include <spdlog/spdlog.h>
#include <docopt/docopt.h>
#include <opencv2/opencv.hpp>

#include "usage.hpp"
#include "utils/file_utils.hpp"
#include "utils/string_utils.hpp"
#include "ocr.hpp"
#include "game/puzzler.hpp"
#include "game/state.hpp"
#include "game/goal.hpp"

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
    spdlog::error("[❗] Cannot enable verbose logging AND quiet mode at the same time!");
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

  // Get the user-specified path to the tessdata folder, if present
  std::string const tessdata_dir = args.at("--tessdata").isString() ? args.at("--tessdata").asString() : "tessdata";
  spdlog::debug("Using tessdata path: {}", tessdata_dir);

  // Check that the tessdata is there
  if (!pnkd::contains_tessdata(tessdata_dir))
  {
    spdlog::error("[❗] Couldn't find tessdata!");
    return EXIT_FAILURE;
  }

  // Load the latest image
  cv::Mat img = pnkd::get_latest_screenshot(path);

  // Did we successfully load an image?
  if (img.empty())
  {
    spdlog::error("[❗] Failed to load image!");
    return EXIT_FAILURE;
  }

  //for (auto const &img : pnkd::get_all_screenshots(path))
  //{
  // Perform OCR
  // --------------------------
  // Get the text from the grid
  // --------------------------
  auto const grid_text = pnkd::get_string_from_grid(img, tessdata_dir); //pnkd::get_string_from_image(img, tessdata_dir);
  spdlog::info("grid_text:\n\n{}\n", grid_text);

  // Did the OCR generate any text?
  if (grid_text.empty())
  {
    spdlog::error("[❗] Failed to extract any text from grid!");
    return EXIT_FAILURE;
  }

  // Parse grid OCR text
  auto const grid = pnkd::split(grid_text, "\n ");

  // ---------------------------
  // Get the text from the goals
  // ---------------------------
  auto const goal_text = pnkd::get_string_from_goals(img, tessdata_dir);
  spdlog::info("goal_text:\n\n{}\n", goal_text);

  // Did the OCR generate any text?
  if (goal_text.empty())
  {
    spdlog::error("[❗] Failed to extract any text from goals!");
    return EXIT_FAILURE;
  }

  // Parse goals OCR text
  auto goal_list = pnkd::goal_list_t{}; //std::vector<pnkd::goal_t>{};
  auto const goals_lines = pnkd::split(goal_text, "\n");
  for (auto const &goal : goals_lines)
  {
    auto const segments = pnkd::split(goal, " ");

    auto this_goal = std::queue<std::string>{};

    auto tmp = std::string{};
    for (auto const &segment : segments)
    {
      // The OCR sometimes confuses 1C for 1D, so fix this if needed
      if (segment == "1D")
      {
        this_goal.push("1C");
        tmp += "1C ";
      } else
      {
        this_goal.push(segment);
        tmp = tmp + segment + " ";
      }
    }

    tmp = pnkd::strip(tmp);
    goal_list.emplace_back(pnkd::goal_t{this_goal, tmp});
  }
  goal_list.m_num_goals = goal_list.size();
  goal_list.m_goals_remaining = goal_list.m_num_goals;

  spdlog::info("goal_list.m_num_goals == {}", goal_list.m_num_goals);
  for (auto const &goal : goal_list)
  {
    spdlog::info(goal.str());
  }

  // Create our initial game state
  auto const initial_state = pnkd::game_state_t{grid, goal_list, buffer_size};

  // Create a puzzler and solve
  auto puzzler = pnkd::puzzler{initial_state};
  puzzler.solve();

  // TODO: Show optimal solution

  return EXIT_SUCCESS;
}