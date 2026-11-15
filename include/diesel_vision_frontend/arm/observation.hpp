#pragma once

#include <cmath>
#include <cstdint>
#include <opencv2/core.hpp>

namespace diesel_vision_frontend {

/// 机械臂目标的完整相机系位姿草案。R_camera_target将目标轴转换到相机轴。
/// 时间戳为同源采样时刻(ns)，单位m；valid=false时禁止消费数值。
/// 与装甲板Observation区分：装配需要完整旋转，不能只用一个板面法向代替。
struct ArmObservation {
    cv::Matx33d R_camera_target = cv::Matx33d::eye();
    cv::Vec3d t_camera_target{0.0, 0.0, 0.0};
    std::uint64_t stamp_ns = 0;
    double confidence = 0.0;
    double reprojection_error_px = 0.0;
    bool valid = false;
};

/// 仅检查几何/数值契约，不证明识别正确、PnP唯一或满足抓取精度。
bool valid_arm_observation(const ArmObservation &o);

} // namespace diesel_vision_frontend
