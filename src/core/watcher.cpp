#include "core/watcher.hpp"

#include <thread>
#include <future>
#include <chrono>

#include <opencv2/opencv.hpp>

#include <spdlog/spdlog.h>

#include "core/notifier.hpp"
#include "core/ocr.hpp"
#include "core/puzzler.hpp"
#include "utils/file_utils.hpp"
#include "utils/string_utils.hpp"

namespace pnkd
{

watcher::watcher(
  std::string const &screenshots_path,
  std::string const &tessdata_path,
  std::size_t buffer_size,
  std::size_t sleep_for) : m_screenshots_path(screenshots_path),
                           m_ocr({tessdata_path}),
                           m_buffer_size(buffer_size),
                           m_sleep_for(sleep_for)
{
  spdlog::info("Created new watcher - screenshots_path: {} / tessdata_dir: {} / buffer_size: {} / sleep_for: {}", screenshots_path, tessdata_path, buffer_size, sleep_for);
}

auto watcher::do_once() -> void
{
  auto const start = std::chrono::high_resolution_clock::now();

  auto const latest_image_path = pnkd::get_path_to_latest_screenshot(this->m_screenshots_path);

  if (latest_image_path == this->m_previous_image_path)
  {
    //spdlog::info("No new images yet...");
    //std::this_thread::sleep_for(1s);
    //continue;
    return;
  }

  spdlog::info("----------------------------");
  spdlog::info("Processing new screenshot: {}", std::filesystem::absolute(latest_image_path).string());

  // Load the latest image
  cv::Mat img = pnkd::get_latest_screenshot(this->m_screenshots_path);

  // Did we successfully load an image?
  if (img.empty())
  {
    spdlog::error("Failed to load image!");
    return; // TODO: Handle this better than just quitting
  }

  // OCR the grid
  auto const grid = this->m_ocr.get_grid_from_img(img); //pnkd::get_grid_from_img(img, tessdata_dir);

  // Did the OCR fail?
  if (grid.empty())
  {
    spdlog::error("Failed to extract any text from grid!");
    return; // TODO: Handle this better than just quitting
  }

  spdlog::info("Grid:\n\n{}\n", pnkd::grid_to_string(grid));

  // OCR the goals
  auto goal_list = this->m_ocr.get_goal_list_from_img(img);

  // Did the OCR fail?
  if (goal_list.empty())
  {
    spdlog::error("Failed to extract any text from goals!");
    return; // TODO: Handle this better than just quitting
  }

  spdlog::info("Target Sequences:\n\n{}\n", pnkd::goal_list_to_string(goal_list));

  spdlog::debug("Total goals: {}", goal_list.total());
  for (auto const &goal : goal_list)
  {
    spdlog::debug("{}", goal.str());
  }

  // Create our initial game state
  auto const initial_state = pnkd::game_state_t{grid, goal_list, this->m_buffer_size};

  // Create a puzzler
  auto puzzler = pnkd::puzzler{initial_state};

  // Solve asynchronously
  auto fut = std::async(std::launch::async, &puzzler::solve, puzzler);
  auto const solutions = fut.get();

  // TODO: Inform user of optimal solutions
  pnkd::show_solutions(solutions);

  // How long did it take?
  auto const finish = std::chrono::high_resolution_clock::now();
  auto const nanoseconds_taken = (finish - start);
  auto const milliseconds_taken = std::chrono::duration<double, std::milli>(nanoseconds_taken).count();

  spdlog::info("Solved in {:.0f}ms", milliseconds_taken);

  this->m_previous_image_path = latest_image_path;
}

auto watcher::start() -> bool
{
  using namespace std::chrono_literals;

  while (this->m_should_watch)
  {
    this->do_once();

    std::this_thread::sleep_for(std::chrono::seconds{this->m_sleep_for}); // Give it at least a second before checking for new screenshots, to avoid hammering the CPU
  }
  /*

  while (this->m_should_watch)
  {
    auto const start = std::chrono::high_resolution_clock::now();

    auto const latest_image_path = pnkd::get_path_to_latest_screenshot(this->m_screenshots_path);

    if (latest_image_path == this->m_previous_image_path)
    {
      //spdlog::info("No new images yet...");
      std::this_thread::sleep_for(1s);
      continue;
    }

    spdlog::info("----------------------------");
    spdlog::info("Processing new screenshot: {}", std::filesystem::absolute(latest_image_path).string());

    // Load the latest image
    cv::Mat img = pnkd::get_latest_screenshot(this->m_screenshots_path);

    // Did we successfully load an image?
    if (img.empty())
    {
      spdlog::error("Failed to load image!");
      return false; // TODO: Handle this better than just quitting
    }

    // OCR the grid
    auto const grid = this->m_ocr.get_grid_from_img(img); //pnkd::get_grid_from_img(img, tessdata_dir);

    // Did the OCR fail?
    if (grid.empty())
    {
      spdlog::error("Failed to extract any text from grid!");
      return false; // TODO: Handle this better than just quitting
    }

    spdlog::info("Grid:\n\n{}\n", pnkd::grid_to_string(grid));

    // OCR the goals
    auto goal_list = this->m_ocr.get_goal_list_from_img(img);

    // Did the OCR fail?
    if (goal_list.empty())
    {
      spdlog::error("Failed to extract any text from goals!");
      return false; // TODO: Handle this better than just quitting
    }

    spdlog::info("Target Sequences:\n\n{}\n", pnkd::goal_list_to_string(goal_list));

    spdlog::debug("Total goals: {}", goal_list.total());
    for (auto const &goal : goal_list)
    {
      spdlog::debug("{}", goal.str());
    }

    // Create our initial game state
    auto const initial_state = pnkd::game_state_t{grid, goal_list, this->m_buffer_size};

    // Create a puzzler
    auto puzzler = pnkd::puzzler{initial_state};

    // Solve asynchronously
    auto fut = std::async(std::launch::async, &puzzler::solve, puzzler);
    auto const solutions = fut.get();

    // TODO: Inform user of optimal solutions
    pnkd::show_solutions(solutions);

    // How long did it take?
    auto const finish = std::chrono::high_resolution_clock::now();
    auto const nanoseconds_taken = (finish - start);
    auto const milliseconds_taken = std::chrono::duration<double, std::milli>(nanoseconds_taken).count();

    spdlog::info("Solved in {:.0f}ms", milliseconds_taken);

    this->m_previous_image_path = latest_image_path;

    std::this_thread::sleep_for(std::chrono::seconds{this->m_sleep_for}); // Give it at least a second before checking for new screenshots, to avoid hammering the CPU
  }
  */

  return true;
}

auto watcher::stop() -> void
{
  spdlog::info("Stopping...");
  this->m_should_watch = false;
}

auto watcher::set_screenshots_path(std::string const &s) -> void
{
  this->m_screenshots_path = s;
}

auto watcher::set_tessdata_path(std::string const &s) -> void
{
  this->m_tessdata_path = s;
  this->m_ocr.set_tessdata_dir(s);
}

auto watcher::set_buffer_size(std::size_t s) -> void
{
  this->m_buffer_size = s;
}

auto watcher::set_sleep_for(std::size_t s) -> void
{
  this->m_sleep_for = s;
}

auto watcher::forget_previous_image() -> void
{
  this->m_previous_image_path.clear();
}

} // namespace pnkd
