#pragma once
#include "diesel_vision_frontend/arm/observation.hpp"
#include <array>
#include <string>

namespace diesel_vision_frontend {
struct ColorMarkerResult {
  ArmObservation observation;
  std::array<cv::Point2d, 4> centers;
  std::string reason;
};
/// 专用四色标记板；颜色顺序红/绿/蓝/黄，对应局部(-x,+y),(+x,+y),(+x,-y),(-x,-y)。
/// square_m为圆心间距而非外框；相机无畸变。缺失/多义/几何或PnP异常不产有效观测。
ColorMarkerResult detect_color_marker(const cv::Mat &bgr,
                                      const cv::Matx33d &intrinsics,
                                      double square_m, std::uint64_t stamp_ns);
} // namespace diesel_vision_frontend
