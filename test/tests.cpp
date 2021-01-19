#define CATCH_CONFIG_MAIN // This tells the Catch2 header to generate a main

#include <string>
#include <filesystem>

#include <spdlog/spdlog.h>
#include <opencv2/opencv.hpp>

#include "catch.hpp"

#include "utils/file_utils.hpp"
#include "utils/string_utils.hpp"
#include "ocr.hpp"

#include "sample_image.hpp"

////////////////////////////////////////////////////////////////
SCENARIO("Tesseract trained data files", "[ocr][tessdata]")
{
  spdlog::set_level(spdlog::level::debug);

  GIVEN("A user-specified path to the tessdata folder")
  {
    WHEN(" the path is not a valid, extant directory")
    {
      THEN(" pnkd::contains_tessdata() will be false")
      {
        REQUIRE(pnkd::contains_tessdata("invalid path") == false);
      }
    }
  }
}

// This isn't working on the Windows test environment - something wrong with finding tessdata folder
////////////////////////////////////////////////////////////////
SCENARIO("Tesseract on sample image", "[ocr][tessdata]")
{
  spdlog::set_level(spdlog::level::debug);

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
