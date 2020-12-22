#pragma once

#include <string>

#include <opencv2/opencv.hpp>

namespace pnkd
{

auto get_string_from_image(cv::Mat img, std::string tessdata_path) -> std::string;

} // namespace pnkd