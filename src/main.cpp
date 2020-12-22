#include <sstream>

#include <spdlog/spdlog.h>
#include <docopt/docopt.h>
#include <opencv2/opencv.hpp>

#include "usage.hpp"
#include "file_utils.hpp"

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

  for (auto const &arg : args)
  {
    std::stringstream ss;
    ss << arg.second;

    spdlog::debug("{}: {}", arg.first, ss.str());
  }

  // Get the user-specified path to the screenshots folder
  std::string const path = args.at("<path>").asString();

  // Load the latest image
  cv::Mat img = pnkd::get_latest_screenshot(path);

  // Did we successfully load an image?
  if (img.empty())
  {
    spdlog::error("Failed to load image!");
    return EXIT_FAILURE;
  }

  // Just testing
  //cv::imshow("Display window", img);
  //cv::waitKey(0); // Wait for a keystroke in the window

  // TODO: Perform OCR

  // TODO: Parse OCR'd text

  // TODO: Solve paths

  // TODO: Message user with solution (Discord bot?)

  return EXIT_SUCCESS;
}