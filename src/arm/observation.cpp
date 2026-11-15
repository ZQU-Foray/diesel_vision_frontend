#include "diesel_vision_frontend/arm/observation.hpp"

#include <cmath>

namespace diesel_vision_frontend {

bool valid_arm_observation(const ArmObservation &o) {
    if (!o.valid || o.stamp_ns == 0 || !std::isfinite(o.confidence) || o.confidence < 0.0 ||
        o.confidence > 1.0 || !std::isfinite(o.reprojection_error_px) ||
        o.reprojection_error_px < 0.0) {
        return false;
    }
    for (double v : o.R_camera_target.val) {
        if (!std::isfinite(v))
            return false;
    }
    for (double v : o.t_camera_target.val) {
        if (!std::isfinite(v))
            return false;
    }
    // 1e-6只用于旋转矩阵数值合法性，不是实机观测质量门限。
    return o.t_camera_target[2] > 0.0 &&
           cv::norm(cv::Mat(o.R_camera_target.t() * o.R_camera_target - cv::Matx33d::eye())) <=
               1e-6 &&
           std::abs(cv::determinant(cv::Mat(o.R_camera_target)) - 1.0) <= 1e-6;
}

} // namespace diesel_vision_frontend
