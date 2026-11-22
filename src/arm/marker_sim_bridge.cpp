#include "diesel_vision_frontend/arm/color_marker.hpp"
#include <iomanip>
#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

int main(int argc, char **argv) {
  try {
    if (argc != 9)
      throw std::invalid_argument(
          "image fx fy cx cy square_m stamp_ns annotated.png");
    cv::Mat image = cv::imread(argv[1]);
    cv::Matx33d k(std::stod(argv[2]), 0, std::stod(argv[4]), 0,
                  std::stod(argv[3]), std::stod(argv[5]), 0, 0, 1);
    const auto result = diesel_vision_frontend::detect_color_marker(
        image, k, std::stod(argv[6]), std::stoull(argv[7]));
    if (!result.observation.valid) {
      cv::imwrite(argv[8], image);
      std::cerr << result.reason << std::endl;
      return 2;
    }
    std::cout << std::setprecision(17) << "OBS ";
    for (double x : result.observation.t_camera_target.val)
      std::cout << x << " ";
    for (double x : result.observation.R_camera_target.val)
      std::cout << x << " ";
    std::cout << result.observation.reprojection_error_px << std::endl;
    for (int i = 0; i < 4; ++i) {
      cv::circle(image, result.centers[i], 6, cv::Scalar(255, 255, 255), 1);
      cv::putText(image, std::to_string(i),
                  result.centers[i] + cv::Point2d(6, 0),
                  cv::FONT_HERSHEY_SIMPLEX, .5, cv::Scalar(255, 255, 255));
    }
    if (!cv::imwrite(argv[8], image))
      throw std::runtime_error("cannot save annotation");
  } catch (const std::exception &error) {
    std::cerr << error.what() << std::endl;
    return 1;
  }
  return 0;
}
