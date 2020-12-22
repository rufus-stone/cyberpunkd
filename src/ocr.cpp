#include "ocr.hpp"

#include <spdlog/spdlog.h>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

namespace pnkd
{

auto preprocess_image(cv::Mat const &raw_img) -> cv::Mat
{
  // First, make the image black/white instead of RGB
  cv::Mat output;
  cv::cvtColor(raw_img, output, cv::COLOR_RGB2GRAY);

  // Convert the image to binary black/white (a.k.a. threshold) and invert that so it's black text on white
  cv::threshold(output, output, 70, 255, cv::THRESH_OTSU); // cv::THRESH_BINARY_INV);

  // How big is the image?
  std::size_t const width = output.size().width;
  std::size_t const height = output.size().height;
  spdlog::debug("{}x{}", width, height);

  // Crop out the gubbins either side of the relevant section of the image
  auto const width_scale = 0.8;
  auto const new_width = width * width_scale;
  auto const x = (width - new_width) / 2;

  auto const height_scale = 0.5;
  auto const new_height = height * height_scale;
  auto const y = (height - new_height) / 2;

  // Rect to use as cropping mask
  auto const roi = cv::Rect(x, y, new_width, new_height);

  // Crop the image
  output = output(roi);

  cv::imshow("Pre-proc", output);
  cv::waitKey();
  return output;
}

// TODO: I think this might be easier if we did the grid and the requirements separately? Crop into two images?

auto get_string_from_image(cv::Mat const &raw_img, std::string tessdata_path) -> std::string
{
  cv::Mat img = preprocess_image(raw_img);

  std::size_t constexpr bytes_per_pixel = 1; // Our preprocessed image is black and white, so only 1 byte. Set this to 3 for RGB, etc.

  auto ocr = std::make_unique<tesseract::TessBaseAPI>();
  ocr->Init(tessdata_path.c_str(), "eng", tesseract::OEM_LSTM_ONLY);

  ocr->SetPageSegMode(tesseract::PSM_AUTO);
  ocr->SetImage(img.data, img.cols, img.rows, bytes_per_pixel, img.step);

  auto const img_text = std::string{ocr->GetUTF8Text()};

  ocr->End(); // Is this needed if we're using unique_ptr??

  return img_text;
}

} // namespace pnkd
