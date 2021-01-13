#include "ocr.hpp"

#include <opencv2/opencv.hpp>

#include <spdlog/spdlog.h>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include "utils/string_utils.hpp"

namespace pnkd
{

auto preprocess_image(cv::Mat const &raw_img, double w_scale, double x_scale, double h_scale, double y_scale, double thresh) -> cv::Mat
{
  cv::Mat output;

  // Blur slightly
  cv::bilateralFilter(raw_img, output, 9, 75, 75); // 9, 75, 75

  // Convert to grayscale
  cv::cvtColor(output, output, cv::COLOR_RGB2GRAY);

  // Convert to black/white only
  cv::threshold(output, output, thresh, 255, cv::THRESH_BINARY_INV); // 70, 255, THRESH_BINARY_INV / THRESH_OTSU

  // Crop out the gubbins either side of the relevant section of the image
  // Origin is the top left
  // How big is the image?
  std::size_t const width = output.size().width;
  std::size_t const height = output.size().height;
  spdlog::debug("Image resolution: {}x{}", width, height);

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

  // Blur
  //cv::medianBlur(output, output, 3);

  //cv::imshow("Pre-proc", output);
  //cv::waitKey();
  return output;
}


auto preprocess_buffer(cv::Mat const &raw_img, double w_scale, double x_scale, double h_scale, double y_scale, double thresh) -> cv::Mat
{
  cv::Mat output = raw_img;

  // Convert to grayscale
  cv::cvtColor(output, output, cv::COLOR_RGB2GRAY);

  // Convert to black/white only
  cv::threshold(output, output, thresh, 255, cv::THRESH_BINARY_INV); // 70, 255, THRESH_BINARY_INV / THRESH_OTSU

  // Crop out the gubbins either side of the relevant section of the image
  // Origin is the top left
  // How big is the image?
  std::size_t const width = output.size().width;
  std::size_t const height = output.size().height;
  spdlog::debug("Image resolution: {}x{}", width, height);

  auto const w = width * w_scale;
  auto const x = 0 + (width * x_scale);

  auto const h = height * h_scale;
  auto const y = 0 + (height * y_scale);

  // Rect to use as cropping mask
  auto const roi = cv::Rect(x, y, w, h);

  // Crop the image
  output = output(roi);

  // Erode - MORPH_RECT / MORPH_CROSS / MORPH_ELLIPSE (MORPH_ELLIPSE produces best results)
  //cv::erode(output, output, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size{3, 3}), cv::Point{-1, -1}, 5);

  // Increase the image size
  //cv::resize(output, output, cv::Size{}, 2, 2);

  // Dilate - MORPH_RECT / MORPH_CROSS / MORPH_ELLIPSE (MORPH_ELLIPSE produces best results)
  //cv::dilate(output, output, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size{3, 7}));

  return output;
}


auto get_string_from_image(cv::Mat const &img, std::string const &tessdata_path, std::string const &char_list) -> std::string
{
  std::size_t constexpr bytes_per_pixel = 1; // Our preprocessed image is black and white, so only 1 byte. Set this to 3 for RGB, etc.

  auto ocr = std::make_unique<tesseract::TessBaseAPI>();
  ocr->Init(tessdata_path.c_str(), "eng", tesseract::OEM_TESSERACT_ONLY); // OEM_LSTM_ONLY / OEM_TESSERACT_ONLY

  ocr->SetImage(img.data, img.cols, img.rows, bytes_per_pixel, img.step);
  ocr->SetSourceResolution(300);

  ocr->SetVariable("tessedit_char_whitelist", char_list.c_str());

  auto const img_text = std::string{ocr->GetUTF8Text()};

  ocr->End();

  return pnkd::strip(img_text);
}

auto get_string_from_grid(cv::Mat const &raw_img, std::string const &tessdata_path) -> std::string
{
  cv::Mat img = pnkd::preprocess_image(raw_img, 0.19, 0.16, 0.35, 0.315);

  return get_string_from_image(img, tessdata_path);
}


auto get_string_from_goals(cv::Mat const &raw_img, std::string const &tessdata_path) -> std::string
{
  cv::Mat img = pnkd::preprocess_image(raw_img, 0.11, 0.437, 0.2, 0.3, 80);

  return get_string_from_image(img, tessdata_path);
}

auto get_buffer_size(cv::Mat const &raw_img) -> std::optional<std::size_t>
{
  cv::Mat img = pnkd::preprocess_buffer(raw_img, 0.237, 0.426, 0.0729, 0.16, 80);

  auto detector = cv::SimpleBlobDetector();

  std::vector<cv::KeyPoint> keypoints;
  detector.detect(img, keypoints);

  cv::Mat img_with_keypoints;
  cv::drawKeypoints(img, keypoints, img_with_keypoints, cv::Scalar(0, 0, 255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

  cv::imshow("img", img_with_keypoints);
  cv::waitKey();

  return std::nullopt;
}

} // namespace pnkd
