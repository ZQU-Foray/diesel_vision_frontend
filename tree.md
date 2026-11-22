# 目录结构说明

2026-11-21：`arm/color_marker.hpp`与`src/arm/color_marker.cpp`实现专用四色标记板像素检测与PnP；
`src/arm/marker_sim_bridge.cpp`为文件/文本适配，`test/test_color_marker.cpp`覆盖正常/漏标/重复色/非法输入。
不是YOLO通用抓取检测器；不读取仿真物体位姿，不做运动规划。

```text
.
├── include/diesel_vision_frontend/
│   ├── armor/   装甲板结果、双模型、图案匹配、PnP与处理接口
│   ├── arm/     机械臂完整相机位姿草案与校验接口
│   └── common/  TargetClass、Observation、Detector与注册表
├── src/
│   ├── armor/   装甲板实现；历史灯条几何仅供仿真适配
│   ├── arm/     机械臂观测校验，未实现真实识别器
│   └── common/  检测器注册与配置装配
├── test/        各模块独立无硬件测试
├── config/      检测器配置
├── tools/       可选相机调试入口
├── CMakeLists.txt / package.xml
├── README.md / plan.md / decision.md
└── AGENTS.md / .diesel-layer / .github/
```

## 包含路径与边界

2026-11-14：include与src同时按业务划分。公共包含路径是破坏性变更；仓内源码、测试和工具已同步，外部调用方需要同步修改。命名空间保持`diesel_vision_frontend`（机械臂后端使用其`::arm`子命名空间），库目标名称不变。不保留根目录旧路径转发头。

PnP仍归前端；双模型真实链路使用外轮廓四角，不转灯条端点。机械臂目前仅有位姿契约，不声称完成识别或抓取。

## 记录约束

不记录build/install/log/.git或缓存产物。
