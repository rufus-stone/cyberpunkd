#pragma once

#include <string>
#include <optional>

#include <opencv2/opencv.hpp>

#include "game/goal.hpp"

namespace pnkd
{

auto preprocess_image(cv::Mat const &raw_img, double w_scale, double x_scale, double h_scale, double y_scale, double thresh = 80) -> cv::Mat;

auto get_string_from_image(cv::Mat const &raw_img, std::string const &tessdata_path = "tessdata", std::string const &char_list = "BD5E91C7AF \n") -> std::string;

auto get_grid_from_img(cv::Mat const &raw_img, std::string const &tessdata_path = "tessdata") -> std::vector<std::string>;
auto get_goal_list_from_img(cv::Mat const &raw_img, std::string const &tessdata_path = "tessdata") -> pnkd::goal_list_t;

} // namespace pnkd