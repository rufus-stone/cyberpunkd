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

  // Convert the image to binary black/white (a.k.a. threshold)
  //cv::threshold(output, output, 70, 255, cv::THRESH_OTSU);
  cv::threshold(output, output, 70, 255, cv::THRESH_BINARY_INV);

  // How big is the image?
  std::size_t const width = output.size().width;
  std::size_t const height = output.size().height;
  spdlog::debug("Image resolution: {}x{}", width, height);

  // Crop out the gubbins either side of the relevant section of the image
  // Origin is the top left
  auto const w = width * 0.19;
  auto const x = 0 + (width * 0.16);

  auto const h = height * 0.3;
  auto const y = 0 + (height * 0.315);

  // Rect to use as cropping mask
  auto const roi = cv::Rect(x, y, w, h);

  // Crop the image
  output = output(roi);

  // Increase the image size
  cv::resize(output, output, cv::Size{}, 2, 2);

  // Erode slightly - this seems to improve OCR accuracy
  std::size_t constexpr erosion_size = 1;
  auto kern = cv::getStructuringElement(cv::MORPH_ELLIPSE, // MORPH_RECT / MORPH_CROSS / MORPH_ELLIPSE
    cv::Size{2 * erosion_size + 1, 2 * erosion_size + 1});

  //cv::dilate(output, output, kern);
  cv::erode(output, output, kern);

  cv::imshow("Pre-proc", output);
  cv::waitKey();
  return output;
}


auto get_string_from_image(cv::Mat const &raw_img, std::string tessdata_path) -> std::string
{
  cv::Mat img = preprocess_image(raw_img);

  std::size_t constexpr bytes_per_pixel = 1; // Our preprocessed image is black and white, so only 1 byte. Set this to 3 for RGB, etc.

  auto ocr = std::make_unique<tesseract::TessBaseAPI>();
  ocr->Init(tessdata_path.c_str(), "eng", tesseract::OEM_LSTM_ONLY);

  ocr->SetImage(img.data, img.cols, img.rows, bytes_per_pixel, img.step);
  ocr->SetSourceResolution(300);

  auto const img_text = std::string{ocr->GetUTF8Text()};

  ocr->End(); // Is this needed if we're using unique_ptr??

  return img_text;
}

} // namespace pnkd
