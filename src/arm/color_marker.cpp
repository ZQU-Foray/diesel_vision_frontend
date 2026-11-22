#include "diesel_vision_frontend/arm/color_marker.hpp"
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>

namespace diesel_vision_frontend {
ColorMarkerResult detect_color_marker(const cv::Mat &image,
                                      const cv::Matx33d &k, double size,
                                      std::uint64_t stamp) {
  ColorMarkerResult out;
  out.reason = "invalid_input";
  if (image.empty() || image.type() != CV_8UC3 || !std::isfinite(size) ||
      size <= 0 || stamp == 0 || !cv::checkRange(cv::Mat(k)) || k(0, 0) <= 0 ||
      k(1, 1) <= 0)
    return out;
  const std::array<cv::Scalar, 4> low = {
      cv::Scalar(0, 0, 160), cv::Scalar(0, 160, 0), cv::Scalar(160, 0, 0),
      cv::Scalar(0, 160, 160)};
  const std::array<cv::Scalar, 4> high = {
      cv::Scalar(60, 60, 255), cv::Scalar(60, 255, 60), cv::Scalar(255, 60, 60),
      cv::Scalar(60, 255, 255)};
  std::vector<cv::Point2d> pixels;
  for (int color = 0; color < 4; ++color) {
    cv::Mat mask;
    cv::inRange(image, low[color], high[color], mask);
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL,
                     cv::CHAIN_APPROX_SIMPLE);
    std::vector<cv::Point2d> points;
    for (const auto &contour : contours) {
      // 图像噪声过滤的仿真参数；至少5px²，非真实颜色阈值标定。
      if (cv::contourArea(contour) < 5)
        continue;
      const auto moments = cv::moments(contour);
      if (moments.m00 > 0)
        points.emplace_back(moments.m10 / moments.m00,
                            moments.m01 / moments.m00);
    }
    if (points.size() != 1) {
      out.reason = "missing_or_ambiguous_color=" + std::to_string(color);
      return out;
    }
    out.centers[color] = points.front();
    pixels.push_back(points.front());
  }
  const double half = size / 2;
  const std::vector<cv::Point3d> object = {
      {-half, half, 0}, {half, half, 0}, {half, -half, 0}, {-half, -half, 0}};
  cv::Vec3d rotation, translation;
  try {
    if (!cv::solvePnP(object, pixels, k, cv::Mat(), rotation, translation,
                      false, cv::SOLVEPNP_ITERATIVE)) {
      out.reason = "pnp_failed";
      return out;
    }
    cv::Matx33d matrix;
    cv::Rodrigues(rotation, matrix);
    std::vector<cv::Point2d> projected;
    cv::projectPoints(object, rotation, translation, k, cv::Mat(), projected);
    double squared = 0;
    for (int i = 0; i < 4; ++i) {
      const auto error = projected[i] - pixels[i];
      squared += error.dot(error);
      if ((matrix * cv::Vec3d(object[i].x, object[i].y, 0) + translation)[2] <=
          0) {
        out.reason = "behind_camera";
        return out;
      }
    }
    const double reprojection = std::sqrt(squared / 4);
    if (!std::isfinite(reprojection) || reprojection > 1.0) {
      out.reason = "reprojection_exceeded";
      return out;
    }
    out.observation = {matrix, translation, stamp, 1.0, reprojection, true};
    if (!valid_arm_observation(out.observation)) {
      out.observation.valid = false;
      out.reason = "invalid_pose";
      return out;
    }
    out.reason = "ok";
  } catch (const cv::Exception &) {
    out.reason = "pnp_exception";
  }
  return out;
}
} // namespace diesel_vision_frontend
