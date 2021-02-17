#include "core/ocr.hpp"

#include <opencv2/opencv.hpp>

#include <spdlog/spdlog.h>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include "utils/string_utils.hpp"

namespace pnkd
{

ocr_t::ocr_t(std::string const &tessdate_dir) : m_tessdata_dir(tessdate_dir) {}


auto ocr_t::get_string_from_image(cv::Mat const &img, std::string const &char_list) -> std::string
{
  std::size_t constexpr bytes_per_pixel = 1; // Our preprocessed image is black and white, so only 1 byte. Set this to 3 for RGB, etc.

  auto ocr = std::make_unique<tesseract::TessBaseAPI>();
  ocr->Init(this->m_tessdata_dir.c_str(), "eng", tesseract::OEM_TESSERACT_ONLY); // OEM_LSTM_ONLY / OEM_TESSERACT_ONLY

  ocr->SetImage(img.data, img.cols, img.rows, bytes_per_pixel, img.step);
  ocr->SetSourceResolution(300);

  ocr->SetVariable("tessedit_char_whitelist", char_list.c_str());

  char *text = ocr->GetUTF8Text();
  auto const img_text = std::string{text};
  delete[] text; // The Tesseract API requires us to delete[] the pointer ourselves to avoid a memory leak

  ocr->End();

  return pnkd::strip(img_text);
}

auto ocr_t::get_grid_from_img(cv::Mat const &raw_img) -> std::vector<std::string>
{
  cv::Mat img = pnkd::ocr_t::preprocess_image(raw_img, 0.19, 0.16, 0.35, 0.315);

  auto const grid_text = this->get_string_from_image(img);

  // Did the OCR generate any text?
  if (grid_text.empty())
  {
    spdlog::error("Failed to extract any text from grid!");
    return std::vector<std::string>{};
  }

  auto grid = pnkd::split(grid_text, "\n ");

  return fix_ocr(grid);
}


auto ocr_t::get_goal_list_from_img(cv::Mat const &raw_img) -> pnkd::goal_list_t
{
  cv::Mat img = pnkd::ocr_t::preprocess_image(raw_img, 0.11, 0.437, 0.25, 0.3, 80);

  auto const goal_text = this->get_string_from_image(img);

  // Did the OCR generate any text?
  if (goal_text.empty())
  {
    spdlog::error("Failed to extract any text from goals!");
    return pnkd::goal_list_t{};
  }

  // Parse goals OCR text
  auto goal_list = pnkd::goal_list_t{};
  auto const goals_lines = pnkd::split(goal_text, "\n");
  std::size_t goal_num = 0;
  for (auto const &goal : goals_lines)
  {
    auto const segments = fix_ocr(pnkd::split(goal, " "));

    auto this_goal = std::queue<std::string>{};

    auto tmp = std::string{};
    for (auto const &segment : segments)
    {
      tmp += segment + " ";
      this_goal.push(segment);
    }

    tmp = pnkd::strip(tmp);
    goal_list.emplace_back(pnkd::goal_t{this_goal, tmp, goal_num++});
  }

  goal_list.init(); // This sets the total number of goals and the number of goals remaining from the size of the internal vector

  return goal_list;
}


auto ocr_t::set_tessdata_dir(std::string const &s) -> void
{
  this->m_tessdata_dir = s;
}


/*
auto preprocess_image(cv::Mat const &raw_img, double w_scale, double x_scale, double h_scale, double y_scale, double thresh) -> cv::Mat
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

auto get_string_from_image(cv::Mat const &img, std::string const &tessdata_path, std::string const &char_list) -> std::string
{
  std::size_t constexpr bytes_per_pixel = 1; // Our preprocessed image is black and white, so only 1 byte. Set this to 3 for RGB, etc.

  auto ocr = std::make_unique<tesseract::TessBaseAPI>();
  ocr->Init(tessdata_path.c_str(), "eng", tesseract::OEM_TESSERACT_ONLY); // OEM_LSTM_ONLY / OEM_TESSERACT_ONLY

  ocr->SetImage(img.data, img.cols, img.rows, bytes_per_pixel, img.step);
  ocr->SetSourceResolution(300);

  ocr->SetVariable("tessedit_char_whitelist", char_list.c_str());

  char *text = ocr->GetUTF8Text();
  auto const img_text = std::string{text};
  delete[] text; // The Tesseract API requires us to delete[] the pointer ourselves to avoid a memory leak

  ocr->End();

  return pnkd::strip(img_text);
}

auto fix_ocr(std::vector<std::string> const &input) -> std::vector<std::string>
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

auto get_grid_from_img(cv::Mat const &raw_img, std::string const &tessdata_path) -> std::vector<std::string>
{
  cv::Mat img = pnkd::preprocess_image(raw_img, 0.19, 0.16, 0.35, 0.315);

  auto const grid_text = get_string_from_image(img, tessdata_path);

  // Did the OCR generate any text?
  if (grid_text.empty())
  {
    spdlog::error("Failed to extract any text from grid!");
    return std::vector<std::string>{};
  }

  auto grid = pnkd::split(grid_text, "\n ");

  return fix_ocr(grid);
}


auto get_goal_list_from_img(cv::Mat const &raw_img, std::string const &tessdata_path) -> pnkd::goal_list_t
{
  cv::Mat img = pnkd::preprocess_image(raw_img, 0.11, 0.437, 0.25, 0.3, 80);

  auto const goal_text = get_string_from_image(img, tessdata_path);

  // Did the OCR generate any text?
  if (goal_text.empty())
  {
    spdlog::error("Failed to extract any text from goals!");
    return pnkd::goal_list_t{};
  }

  // Parse goals OCR text
  auto goal_list = pnkd::goal_list_t{};
  auto const goals_lines = pnkd::split(goal_text, "\n");
  std::size_t goal_num = 0;
  for (auto const &goal : goals_lines)
  {
    auto const segments = fix_ocr(pnkd::split(goal, " "));

    auto this_goal = std::queue<std::string>{};

    auto tmp = std::string{};
    for (auto const &segment : segments)
    {
      tmp += segment + " ";
      this_goal.push(segment);
    }

    tmp = pnkd::strip(tmp);
    goal_list.emplace_back(pnkd::goal_t{this_goal, tmp, goal_num++});
  }

  goal_list.init(); // This sets the total number of goals and the number of goals remaining from the size of the internal vector

  return goal_list;
}
*/

} // namespace pnkd
