#define CATCH_CONFIG_MAIN // This tells the Catch2 header to generate a main

#include <string>
#include <filesystem>

#include <spdlog/spdlog.h>
#include <opencv2/opencv.hpp>

#include "catch.hpp"

#include "utils/file_utils.hpp"
#include "utils/string_utils.hpp"
#include "ocr.hpp"
#include "game/goal.hpp"
#include "game/state.hpp"
#include "game/puzzler.hpp"

#include "sample_image.hpp"

////////////////////////////////////////////////////////////////
SCENARIO("Tesseract trained data files", "[ocr][tessdata]")
{
  spdlog::set_level(spdlog::level::off);

  GIVEN("A user-specified path to the tessdata folder")
  {
    WHEN(" the path is NOT a valid, extant directory")
    {
      THEN(" pnkd::contains_tessdata() will be false")
      {
        REQUIRE(pnkd::contains_tessdata("invalid path") == false);
      }
    }

    WHEN(" the path is a valid, extant directory")
    {
      THEN(" pnkd::contains_tessdata() will be true")
      {
        REQUIRE(pnkd::contains_tessdata("tessdata") == true);
      }
    }
  }
}

////////////////////////////////////////////////////////////////
SCENARIO("Tesseract on sample image", "[ocr][tessdata]")
{
  spdlog::set_level(spdlog::level::off);

  GIVEN("A valid sample image")
  {
    cv::Mat img = cv::imdecode(cv::InputArray{pnkd::sample_img, pnkd::sample_img_size}, cv::IMREAD_COLOR);

    WHEN(" the grid is fed to Tesseract")
    {
      auto const grid = pnkd::get_grid_from_img(img);

      THEN(" the correct grid data is identified")
      {
        spdlog::info("OCR:\n\n{}\n", pnkd::grid_to_string(grid));
        REQUIRE(grid == std::vector<std::string>{"1C", "55", "1C", "1C", "1C", "1C", "1C", "55", "BD", "1C", "55", "55", "BD", "E9", "1C", "BD", "1C", "1C", "1C", "55", "E9", "1C", "55", "E9", "1C"});
      }
    }

    WHEN(" the goals are fed to Tesseract")
    {
      auto const goal_list = pnkd::get_goal_list_from_img(img);

      THEN(" the correct goals are identified")
      {
        spdlog::info("OCR:\n\n{}\n", pnkd::goal_list_to_string(goal_list));
        REQUIRE(goal_list[0].str() == "55 1C");
        REQUIRE(goal_list[1].str() == "1C 1C 1C");
      }
    }
  }
}


////////////////////////////////////////////////////////////////
SCENARIO("Path solving", "[puzzler]")
{
  spdlog::set_level(spdlog::level::off);

  GIVEN("A 5x5 grid, a set of 3 goals, and a buffer size of 6")
  {
    // clang-format off
    auto const test_grid = std::vector<std::string>{
      "1C", "BD", "BD", "1C", "BD", 
      "E9", "55", "E9", "55", "BD", 
      "BD", "1C", "E9", "55", "BD", 
      "BD", "55", "55", "1C", "BD", 
      "55", "BD", "55", "55", "1C"};

    auto const goals = std::vector<std::vector<std::string>>{
      {"1C", "BD"}, 
      {"E9", "55", "1C"}, 
      {"55", "55", "E9"}};
    // clang-format on

    auto const test_goal_list = pnkd::goal_list_t{goals};

    constexpr std::size_t buffer_size = 6;

    WHEN(" the the game state is initialised and the puzzler activated")
    {
      auto const initial_state = pnkd::game_state_t{test_grid, test_goal_list, buffer_size};

      auto puzzler = pnkd::puzzler{initial_state};
      auto const solutions = puzzler.solve();

      THEN(" the optimal solutions are identified")
      {
        // The 1st goal "1C BD" can be solved in 2 moves
        std::size_t moves_for_1c_bd = std::max_element(std::begin(solutions.at(1).goals()), std::end(solutions.at(1).goals()), [](auto const &lhs, auto const &rhs) { return lhs.moves_taken() < rhs.moves_taken(); })->moves_taken();
        REQUIRE(moves_for_1c_bd == 2);

        // The 2nd goal "E9 55 1C" can be solved in 4 moves
        std::size_t moves_for_e9_55_1c = std::max_element(std::begin(solutions.at(2).goals()), std::end(solutions.at(2).goals()), [](auto const &lhs, auto const &rhs) { return lhs.moves_taken() < rhs.moves_taken(); })->moves_taken();
        REQUIRE(moves_for_e9_55_1c == 4);

        // The 2nd and 3rd goals "E9 55 1C" and "55 55 E9" can both be solved in 6 moves
        std::size_t moves_for_e9_55_1c_and_55_55_e9 = std::max_element(std::begin(solutions.at(6).goals()), std::end(solutions.at(6).goals()), [](auto const &lhs, auto const &rhs) { return lhs.moves_taken() < rhs.moves_taken(); })->moves_taken();
        REQUIRE(moves_for_e9_55_1c_and_55_55_e9 == 6);
      }
    }
  }
}
