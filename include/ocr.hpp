#pragma once

#include <string>
#include <optional>

#include <opencv2/opencv.hpp>

namespace pnkd
{

auto preprocess_image(cv::Mat const &raw_img, double w_scale, double x_scale, double h_scale, double y_scale, double thresh = 80) -> cv::Mat;
auto preprocess_buffer(cv::Mat const &raw_img, double w_scale, double x_scale, double h_scale, double y_scale, double thresh) -> cv::Mat;

auto get_string_from_image(cv::Mat const &raw_img, std::string const &tessdata_path = "tessdata", std::string const &char_list = "BD5E91C7AF \n") -> std::string;

auto get_string_from_grid(cv::Mat const &raw_img, std::string const &tessdata_path = "tessdata") -> std::string;

auto get_string_from_goals(cv::Mat const &raw_img, std::string const &tessdata_path = "tessdata") -> std::string;

auto get_buffer_size(cv::Mat const &raw_img) -> std::optional<std::size_t>;

} // namespace pnkd