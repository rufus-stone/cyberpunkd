#define CATCH_CONFIG_MAIN // This tells the Catch2 header to generate a main

#include <string>
#include <filesystem>

#include <spdlog/spdlog.h>
#include <opencv2/opencv.hpp>

#include "catch.hpp"

#include "file_utils.hpp"
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


////////////////////////////////////////////////////////////////
SCENARIO("Tesseract on sample image", "[ocr][tessdata]")
{
  spdlog::set_level(spdlog::level::debug);

  GIVEN("A valid sample image")
  {
    cv::Mat img = cv::imdecode(pnkd::sample_img, cv::IMREAD_COLOR);

    WHEN(" the grid is fed to Tesseract")
    {
      auto const img_text = pnkd::get_string_from_image(img);

      THEN(" the correct text is identified")
      {
        spdlog::info("OCR:\n\n{}\n", img_text);
        REQUIRE(img_text == "1C 55 1C 1C 1C\n1C 1C 55 BD 1C\n55 55 BD E9 1C\nBC 1C 1C 1C 55\nE9 1C 55 E9 1C");
      }
    }
  }
}
