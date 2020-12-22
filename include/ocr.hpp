#pragma once

#include <string>

#include <opencv2/opencv.hpp>

namespace pnkd
{

auto preprocess_image(cv::Mat const &raw_img) -> cv::Mat;
auto get_string_from_image(cv::Mat const &raw_img, std::string tessdata_path) -> std::string;

} // namespace pnkd