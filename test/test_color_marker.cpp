#include "diesel_vision_frontend/arm/color_marker.hpp"
#include <gtest/gtest.h>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>
using namespace diesel_vision_frontend;
namespace {
const cv::Matx33d k(600, 0, 320, 0, 600, 240, 0, 0, 1);
cv::Mat image() {
  cv::Mat frame(480, 640, CV_8UC3, cv::Scalar(0, 0, 0));
  const std::vector<cv::Point3d> object = {
      {-.02, .02, 0}, {.02, .02, 0}, {.02, -.02, 0}, {-.02, -.02, 0}};
  std::vector<cv::Point2d> pixels;
  cv::projectPoints(object, cv::Vec3d(0, 0, 0), cv::Vec3d(0, 0, .4), k,
                    cv::Mat(), pixels);
  const std::vector<cv::Scalar> colors = {
      {0, 0, 255}, {0, 255, 0}, {255, 0, 0}, {0, 255, 255}};
  for (int i = 0; i < 4; ++i)
    cv::circle(frame, pixels[i], 5, colors[i], -1);
  return frame;
}
} // namespace
TEST(ColorMarker, PixelMeasurementPnPRoundTrip) {
  auto result = detect_color_marker(image(), k, .04, 1);
  ASSERT_TRUE(result.observation.valid) << result.reason;
  EXPECT_NEAR(result.observation.t_camera_target[2], .4, 1e-5);
  EXPECT_LT(result.observation.reprojection_error_px, 1e-5);
}
TEST(ColorMarker, MissingOrDuplicateColorIsNotObservation) {
  cv::Mat blank(480, 640, CV_8UC3, cv::Scalar(0, 0, 0));
  EXPECT_FALSE(detect_color_marker(blank, k, .04, 1).observation.valid);
  auto duplicate = image();
  cv::circle(duplicate, cv::Point(50, 50), 5, cv::Scalar(0, 0, 255), -1);
  EXPECT_FALSE(detect_color_marker(duplicate, k, .04, 1).observation.valid);
}
TEST(ColorMarker, InvalidContractRejected) {
  EXPECT_FALSE(detect_color_marker(image(), k, 0, 1).observation.valid);
  EXPECT_FALSE(detect_color_marker(image(), k, .04, 0).observation.valid);
  auto invalid = k;
  invalid(0, 0) = 0;
  EXPECT_FALSE(detect_color_marker(image(), invalid, .04, 1).observation.valid);
}
