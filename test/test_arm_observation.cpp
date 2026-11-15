#include "diesel_vision_frontend/arm/observation.hpp"
#include <gtest/gtest.h>
#include <limits>

TEST(ArmObservation, ValidityAndGeometry) {
  using namespace diesel_vision_frontend;
  ArmObservation o;
  EXPECT_FALSE(valid_arm_observation(o));
  o.valid = true;
  o.stamp_ns = 1;
  o.t_camera_target[2] = 1;
  EXPECT_TRUE(valid_arm_observation(o));
  o.R_camera_target(0, 0) = -1;
  EXPECT_FALSE(valid_arm_observation(o));
  o.R_camera_target = cv::Matx33d::eye();
  o.t_camera_target[0] = std::numeric_limits<double>::quiet_NaN();
  EXPECT_FALSE(valid_arm_observation(o));
}
