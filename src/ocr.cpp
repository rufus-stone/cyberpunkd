#include "ocr.hpp"

#include <spdlog/spdlog.h>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

namespace pnkd
{

auto get_string_from_image(cv::Mat img, std::string tessdata_path) -> std::string
{
  auto ocr = std::make_unique<tesseract::TessBaseAPI>();
  ocr->Init(tessdata_path.c_str(), "eng", tesseract::OEM_LSTM_ONLY); //ocr->Init(NULL, "eng", tesseract::OEM_LSTM_ONLY);

  ocr->SetPageSegMode(tesseract::PSM_AUTO);
  ocr->SetImage(img.data, img.cols, img.rows, 3, img.step);

  auto const img_text = std::string{ocr->GetUTF8Text()};

  ocr->End(); // Is this need if we're using unique_ptr??

  return img_text;
}

} // namespace pnkd
