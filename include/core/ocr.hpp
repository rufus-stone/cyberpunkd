#pragma once

#include <string>
#include <optional>
#include <vector>

#include <opencv2/opencv.hpp>
#include <spdlog/spdlog.h>

#include "game/goal.hpp"

namespace pnkd
{

class ocr_t
{
private:
  std::string m_tessdata_dir{};

public:
  explicit ocr_t(std::string const &tessdate_dir = "tessdata");

  auto get_string_from_image(cv::Mat const &raw_img, std::string const &char_list = "BD5E91C7AF \n") -> std::string;
  auto get_grid_from_img(cv::Mat const &raw_img) -> std::vector<std::string>;
  auto get_goal_list_from_img(cv::Mat const &raw_img) -> pnkd::goal_list_t;


  static auto fix_ocr(std::vector<std::string> const &input) -> std::vector<std::string>
  {
    auto output = input;

    // Fix any mistakes
    for (auto &segment : output)
    {
      // The OCR sometimes confuses 1C for 1D, and BD for BC, so fix this if needed
      if (segment == "1D")
      {
        segment = "1C";
      } else if (segment == "BC")
      {
        segment = "BD";
      }
    }

    return output;
  }

  static auto preprocess_image(cv::Mat const &raw_img, double w_scale, double x_scale, double h_scale, double y_scale, double thresh = 80) -> cv::Mat
  {
    cv::Mat output;

    // Blur slightly
    cv::bilateralFilter(raw_img, output, 9, 75, 75);

    // Convert to grayscale
    cv::cvtColor(output, output, cv::COLOR_RGB2GRAY);

    // Convert to black/white only
    cv::threshold(output, output, thresh, 255, cv::THRESH_BINARY_INV);

    // How big is the image?
    std::size_t const width = output.size().width;
    std::size_t const height = output.size().height;
    spdlog::debug("Image resolution: {}x{}", width, height);

    // Calc dimensions of relevant section of image (origin is top left)
    auto const w = width * w_scale;
    auto const x = 0 + (width * x_scale);

    auto const h = height * h_scale;
    auto const y = 0 + (height * y_scale);

    // Rect to use as cropping mask
    auto const roi = cv::Rect(x, y, w, h);

    // Crop the image
    output = output(roi);

    // Erode - MORPH_RECT / MORPH_CROSS / MORPH_ELLIPSE (MORPH_ELLIPSE produces best results)
    cv::erode(output, output, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size{3, 3}));

    // Increase the image size
    cv::resize(output, output, cv::Size{}, 2, 2);

    // Dilate - MORPH_RECT / MORPH_CROSS / MORPH_ELLIPSE (MORPH_ELLIPSE produces best results)
    cv::dilate(output, output, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size{3, 7}));

    return output;
  }
};

} // namespace pnkd