#define CATCH_CONFIG_MAIN // This tells the Catch2 header to generate a main

#include <string>
#include <filesystem>

#include "catch.hpp"

#include "file_utils.hpp"

////////////////////////////////////////////////////////////////
SCENARIO("Tesseract trained data files", "[ocr][tessdata]")
{
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
